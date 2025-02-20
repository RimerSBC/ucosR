/**-----------------------------------------------------------------------------
 * Copyright (c) 2025 Sergey Sanders
 * sergey@sesadesign.com
 * -----------------------------------------------------------------------------
 * Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0
 * International (CC BY-NC-SA 4.0). 
 * 
 * You are free to:
 *  - Share: Copy and redistribute the material.
 *  - Adapt: Remix, transform, and build upon the material.
 * 
 * Under the following terms:
 *  - Attribution: Give appropriate credit and indicate changes.
 *  - NonCommercial: Do not use for commercial purposes.
 *  - ShareAlike: Distribute under the same license.
 * 
 * DISCLAIMER: This work is provided "as is" without any guarantees. The authors
 * aren’t responsible for any issues, damages, or claims that come up from using
 * it. Use at your own risk!
 * 
 * Full license: http://creativecommons.org/licenses/by-nc-sa/4.0/
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

uint16_t emptyNext = 0; // LSB-count or last data if MSB is 0xFF, MSB-next valid sector, self if partially full and 0xff
                        // if it's last full sector

lf_err_t lf_error;

void __attribute__((weak)) lf_init(void) {}

lf_err_t lf_format(uint32_t size, uint16_t devID, char* name)
{
   uint16_t sPtr;
   lf_phy_t tmpPhy = {
      .fsType = LLFS_VOLUME_VALID,
      .fsTypeN = (uint8_t)~LLFS_VOLUME_VALID,
      .devID = devID,
      .secCount = size / LLFS_SECTOR_SIZE,
      .compress = LLFS_COMPRESS_NONE,
      .ecc = LLFS_ECC_NONE,
   };

   memcpy(tmpPhy.name, name, LLFS_VOLNAME_LEN);
   memset(volumeEEPROM->sData, LLFS_EMPTY_BYTE, LLFS_SECTOR_SIZE);
   memcpy(volumeEEPROM->sData, &tmpPhy, sizeof(lf_phy_t));
   volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] = LLFS_ATTR_INDEX; /// mark sector as an index.
   volumeEEPROM->sData[LLFS_SECTOR_NEXT_ADDR] = 0x00; // first sector is the last one
   volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] = LLFS_MARK_INDEX_BYTE;
   if(volumeEEPROM->write_sector(volumeEEPROM->sData, 0))
      for(sPtr = 1; sPtr < volumeEEPROM->phy->secCount - (volumeEEPROM->phy->secCount == 0xff) ? 1 : 0; sPtr++)
      {
         if(!(volumeEEPROM->set_sector_next(&emptyNext, sPtr))) break;
      }
   return lf_error = LF_ERR_NONE;
}

#define LLFS_COUNT_UNUSED_AS_FREE 0

uint32_t lf_get_free(uint8_t volIndex)
{
   uint32_t size = 0;
   uint16_t sPtr;
   uint16_t next;

   for(sPtr = 1; sPtr < volumeEEPROM->phy->secCount; sPtr++)
   {
      if(!volumeEEPROM->get_sector_next(&next, sPtr)) return 0;
      if(!next) size += LLFS_SECTOR_SIZE - 1;
#if LLFS_COUNT_UNUSED_AS_FREE
      else if(next >> 8 == sPtr)
         size += LLFS_SECTOR_SIZE - (next & 0xff) - 1;
#endif
   }
   return size;
}

/// get next free available
/// if looking for index sector, the 0xff sector can be used
uint16_t lf_get_free_sector()
{
   uint16_t sPtr;
   uint16_t next;

   for(sPtr = 1; sPtr < volumeEEPROM->phy->secCount; sPtr++)
   {
      if(!volumeEEPROM->get_sector_next(&next, sPtr)) return 0;
      if(!next) return sPtr;
   }
   lf_error = LF_ERR_FULL;
   return 0;
}

uint16_t lf_add_record(lf_record_t* newRecord, char* name, uint8_t attr)
{
   uint16_t indexBlock = 0;
   uint8_t index;
   uint8_t rPtr;
   uint16_t next;
   uint16_t sect;
   uint16_t pos = 0;
   lf_record_t* iRecord;

   while(!pos)
   {
      if(!volumeEEPROM->read_sector(volumeEEPROM->sData, indexBlock)) return 0;
      if(!((volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] == LLFS_ATTR_INDEX) && (volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] == LLFS_MARK_INDEX_BYTE)))
      {
         lf_error = LF_ERR_VOLUME;
         return 0;
      }
      rPtr = indexBlock ? 0 : sizeof(lf_record_t);
      for(index = indexBlock ? 0 : 1; index < LLFS_SECTOR_SIZE / sizeof(lf_record_t) - 1; index++)
      {
         if(volumeEEPROM->sData[rPtr] == LLFS_EMPTY_BYTE)
         {
            uint8_t i;
            iRecord = (lf_record_t*)&volumeEEPROM->sData[rPtr];
            if(!(attr & 0x70))
            {
               if(!(sect = lf_get_free_sector())) return 0;
               /// sect points to the available sector
               uint16_t next = sect << 8;
               if(!(volumeEEPROM->set_sector_next(&next, sect))) return 0;
               iRecord->fptr = sect;
            }
            else
               iRecord->fptr = 0xff; // the iRecord is either directory,link or index
            iRecord->attr = attr;
            for(i = 0; i < LLFS_FILENAME_LEN && name[i]; i++) iRecord->name[i] = name[i];
            if(i < LLFS_FILENAME_LEN) iRecord->name[i] = '\0';
            pos = (indexBlock << 8) + index;
            if(!volumeEEPROM->write_sector(volumeEEPROM->sData, indexBlock)) return 0;
            memcpy(newRecord, iRecord, sizeof(lf_record_t));
            return pos;
         }
         rPtr += sizeof(lf_record_t);
         iRecord = (lf_record_t*)&volumeEEPROM->sData[rPtr];
      }
      if((iRecord->attr & LLFS_ATTR_INDEX) && iRecord->fptr) { indexBlock = iRecord->fptr; }
      else // create new index sector
      {
         // if the volume has the maximum sectors (64KB) then check last sector for availability
         if(volumeEEPROM->get_sector_next(&next, LLFS_MAX_SECTOR_COUNT - 1)) return 0;
         if((volumeEEPROM->phy->secCount == LLFS_MAX_SECTOR_COUNT) && (!next))
         {
            if(lf_error) return 0;
            sect = LLFS_MAX_SECTOR_COUNT - 1;
         }
         else if(!(sect = lf_get_free_sector()))
            return 0; // find next available sector
         iRecord->fptr = sect;
         iRecord->attr = LLFS_ATTR_INDEX;
         iRecord->name[11] = 0xff;                                                  // mark sector as used
         if(!volumeEEPROM->write_sector(volumeEEPROM->sData, indexBlock)) return 0; // update current sector
         indexBlock = sect;
         memset(volumeEEPROM->sData, 0x00, LLFS_SECTOR_SIZE);
         volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] = LLFS_ATTR_INDEX; /// mark sector as an index.
         volumeEEPROM->sData[LLFS_SECTOR_NEXT_ADDR] = 0x00; /// mark as last one
         volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] = LLFS_MARK_INDEX_BYTE;
         if(!volumeEEPROM->write_sector(volumeEEPROM->sData, indexBlock)) return 0; // update new sector
      }
   }
   lf_error = LF_ERR_FULL;
   return 0;
}

void lf_del_record(uint16_t pos)
{
   uint8_t indexBlock = pos >> 8;
   uint8_t index = pos;

   if(!volumeEEPROM->read_sector(volumeEEPROM->sData, indexBlock)) return;
   if((volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] == LLFS_ATTR_INDEX) && (volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] == LLFS_MARK_INDEX_BYTE))
   {
      memset((uint8_t*)(volumeEEPROM->sData + index * sizeof(lf_record_t)), LLFS_EMPTY_BYTE, sizeof(lf_record_t));
      if(!volumeEEPROM->write_sector(volumeEEPROM->sData, indexBlock)) return; // update index sector
   }
   else
      lf_error = LF_ERR_MEM;
   return;
}

bool lf_match(char* fname, char* exp)
{
   uint8_t i, ep;

   if((fname[0] < '!') || (fname[0] > '~')) return false; // wrong file name
   for(i = 0, ep = 0; i < LLFS_FILENAME_LEN && fname[i]; i++)
   {
      if(exp[ep] == '*')
      {
         if(!exp[++ep])
            return true;
         else if(!fname[++i] || i == LLFS_FILENAME_LEN)
            return false;           // nothing to check, the file
         while(fname[i] != exp[ep]) // there is a character to match after asterisks.
         {
            if(!fname[++i] || i == LLFS_FILENAME_LEN) return false;
         }
      }
      if((fname[i] != exp[ep]) || (!exp[ep])) return false;
      ep++;
   }
   return (exp[ep]) ? false : true; // fail if there are unchecked haracter in expression
}

uint16_t lf_find_record(char* name, lf_record_t* record, uint8_t next)
{
   uint8_t indexBlock;
   uint8_t index;
   uint16_t pos = 0;
   static uint16_t lastPos = 0;

   if(!next) lastPos = 1;
   indexBlock = lastPos >> 8;
   index = lastPos;
   lf_error = LF_ERR_NOTFOUND;
   do {
      if(volumeEEPROM->read_sector(volumeEEPROM->sData, indexBlock) != LLFS_SECTOR_SIZE) return 0;
      if(!((volumeEEPROM->sData[LLFS_SECTOR_TYPE_ADDR] == LLFS_ATTR_INDEX) && (volumeEEPROM->sData[LLFS_MARK_INDEX_ADDR] == LLFS_MARK_INDEX_BYTE)))
         return 0; // check that the sector is an index sector
      for(; index < LLFS_SECTOR_SIZE / sizeof(lf_record_t) - 1; index++)
      {
         if(lf_match(((lf_record_t*)&volumeEEPROM->sData[index * sizeof(lf_record_t)])->name, name))
         {
            pos = indexBlock;
            pos = (pos << 8) + index;
            lastPos = pos + 1;
            memcpy(record, &volumeEEPROM->sData[index * sizeof(lf_record_t)], sizeof(lf_record_t));
            return pos;
         }
      }
      lastPos = 0;
      index = 0;
      indexBlock = volumeEEPROM->sData[LLFS_SECTOR_NEXT_ADDR];
   } while(indexBlock); // there is another index sector to check
   lastPos = 0; // no matches found
   return 0;
}

char* lf_rname_tostr(char* destStr, char* name)
{
   uint8_t i;

   for(i = 0; i < LLFS_FILENAME_LEN; i++)
   {
      destStr[i] = name[i];
      if(!destStr[i]) break;
   }
   destStr[i] = 0; // set string termination at the end.
   return destStr;
}
/**
 * @brief clear data on the volume begining from the pointer
 * @mode fPtr
 */

