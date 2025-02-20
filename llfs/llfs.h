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

#ifndef LLFS_H_INCLUDED
#define LLFS_H_INCLUDED
#include "stdint.h"
#include "stdbool.h"

#define LLFS_FILENAME_LEN	12
#define LLFS_VOLNAME_LEN	8
#define LLFS_SECTOR_SIZE	256
#define LLFS_MAX_SECTOR_COUNT	256
#define LLFS_MAX_VOLUME_COUNT	4

#define LLFS_VOLUME_VALID	0xdb
#define LLFS_COMPRESS_NONE	0x00
#define LLFS_ECC_NONE		0x00

#define LLFS_SECTOR_NEXT_ADDR   0xf0 // Pointer to next index sector, 0x00 for none
#define LLFS_SECTOR_TYPE_ADDR   0xf1 // Last lf_record_t with attribute set to LLFS_ATTR_INDEX
#define LLFS_MARK_INDEX_ADDR    0xff // 
#define LLFS_MARK_INDEX_BYTE    0x00 // Last byte in the index sector  

/// User accessable attributes
#define LLFS_ATTR_EXEC		0x04
#define LLFS_ATTR_WRITE		0x02
#define LLFS_ATTR_READ		0x01	
#define LLFS_ATTR_LOCK		0x08
/// System only accessable attributes
#define LLFS_ATTR_DIR		0x10	// directory
#define LLFS_ATTR_LINK		0x20	// link
#define LLFS_ATTR_INDEX		0x40	// the fptr points to next index sector
#define LLFS_ATTR_FVALID	0x80	// file descriptor valid status

#define MODE_EXEC		0x01
#define MODE_WRITE		0x02
#define MODE_READ		0x04
#define MODE_CREATE		0x08
#define MODE_APPEND		0x10

typedef enum
{
	LF_ERR_NONE = 0,
	LF_ERR_VOLUME,
	LF_ERR_READ,
	LF_ERR_WRITE,
	LF_ERR_FULL,
	LF_ERR_MEM,
	LF_ERR_NOTOPEN,
	LF_ERR_NOTFOUND,
	LF_ERR_FNAME,
} lf_err_t;

typedef struct __attribute__((packed,aligned(4)))
{
	uint8_t fsType;	// 0xdb 
	uint8_t fsTypeN; // ~fsType
	uint16_t devID;
	uint16_t secCount; // sector count
	uint8_t compress; // type of compression
	uint8_t ecc; // ecc base size
	uint8_t name[LLFS_VOLNAME_LEN]; // for further use 
}lf_phy_t;

typedef struct __attribute__((packed,aligned(4)))
{
	const lf_phy_t *phy;
	uint16_t (*write_sector)(uint8_t *data,uint16_t sector);
	uint16_t (*read_sector)(uint8_t *data,uint16_t sector);
	bool (*get_sector_next)(uint16_t *next,uint16_t sector);
	bool (*set_sector_next)(uint16_t *next,uint16_t sector);
	uint8_t  (*init)(void *volume);
	uint8_t sData[LLFS_SECTOR_SIZE]; // Sector temporary data
}volume_t;

typedef struct __attribute__((packed,aligned(4)))
{
	uint8_t fptr; // sector pointer/
	uint8_t attr; // file attribute
	uint16_t rptr; // reference pointer, 0 for root
	char name[LLFS_FILENAME_LEN];
} lf_record_t; // record - file descriptor for small volume with sector count up to 254, 0 and 255 are index sectors

typedef struct __attribute__((packed,aligned(4)))
{
	uint16_t index;	// points to the first sector 
	uint16_t pos;  // current position
	uint8_t *sData; // temporary sector data
	uint8_t dataSect; // temporary sector index
	uint16_t upIndex; // upper directory
	char name[LLFS_FILENAME_LEN+1];
	uint8_t mode;
	uint8_t changed;
	uint8_t volume;
} lfile_t;

extern volume_t *volumes[];
extern lf_err_t lf_error;
void lf_init(void);
lf_err_t lf_format(uint32_t size,uint16_t devID,char *name);
void lf_delete(char *name);
uint32_t lf_get_free(uint8_t volIndex);
uint32_t lf_get_fsize(char* name, uint16_t fPtr);
uint16_t lf_find_record(char *name,lf_record_t *record, uint8_t next);
char *lf_rname_tostr(char *destStr,char *name);
void lf_close(lfile_t *file);
lfile_t *lf_open(char *name, uint8_t mode);
uint16_t lf_write(lfile_t *file,void *data,uint16_t size);
uint16_t lf_read(lfile_t *file,void *data,uint16_t size);
char *lf_gets(char *str, uint16_t size, lfile_t *file);

#endif // LLFS_H_INCLUDED