//--------------------------------------------------
#ifndef RETARGET_H
#define RETARGET_H

//--------------------------------------------------
#include <stdint.h>

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
#define FPUTC_BUFF_SIZE 2048
#define FGETC_BUFF_SIZE 2048

//--------------------------------------------------
extern char fgetc_buff[FGETC_BUFF_SIZE];
extern uint16_t fgetc_head;
extern uint16_t fgetc_tail;

//--------------------------------------------------
extern void retarget_task(void);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