void lf_clean_data(uint8_t fPtr)
{
   uint16_t next;

   if(!volumeEEPROM->get_sector_next(&next, fPtr)) return;
   volumeEEPROM->set_sector_next(&emptyNext, fPtr); // mark the sector as empty
   while(((next & 0xff00) != 0xff00) && ((next >> 8) != fPtr))
   {
      fPtr = next >> 8;
      if(!volumeEEPROM->get_sector_next(&next, fPtr)) return;
      volumeEEPROM->set_sector_next(&emptyNext, fPtr);
      if(lf_error) return;
   }
}
uint16_t lf_find_eof(uint16_t fPtr)
{
   uint16_t next;

   if(!volumeEEPROM->get_sector_next(&next, fPtr)) return 0;
   while(((next & 0xff00) != 0xff00) && ((next & 0xff00) != fPtr << 8))
   {
      fPtr = next >> 8;
      if(!volumeEEPROM->get_sector_next(&next, fPtr)) return 0;
   }
   return (fPtr << 8) + (((next & 0xff00) == 0xff00) ? 0 : (next & 0x00ff));
}
// uint32_t lf_get_fsize(uint16_t fPtr)
/**
* Get file size, identified by name or if empty, by fPtr (retrived by lf_find_record)
*/
uint32_t lf_get_fsize(char* name, uint16_t fPtr)
{
   uint16_t next;
   uint32_t size = 0;
   lf_record_t record;

   if(*name)
      if(!(fPtr = lf_find_record(name, &record, false))) return 0;

   if(!volumeEEPROM->get_sector_next(&next, fPtr)) return 0;
   while(((next & 0xff00) != 0xff00) && ((next & 0xff00) != fPtr << 8))
   {
      size += LLFS_SECTOR_SIZE - 1;
      fPtr = next >> 8;
      if(!volumeEEPROM->get_sector_next(&next, fPtr)) return size;
   }

   return size + ((uint8_t)next);
}
/**
 * @brief Deletes file regardless of the type.
 * @mode name
 * @return
 */
