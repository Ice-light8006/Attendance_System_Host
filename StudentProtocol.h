#ifndef STUDENT_PROTOCOL_H
#define STUDENT_PROTOCOL_H

/* Shared wire format; keep host and MCU copies identical.
 * Request: finger_id BE16. Reply: version, status, finger_id BE16.
 * OK reply continues with student_id BE32, age U8, then four
 * [byte length U8, UTF-8 bytes] fields: name, sex, class, email.
 * Error replies contain only the four-byte prefix. Max payload: 255.
 */
#define STUDENT_INFO_REQUEST 0xACU
#define STUDENT_INFO_RESPONSE 0xB9U
#define STUDENT_INFO_VERSION 1U
/* Age 0..254; 255 means not filled. Zero-length text means not filled. */
#define STUDENT_INFO_AGE_UNKNOWN 255U
#define STUDENT_INFO_OK 0U
#define STUDENT_INFO_NOT_FOUND 1U
#define STUDENT_INFO_DB_ERROR 2U
#define STUDENT_INFO_TOO_LONG 3U
#define STUDENT_INFO_INVALID 4U
#define STUDENT_INFO_MAX_PAYLOAD 255U
#define STUDENT_INFO_MAX_FINGER 1024U
#endif
