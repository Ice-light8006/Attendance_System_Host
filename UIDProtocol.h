#ifndef UIDPROTOCOL_H
#define UIDPROTOCOL_H

//与下位机 UID_Host.h 中的指令保持一致
#define UID_LIST          0xA9
#define UID_RESULT        0xAA

#define UID_QUERY         0xB3
#define UID_INSERT        0xB4
#define UID_UPDATE        0xB5
#define UID_DELETE        0xB6
#define UID_REQUEST_LIST  0xB7

#define UID_SUCCESS       0xFF
#define UID_INDEX_ERROR   0x00
#define UID_EMPTY         0x01
#define UID_DUPLICATE     0x02
#define UID_FULL          0x03
#define UID_NOT_INIT      0x04
#define UID_FLASH_ERROR   0x05
#define UID_STATE_ERROR   0x06
#define UID_ARG_ERROR     0x07
#define UID_OCCUPIED      0x08

#endif // UIDPROTOCOL_H
