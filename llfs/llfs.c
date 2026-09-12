/**-----------------------------------------------------------------------------
 * Copyright (c) 2025 Sergey Sanders
 * sergey@sesadesign.com
 * -----------------------------------------------------------------------------
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ---------------------------------------------------------------------------*/

/**
 * @file llfs.c
 * @author Sergey Sanders
 * @date 19 Jul 2016
 * @brief LLFS
 *
 * Linked List File System Module.
 *
 * @see llfs_conf.h and llfs_drv_conf.h for details.
 */
#include "llfs.h"

#include "FreeRTOS.h"
#include "llfs_vol_eeprom.h"
#include "stdbool.h"
#include "string.h"
#include "task.h"

static uint16_t EmptyNext = 0; // free sector mark: COUNT and LINK both zero

lf_err_t lf_error;

void __attribute__((weak)) lf_init(void) {}

/**
 * @brief Format the volume: build the first index sector and release all data sectors.
 * @param size media size in bytes, clamped to the volume sector count
 * @param devID device ID stored in the physical descriptor
 * @param name volume name, LLFS_VOLNAME_LEN bytes are read as is, a terminator is not
 *        required and a shorter buffer must not be passed
 * @return LF_ERR_NONE on success, media error otherwise
 */
lf_err_t lf_format(uint32_t size, uint16_t devID, char *name)
{
   uint16_t sPtr = size / LLFS_SECTOR_SIZE; // requested sector count
   lf_phy_t tmpPhy = {
       .fsType = LLFS_VOLUME_VALID,
       .fsTypeN = (uint8_t)~LLFS_VOLUME_VALID,
       .devID = devID,
       .secCount = sPtr > volumeEEPROM->phy->secCount ? volumeEEPROM->phy->secCount : sPtr,
       .compress = LLFS_COMPRESS_NONE,
       .ecc = LLFS_ECC_NONE,
   };

   memcpy(tmpPhy.name, name, LLFS_VOLNAME_LEN);
   memset(volumeEEPROM->sData, LLFS_EMPTY_BYTE, LLFS_SECTOR_SIZE);
   memcpy(volumeEEPROM->sData, &tmpPhy, sizeof(lf_phy_t));
   volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] = LLFS_ATTR_INDEX; /// mark sector as an index.
   volumeEEPROM->sData[LLFS_SECTOR_NEXT_ADDR] = 0x00;            // first sector is the last one
   /// the mark byte is name[11] of the INDEX record, name[10] stays 0xff so the sector is never seen as free
   volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] = LLFS_MARK_INDEX_BYTE;

   if (!volumeEEPROM->write_sector(volumeEEPROM->sData, 0)) return lf_error;
   for (sPtr = 1; sPtr < tmpPhy.secCount; sPtr++) // release the data sectors, 0xff included: only an index may claim it
   {
      if (!(volumeEEPROM->set_sector_next(&EmptyNext, sPtr))) return lf_error;
   }
   return lf_error = LF_ERR_NONE;
}

#define LLFS_COUNT_UNUSED_AS_FREE 0

/**
 * @brief Get the free space of a volume.
 * @param volIndex volume index, ignored: the EEPROM is the only volume so far
 * @return free space in bytes
 */
uint32_t lf_get_free(uint8_t volIndex)
{
   uint32_t size = 0;
   uint16_t sPtr;
   uint16_t next;

   (void)volIndex;
   for (sPtr = 1; sPtr < volumeEEPROM->phy->secCount - 1; sPtr++) // 0 is an index, the last one is reserved for the second index
   {
      if (!volumeEEPROM->get_sector_next(&next, sPtr)) return 0;
      if (!next) size += LLFS_SECTOR_DATA_SIZE; // COUNT and LINK zero: the sector is free
#if LLFS_COUNT_UNUSED_AS_FREE
      else if (next >> 8 == sPtr)
         size += LLFS_SECTOR_DATA_SIZE - (next & 0xff); // partially filled: tail of the sector
#endif
   }
   return size;
}

/**
 * @brief Get the first free sector.
 * @param asIndex true when the sector is requested for an index sector: 0xff may be used,
 *        it never holds data because LINK 0xff marks the end of a chain
 * @return sector number, 0 if the volume is full
 */