void lf_delete(char* name)
{
   uint16_t pos = 0;
   uint8_t next = false;
   lf_record_t record;

   while((pos = lf_find_record(name, &record, next)))
   {
      next = true;
      if(lf_error) return;
      if(!(record.attr & (LLFS_ATTR_DIR | LLFS_ATTR_LINK | LLFS_ATTR_INDEX))) // regular file, has data
      {
         lf_clean_data(record.fptr);
      }
      lf_del_record(pos);
   }
   return;
}

void lf_flush(lfile_t* file) { volumeEEPROM->write_sector(file->sData, file->pos >> 8); }

void lf_close(lfile_t* file)
{
   if(file == NULL) return;
   if(file->mode & MODE_WRITE) lf_flush(file);
   vPortFree(file->sData);
   vPortFree(file);
}

lfile_t* lf_open(char* name, uint8_t mode)
{
   lfile_t* tmpFile;
   uint8_t tmpVol = 0;
   lf_record_t record;

   lf_error = LF_ERR_MEM;
   if(!*name || !name)
   {
      lf_error = LF_ERR_FNAME;
      return NULL;
   }
   if((tmpFile = pvPortMalloc(sizeof(lfile_t))) != NULL)
   {
      if((tmpFile->sData = pvPortMalloc(LLFS_SECTOR_SIZE)) == NULL)
      {
         vPortFree(tmpFile);
         return NULL;
      }
      if(!(tmpFile->index = lf_find_record(name, &record, 0)))
      {
         if(mode & MODE_CREATE) tmpFile->index = lf_add_record(&record, name, mode & 0x07);
      }
      if(!tmpFile->index)
      {
         lf_close(tmpFile);
         return NULL;
      }
      if(lf_error)
      {
         lf_close(tmpFile);
         return NULL;
      }
      tmpFile->volume = tmpVol;
      tmpFile->mode = mode;
      strcpy(tmpFile->name, name);
      tmpFile->changed = 0;
      tmpFile->dataSect = 0;
      tmpFile->pos = ((uint16_t)record.fptr) << 8;

      if(mode & MODE_WRITE)
      {
         if(mode & MODE_APPEND)
         {
            if(!(tmpFile->pos = lf_find_eof(record.fptr)))
            {
               lf_close(tmpFile);
               return NULL;
            }
         }
         else
         {
            lf_clean_data(tmpFile->pos >> 8);
            volumeEEPROM->set_sector_next(&tmpFile->pos, tmpFile->pos >> 8); // mark first sector as used empty
         }
      }
      if(!volumeEEPROM->read_sector(tmpFile->sData, tmpFile->pos >> 8))
      {
         lf_close(tmpFile);
         return NULL;
      }
      else
         tmpFile->dataSect = tmpFile->pos >> 8;
   }
   return tmpFile;
}

