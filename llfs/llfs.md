## *Linked List File System*
Written by Sergey Sanders
 
LLFS is a file system designed for low-volume memory devices up to 64 KB. It uses a linked list data structure with 256-byte sectors. 
The media is partitioned as follows:

| Sector | Type |
|---|---|
|0x00|Index[0]|
|0x01-0xFE|Data (Index[n])|
|0xFF|Index[1]|

Sector 0xFF never holds data: *LINK* 0xFF marks the end of a chain, so the value is not
available as a sector reference. It is claimed by the second index sector instead.

First index sector includes physical media and the file table:

|Offset|Struct|
|---|---|
|0x00|*lf_phy_t*|
|0x10-0xE0|*lf_record_t*|
|0xF0|INDEX *lf_record_t*|

The INDEX record is the last one of an index sector and overlaps the sector tail:
`fptr` (0xF0) points to the next index sector, `attr` (0xF1) carries LLFS_ATTR_INDEX,
`name[10]` is byte 0xFE and `name[11]` is byte 0xFF. Byte 0xFF must stay 0x00, it is the
index sector mark, and byte 0xFE must stay non-zero, otherwise the sector is seen as free.
The `name` field of the INDEX record is therefore not usable.

A file name occupies up to 12 characters and is zero terminated only when it is shorter,
use *lf_rname_tostr()* to read it as a string.

Data sectors is organized as follows

| Data Sector structure[256]|
|---|
|Byte 0|
|Byte 1|
| ... |
|Byte 253|
|*COUNT* [Byte 254]|
|*NEXT* [Byte 255]|

- *COUNT*: number of valid bytes, or the 255th data byte if the sector is full
- *NEXT*: 
- - If the sector is not full, then points to the current sector, *COUNT* holds the length,
- - if sector is full, then points to the next sector,
- - if sector is full,and no data left, then set to 0xFF.
- *COUNT* and *NEXT* both zero mark a free sector, a full sector therefore carries
  255 data bytes and a partially filled one at most 254.