static uint16_t lf_get_free_sector(bool asIndex)
{
   uint16_t sPtr;
   uint16_t next;

   if (asIndex && (volumeEEPROM->phy->secCount == LLFS_MAX_SECTOR_COUNT)) // full size volume: the last sector is kept for an index
   {
      if (!volumeEEPROM->get_sector_next(&next, LLFS_SECTOR_LAST)) return 0;
      if (!next) return LLFS_SECTOR_LAST;
   }
   for (sPtr = 1; sPtr < volumeEEPROM->phy->secCount - 1; sPtr++)
   {
      if (!volumeEEPROM->get_sector_next(&next, sPtr)) return 0;
      if (!next) return sPtr;
   }
   lf_error = LF_ERR_FULL;
   return 0;
}

/**
 * @brief Add a record to the first free slot, extending the index chain if needed.
 * @param newRecord receives a copy of the new record
 * @param name file name, truncated to LLFS_FILENAME_LEN characters
 * @param attr record attributes, a data sector is allocated for regular files only
 * @return record position: index sector in MSB, record number in LSB, 0 on failure
 */
static uint16_t lf_add_record(lf_record_t *newRecord, char *name, uint8_t attr)
{
   uint16_t indexBlock = 0;
   uint16_t next;
   uint16_t sect;
   uint8_t index;
   uint8_t rPtr;
   lf_record_t *iRecord;

   while (1)
   {
      if (!volumeEEPROM->read_sector(volumeEEPROM->sData, indexBlock)) return 0;
      /// an index sector carries the INDEX attribute in the last record and a zero mark byte
      if (!((volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] == LLFS_ATTR_INDEX) && (volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] == LLFS_MARK_INDEX_BYTE)))
      {
         lf_error = LF_ERR_VOLUME;
         return 0;
      }
      rPtr = indexBlock ? 0 : sizeof(lf_record_t); // record 0 of sector 0 holds lf_phy_t
      for (index = indexBlock ? 0 : 1; index < LLFS_RECORD_COUNT; index++)
      {
         iRecord = (lf_record_t *)&volumeEEPROM->sData[rPtr];
         if ((uint8_t)iRecord->name[0] == LLFS_EMPTY_BYTE) // erased slot, name[] is char: cast before comparing
         {
            uint8_t i;
            if (!(attr & (LLFS_ATTR_DIR | LLFS_ATTR_LINK | LLFS_ATTR_INDEX)))
            {
               if (!(sect = lf_get_free_sector(false))) return 0;
               /// sect points to the available sector
               next = sect << 8; // COUNT 0, LINK self: claimed and empty
               if (!(volumeEEPROM->set_sector_next(&next, sect))) return 0;
               iRecord->fptr = sect;
            }
            else
               iRecord->fptr = LLFS_SECTOR_LAST; // the iRecord is either directory,link or index
            iRecord->attr = attr;
            iRecord->rptr = 0; // root, directories are not implemented yet
            for (i = 0; i < LLFS_FILENAME_LEN && name[i]; i++)
               iRecord->name[i] = name[i];
            if (i < LLFS_FILENAME_LEN) iRecord->name[i] = '\0'; // all 12 chars used: no terminator, see lf_rname_tostr()
            if (!volumeEEPROM->write_sector(volumeEEPROM->sData, indexBlock)) return 0;
            memcpy(newRecord, iRecord, sizeof(lf_record_t));
            return (indexBlock << 8) + index;
         }
         rPtr += sizeof(lf_record_t);
      }
      /// the last record of an index sector points to the next one
      iRecord = (lf_record_t *)&volumeEEPROM->sData[LLFS_SECTOR_NEXT_ADDR];
      if ((iRecord->attr & LLFS_ATTR_INDEX) && iRecord->fptr)
      {
         indexBlock = iRecord->fptr;
         continue;
      }
      if (!(sect = lf_get_free_sector(true))) return 0;
      iRecord->fptr = sect;
      iRecord->attr = LLFS_ATTR_INDEX;
      if (!volumeEEPROM->write_sector(volumeEEPROM->sData, indexBlock)) return 0; // update current sector
      indexBlock = sect;
      memset(volumeEEPROM->sData, LLFS_EMPTY_BYTE, LLFS_SECTOR_SIZE);
      volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] = LLFS_ATTR_INDEX; /// mark sector as an index.
      volumeEEPROM->sData[LLFS_SECTOR_NEXT_ADDR] = 0x00;            /// mark as last one
      volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] = LLFS_MARK_INDEX_BYTE;
      if (!volumeEEPROM->write_sector(volumeEEPROM->sData, indexBlock)) return 0; // update new sector
   }
}