uint16_t lf_write(lfile_t* file, void* data, uint16_t size)
{
   uint16_t bCount = 0;
   uint16_t dPtr, chunk;

   if(!file)
   {
      lf_error = LF_ERR_NOTOPEN;
      return 0;
   }
   uint16_t sect = file->pos;
   dPtr = (uint8_t)file->pos;
   if(file->sData[LLFS_SECTOR_SIZE - 1] == 0xff) // allocate a new sector
   {
      if(!volumeEEPROM->get_sector_next(&chunk, file->pos >> 8)) return 0; // get last two bytes of the sector
      if(!(sect = lf_get_free_sector())) return 0;                         // disk full
      sect <<= 8;
      chunk = (chunk & 0x00ff) | sect; // update next sector
      volumeEEPROM->set_sector_next(&chunk, file->pos >> 8);
      volumeEEPROM->set_sector_next(&sect, sect >> 8);
      file->pos = sect;
   }
   while(size)
   {
      chunk = LLFS_SECTOR_SIZE - dPtr - 1;
      if(chunk > size) chunk = size;
      size -= chunk;
      memcpy((uint8_t*)(file->sData + dPtr), data, chunk);
      data += chunk;
      dPtr += chunk;
      bCount += chunk;
      if((dPtr) == (LLFS_SECTOR_SIZE - 1)) // sector full
      {
         if(size) // it isn't the last sector, so allocate another one
         {
            if(!(sect = lf_get_free_sector()))
            {
               file->sData[LLFS_SECTOR_SIZE - 1] = 0xff; // mark last sector full
               file->pos |= 0x00ff;
               volumeEEPROM->write_sector(file->sData, file->pos >> 8);
               break; // disk full, but some data has been written
            }
            file->sData[LLFS_SECTOR_SIZE - 1] = (uint8_t)sect;
            sect <<= 8;
            if(!volumeEEPROM->set_sector_next(&sect, sect >> 8)) return 0; // allocate sector
         }
         else
         {
            file->sData[LLFS_SECTOR_SIZE - 1] = 0xff; // mark last sector full
            volumeEEPROM->write_sector(file->sData, file->pos >> 8);
            sect = file->pos | 0x00ff; // sector full
         }
         if(!volumeEEPROM->write_sector(file->sData, file->pos >> 8)) return 0;
         file->pos = sect;
      }
      else
      {
         file->sData[LLFS_SECTOR_SIZE - 2] = (uint8_t)dPtr;
         file->pos = (file->pos & 0xff00) + (uint8_t)dPtr;
      }
      dPtr = 0;
   }
   return bCount;
}

