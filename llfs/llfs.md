## *Linked List File System*
Written by Sergey Sanders
 
LLFS is a file system designed for low-volume memory devices up to 64 KB. It uses a linked list data structure with 256-byte sectors. 
The media is partitioned as follows:

| Sector | Type |
|---|---|
|0x00|Index[0]|
|0x01-0xFE|Data (Index[n])|
|0xFF|Index[1]|

First index sector includes physical media and the file table:

|Offset|Struct|
|---|---|
|0x00|*lf_phy_t*|
|0x10-0xE0|*lf_record_t*|
|0xF0|INDEX *lf_record_t*|

Data sectors is organized as follows

| Data Sector structure[256]|
|---|
|Byte 0|
|Byte 1|
| ... |
|Byte 253|
|*COUNT* [Byte 254]|
|*NEXT*|

- *COUNT*: number of valid bytes or last byte if *NEXT* is 0xFF
- *NEXT*: 
- - If count less than 254, then points to the current sector,
- - if sector is full, then points to the next sector,
- - if sector is full,and no data left, then set to 0xFF.