/**
 * @brief Erase a record.
 * @param pos record position as returned by lf_find_record()
 */
static void lf_del_record(uint16_t pos)
{
   uint8_t index = pos; // record number inside the index sector
   if (index >= LLFS_RECORD_COUNT)
   {
      lf_error = LF_ERR_NOTFOUND;
      return;
   }
   if (!volumeEEPROM->read_sector(volumeEEPROM->sData, pos >> 8)) return;
   if ((volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] == LLFS_ATTR_INDEX) && (volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] == LLFS_MARK_INDEX_BYTE))
   {
      memset((uint8_t *)(volumeEEPROM->sData + index * sizeof(lf_record_t)), LLFS_EMPTY_BYTE, sizeof(lf_record_t));
      volumeEEPROM->write_sector(volumeEEPROM->sData, pos >> 8); // update index sector
   }
   else
      lf_error = LF_ERR_VOLUME;
   return;
}

/**
 * @brief Match a record name against an expression.
 * @param fname record name, LLFS_FILENAME_LEN chars, zero terminated only if shorter
 * @param exp zero terminated expression, '*' stands for any number of characters
 * @return true on match
 */
static bool lf_match(char *fname, char *exp)
{
   uint8_t i = 0, ep = 0;                  // name and expression pointers
   uint8_t si = 0, sep = LLFS_SECTOR_LAST; // retry point after a '*', 0xff: no '*' seen yet

   if ((fname[0] < '!') || (fname[0] > '~')) return false; // wrong file name
   while (i < LLFS_FILENAME_LEN && fname[i])               // no terminator when all 12 chars are used
   {
      if (exp[ep] == '*')
      {
         sep = ++ep;
         si = i;
         continue;
      } // remember where to resume
      if (exp[ep] && (exp[ep] == fname[i]))
      {
         ep++;
         i++;
         continue;
      }
      if (sep == LLFS_SECTOR_LAST) return false; // mismatch and no '*' to fall back on
      ep = sep;
      i = ++si; // let the '*' swallow one more character
   }
   while (exp[ep] == '*')
      ep++;                         // trailing '*' matches an empty tail
   return (exp[ep]) ? false : true; // fail if there are unchecked haracter in expression
}

/**
 * @brief Find a record by name or expression.
 * @param name file name or expression, see lf_match()
 * @param record receives a copy of the record found
 * @param next false to start a new search, true to continue the previous one
 * @return record position: index sector in MSB, record number in LSB, 0 if not found
 * @note the search cursor is static, a search cannot be nested or shared between tasks
 */
uint16_t lf_find_record(char *name, lf_record_t *record, uint8_t next)
{
   uint8_t indexBlock;
   uint8_t index;
   static uint16_t lastPos = 0;
   if (!next || !lastPos) lastPos = 1;
   indexBlock = lastPos >> 8;
   index = lastPos;
   do
   {
      if (volumeEEPROM->read_sector(volumeEEPROM->sData, indexBlock) != LLFS_SECTOR_SIZE) return 0;
      if (!((volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] == LLFS_ATTR_INDEX) && (volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] == LLFS_MARK_INDEX_BYTE)))
      { // check that the sector is an index sector
         lf_error = LF_ERR_VOLUME;
         return 0;
      }
      for (; index < LLFS_RECORD_COUNT; index++)
      {
         if (lf_match(((lf_record_t *)&volumeEEPROM->sData[index * sizeof(lf_record_t)])->name, name))
         {
            memcpy(record, &volumeEEPROM->sData[index * sizeof(lf_record_t)], sizeof(lf_record_t));
            lastPos = (indexBlock << 8) + index + 1; // resume after this record
            return lastPos - 1;
         }
      }
      index = 0;
      indexBlock = volumeEEPROM->sData[LLFS_SECTOR_NEXT_ADDR]; // fptr of the INDEX record
   } while (indexBlock); // there is another index sector to check
   lastPos = 0; // no matches found
   lf_error = LF_ERR_NOTFOUND;
   return 0;
}