uint16_t lf_read(lfile_t* file, void* data, uint16_t size)
{
   uint16_t bCount = 0, sectDSize;
   uint16_t chunk;
   uint16_t sect = file->pos >> 8;

   if(!file)
   {
      lf_error = LF_ERR_NOTOPEN;
      return 0;
   }
   while(size)
   {
      if(file->dataSect != sect) // update sector data
      {
         if(!(volumeEEPROM->read_sector(file->sData, sect)))
            return 0; // read first sector
         else
            file->dataSect = sect;
      }
      if(file->sData[LLFS_SECTOR_SIZE - 1] == (uint8_t)sect) // partially filled sector
         sectDSize = file->sData[LLFS_SECTOR_SIZE - 2];
      else
         sectDSize = LLFS_SECTOR_SIZE - 1;

      chunk = sectDSize - (uint8_t)file->pos;
      if(chunk > size) chunk = size;
      size -= chunk;

      if(!chunk && (chunk < size)) // no data
      {
         if((file->sData[LLFS_SECTOR_SIZE - 1] == 0xff) || (file->sData[LLFS_SECTOR_SIZE - 1] == sect)) // last sector
            return bCount;
         else // jump to next sector
         {
            sect = file->sData[LLFS_SECTOR_SIZE - 1];
            file->pos = sect << 8;
         }
      }
      else
      {
         memcpy(data, (uint8_t*)(file->sData + (file->pos & 0x00ff)), chunk);
         data += chunk;
         bCount += chunk;
         file->pos += chunk;
      }
   }
   return bCount;
}

char* lf_gets(char* str, uint16_t size, lfile_t* file)
{
   uint16_t sectDSize;
   uint16_t dPtr = 0;
   uint16_t strPtr = 0;

   if(!file)
   {
      lf_error = LF_ERR_NOTOPEN;
      return NULL;
   }
   uint16_t sect = file->pos >> 8;
   dPtr = (uint8_t)file->pos;
   while(strPtr < size)
   {
      if(file->dataSect != sect) // update sector data
      {
         if(!(volumeEEPROM->read_sector(file->sData, sect)))
            return NULL; // read first sector
         else
            file->dataSect = sect;
      }
      if(file->sData[LLFS_SECTOR_SIZE - 1] == (uint8_t)sect) // partially filled sector
         sectDSize = file->sData[LLFS_SECTOR_SIZE - 2];
      else
         sectDSize = LLFS_SECTOR_SIZE - 1;

      while(dPtr < sectDSize) // file->sData[dPtr] && file->sData[dPtr]!='\n' && strPtr<size)
      {
         if(strPtr >= size - 1) return NULL; // the line is too long
         str[strPtr] = file->sData[dPtr++];
         if(str[strPtr++] == '\n')
         {
            file->pos = (file->pos & 0xff00) + dPtr;
            if(strPtr >= size) return NULL; // the line is too long
            str[strPtr] = '\0';             // add end of line
            return str;
         }
      } // end of sector;
      dPtr = 0;
      if((file->sData[LLFS_SECTOR_SIZE - 1] == 0xff) || (file->sData[LLFS_SECTOR_SIZE - 1] == sect)) // last sector
         return NULL;
      else // jump to next sector
      {
         sect = file->sData[LLFS_SECTOR_SIZE - 1];
         file->pos = sect << 8;
      }
   }
   return NULL;
}