/**
 * @brief Convert a record name into a C string, this is the only safe way to read a name.
 * @param destStr destination buffer, LLFS_FILENAME_LEN+1 bytes
 * @param name lf_record_t::name, or any string to be truncated to LLFS_FILENAME_LEN
 * @return destStr, always zero terminated
 */
char *lf_rname_tostr(char *destStr, char *name)
{
   uint8_t i;

   for (i = 0; i < LLFS_FILENAME_LEN; i++)
   {
      destStr[i] = name[i];
      if (!destStr[i]) break;
   }
   destStr[i] = 0; // set string termination at the end.
   return destStr;
}
/**
 * @brief clear data on the volume begining from the pointer
 * @mode fPtr
 */
static void lf_clean_data(uint8_t fPtr)
{
   uint16_t next;
   while (fPtr && (fPtr != LLFS_SECTOR_LAST)) // 0 and 0xff are index sectors, they hold no data
   {
      if (!volumeEEPROM->get_sector_next(&next, fPtr)) return;
      if (!volumeEEPROM->set_sector_next(&EmptyNext, fPtr)) return;     // mark the sector as empty
      if (((next & 0xff00) == 0xff00) || ((next >> 8) == fPtr)) return; // last full or partially filled sector
      fPtr = next >> 8;                                                 // follow the chain
   }
}

/**
 * @brief Find the write position of the last sector of a chain.
 * @param fPtr first sector of the file
 * @return sector in MSB, offset in LSB, 0xff offset if the sector is full, 0 on error
 */
static uint16_t lf_find_eof(uint16_t fPtr)
{
   uint16_t next;
   while (fPtr && (fPtr != LLFS_SECTOR_LAST))
   {
      if (!volumeEEPROM->get_sector_next(&next, fPtr)) return 0;
      if ((next & 0xff00) == 0xff00) return (fPtr << 8) | LLFS_SECTOR_DATA_SIZE; // full: lf_write() chains a new sector
      if ((next >> 8) == fPtr) return (fPtr << 8) + (next & 0x00ff);             // partially filled: COUNT is the offset
      fPtr = next >> 8;
   }
   return 0;
}
/**
 * Get file size, identified by name or if empty, by fPtr (retrived by lf_find_record)
 * @param name file name, empty or NULL to use fPtr
 * @param fPtr first sector of the file, lf_record_t::fptr
 * @return file size in bytes
 */
uint32_t lf_get_fsize(char *name, uint16_t fPtr)
{
   uint16_t next;
   uint32_t size = 0;
   lf_record_t record;
   if (name && *name)
   {
      if (!lf_find_record(name, &record, false)) return 0;
      fPtr = record.fptr;
   }
   while (fPtr && (fPtr != LLFS_SECTOR_LAST))
   {
      if (!volumeEEPROM->get_sector_next(&next, fPtr)) return size;
      if ((next & 0xff00) == 0xff00) return size + LLFS_SECTOR_DATA_SIZE; // last full sector
      if ((next >> 8) == fPtr) return size + (next & 0x00ff);             // partially filled: COUNT byte
      size += LLFS_SECTOR_DATA_SIZE;
      fPtr = next >> 8;
   }
   return size;
}
/**
 * @brief Deletes file regardless of the type.
 * @mode name
 * @return
 */
void lf_delete(char *name)
{
   uint16_t pos;
   uint8_t next = false;
   lf_record_t record;
   while ((pos = lf_find_record(name, &record, next)))
   {
      next = true;
      if (!(record.attr & (LLFS_ATTR_DIR | LLFS_ATTR_LINK | LLFS_ATTR_INDEX))) // regular file, has data
         lf_clean_data(record.fptr);
      lf_del_record(pos);
   }
   return;
}

/**
 * @brief Write the cached sector back to the media if it was modified.
 * @return true on success
 */
static bool lf_flush(lfile_t *file)
{
   if (!file->changed) return true;
   if (!volumeEEPROM->write_sector(file->sData, file->pos >> 8)) return false;
   file->changed = 0;
   return true;
}

/**
 * @brief Flush and release an open file.
 */
void lf_close(lfile_t *file)
{
   if (file == NULL) return;
   if (file->mode & MODE_WRITE) lf_flush(file);
   vPortFree(file->sData);
   vPortFree(file);
}

/**
 * @brief Open a file, optionally creating it.
 * @param name file name, truncated to LLFS_FILENAME_LEN characters
 * @param mode MODE_READ, MODE_WRITE, MODE_CREATE, MODE_APPEND combination
 * @return open file or NULL, lf_error holds the reason
 */
lfile_t *lf_open(char *name, uint8_t mode)
{
   lfile_t *tmpFile;
   lf_record_t record;
   uint16_t next; // old chain link, MODE_WRITE only
   if (!name || !*name)
   {
      lf_error = LF_ERR_FNAME;
      return NULL;
   }
   lf_error = LF_ERR_MEM;
   if ((tmpFile = pvPortMalloc(sizeof(lfile_t))) == NULL) return NULL;
   if ((tmpFile->sData = pvPortMalloc(LLFS_SECTOR_SIZE)) == NULL)
   {
      vPortFree(tmpFile);
      return NULL;
   }
   tmpFile->mode = tmpFile->changed = 0; // nothing to flush if the open fails
   if (!(tmpFile->index = lf_find_record(name, &record, 0)))
   {
      if (!(mode & MODE_CREATE) || !(tmpFile->index = lf_add_record(&record, name, LLFS_ATTR_FROM_MODE(mode))))
      {
         lf_close(tmpFile);
         return NULL;
      }
   }
   if (!record.fptr || (record.fptr == LLFS_SECTOR_LAST) || (record.attr & (LLFS_ATTR_DIR | LLFS_ATTR_LINK | LLFS_ATTR_INDEX)))
   {
      lf_error = LF_ERR_FNAME;
      lf_close(tmpFile);
      return NULL;
   }
   tmpFile->volume = 0; // the EEPROM is the only volume so far
   tmpFile->mode = mode;
   tmpFile->upIndex = record.rptr; // reference to the parent directory
   lf_rname_tostr(tmpFile->name, name);
   tmpFile->pos = ((uint16_t)record.fptr) << 8;
   if (mode & MODE_WRITE)
   {
      if (mode & MODE_APPEND)
      {
         if (!(tmpFile->pos = lf_find_eof(record.fptr)))
         {
            lf_close(tmpFile);
            return NULL;
         }
      }
      else
      {
         if (!volumeEEPROM->get_sector_next(&next, record.fptr) ||       // where the old chain continues
             !volumeEEPROM->set_sector_next(&tmpFile->pos, record.fptr)) // COUNT 0, LINK self: keep the first sector
         {
            lf_close(tmpFile);
            return NULL;
         }
         if (((next & 0xff00) != 0xff00) && ((next >> 8) != record.fptr))
            lf_clean_data(next >> 8); // release the tail of the old content
      }
   }
   if (!volumeEEPROM->read_sector(tmpFile->sData, tmpFile->pos >> 8))
   {
      lf_close(tmpFile);
      return NULL;
   }
   tmpFile->dataSect = tmpFile->pos >> 8;
   return tmpFile;
}

/**
 * @brief Append data to a file opened for writing.
 * @return number of bytes written, less than size if the volume is full
 */
uint16_t lf_write(lfile_t *file, void *data, uint16_t size)
{
   uint16_t bCount = 0; // bytes written
   uint16_t dPtr;       // offset inside the cached sector
   uint16_t chunk;
   uint16_t sect;
   if (!file || !(file->mode & MODE_WRITE))
   {
      lf_error = LF_ERR_NOTOPEN;
      return 0;
   }
   dPtr = (uint8_t)file->pos;
   while (size)
   {
      if (file->sData[LLFS_SECTOR_LINK_ADDR] == LLFS_SECTOR_LAST) // cached sector is full: chain a new one
      {
         if (!(sect = lf_get_free_sector(false))) return bCount; // disk full
         chunk = sect << 8;
         /// claim the new sector first, a link into a free sector could be handed out twice
         if (!volumeEEPROM->set_sector_next(&chunk, sect)) return bCount;           // COUNT 0, LINK self: claimed and empty
         if (!volumeEEPROM->get_sector_next(&chunk, file->pos >> 8)) return bCount; // COUNT holds data, keep it
         chunk = (chunk & 0x00ff) | (sect << 8);
         if (!volumeEEPROM->set_sector_next(&chunk, file->pos >> 8)) return bCount; // link the full sector to the new one
         memset(file->sData, LLFS_EMPTY_BYTE, LLFS_SECTOR_SIZE);                    // the cache still holds the previous sector
         file->sData[LLFS_SECTOR_COUNT_ADDR] = 0;
         file->sData[LLFS_SECTOR_LINK_ADDR] = (uint8_t)sect;
         file->pos = sect << 8;
         file->dataSect = sect;
         dPtr = 0;
      }
      chunk = LLFS_SECTOR_DATA_SIZE - dPtr;
      if (chunk > size) chunk = size;
      memcpy(file->sData + dPtr, data, chunk);
      data = (uint8_t *)data + chunk;
      dPtr += chunk;
      size -= chunk;
      bCount += chunk;
      file->changed = 1;
      if (dPtr < LLFS_SECTOR_DATA_SIZE) // partially filled: COUNT keeps the length, lf_close() flushes it
      {
         file->sData[LLFS_SECTOR_COUNT_ADDR] = (uint8_t)dPtr;
         file->pos = (file->pos & 0xff00) + dPtr;
         break;
      }
      /// sector full: COUNT became data, LINK closes the file until the next sector is chained
      file->sData[LLFS_SECTOR_LINK_ADDR] = LLFS_SECTOR_LAST;
      if (!volumeEEPROM->write_sector(file->sData, file->pos >> 8)) return bCount;
      file->changed = 0;
      file->pos |= LLFS_SECTOR_DATA_SIZE; // full sector position, the next write chains a new one
   }
   return bCount;
}

/**
 * @brief Read data from the current position.
 * @return number of bytes read, less than size at the end of the file
 */
uint16_t lf_read(lfile_t *file, void *data, uint16_t size)
{
   uint16_t bCount = 0; // bytes read
   uint16_t chunk;
   uint16_t sect;

   if (!file || !(file->mode & (MODE_READ | MODE_WRITE)))
   {
      lf_error = LF_ERR_NOTOPEN;
      return 0;
   }
   sect = file->pos >> 8;
   while (size)
   {
      if (file->dataSect != sect) // the cache holds another sector
      {
         if (!(volumeEEPROM->read_sector(file->sData, sect))) return bCount;
         file->dataSect = sect;
      }
      /// LINK self: partially filled sector, COUNT is the length, otherwise the sector is full
      chunk = (file->sData[LLFS_SECTOR_LINK_ADDR] == (uint8_t)sect) ? file->sData[LLFS_SECTOR_COUNT_ADDR] : LLFS_SECTOR_DATA_SIZE;
      chunk = (chunk > (uint8_t)file->pos) ? chunk - (uint8_t)file->pos : 0; // bytes left in this sector
      if (chunk > size) chunk = size;
      if (chunk)
      {
         memcpy(data, file->sData + (file->pos & 0x00ff), chunk);
         data = (uint8_t *)data + chunk;
         size -= chunk;
         bCount += chunk;
         file->pos += chunk;
      }
      else
      {
         if ((file->sData[LLFS_SECTOR_LINK_ADDR] == LLFS_SECTOR_LAST) || (file->sData[LLFS_SECTOR_LINK_ADDR] == (uint8_t)sect))
            return bCount;                          // end of file
         sect = file->sData[LLFS_SECTOR_LINK_ADDR]; // jump to next sector
         file->pos = sect << 8;
      }
   }
   return bCount;
}

/**
 * @brief Read a line, the '\n' included, at most size-1 characters.
 * @return str, always zero terminated, NULL at the end of the file
 */
char *lf_gets(char *str, uint16_t size, lfile_t *file)
{
   uint16_t strPtr = 0;
   if (!size--) return NULL; // keep room for the terminator
   while (strPtr < size)
   {
      if (!lf_read(file, &str[strPtr], 1)) break; // end of file
      if (str[strPtr++] == '\n') break;
   }
   str[strPtr] = '\0';
   return strPtr ? str : NULL;
}
