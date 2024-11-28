//--------------------------------------------------
// Перенаправление вывода printf в usb-cdc
//--------------------------------------------------
#include <stdio.h>

#include "app_export.h"
#include "gpio_if.h"
#include "retarget.h"
#include "usbd_cdc_if.h"

//--------------------------------------------------
extern USBD_HandleTypeDef hUsbDeviceFS;

//--------------------------------------------------
static const uint32_t fputc_timeout = 2; // ms
//--------------------------------------------------
struct __FILE
{ int handle; };

FILE __stdin;
FILE __stdout;
//--------------------------------------------------
/* ring buffer for output. head == tail means empty buffer */
static uint8_t fputc_buff[FPUTC_BUFF_SIZE];
static uint16_t fputc_head = 0;
static uint16_t fputc_tail = 0;
static uint32_t fputc_clock = 0;

/* ring buffer for input. head == tail means empty buffer */
char fgetc_buff[FGETC_BUFF_SIZE];
uint16_t fgetc_head = 0;
uint16_t fgetc_tail = 0;
//--------------------------------------------------
static int fputc_tx(void);
//--------------------------------------------------
int fputc(int ch, FILE *f)/*{{{*/
{
/* if full buffer, try to send some data */
if(fputc_head == FPUTC_BUFF_SIZE - 1 && fputc_tail == 0)
    {
    if(fputc_tx() != 0)
	{ return EOF; }
    }
else if(fputc_head == fputc_tail - 1)
    {
    if(fputc_tx() != 0)
	{ return EOF; }
    }

fputc_buff[fputc_head] = ch;
fputc_head++;

if(fputc_head == FPUTC_BUFF_SIZE)
    { fputc_head = 0; }

fputc_clock = HAL_GetTick();
return ch;
}/*}}}*/
//--------------------------------------------------
int fgetc(FILE *f)/*{{{*/
{
int retval;

/* check for empty buffer */
retval = EOF;
if(fgetc_tail == fgetc_head)
    { return retval; }

retval = fgetc_buff[ fgetc_tail ];

fgetc_tail++;
if(fgetc_tail == FGETC_BUFF_SIZE)
    { fgetc_tail = 0; }

return retval;
}/*}}}*/
//--------------------------------------------------
void retarget_task(void)/*{{{*/
{
/* ready to send */
if(HAL_GetTick() - fputc_clock >= fputc_timeout &&
   fputc_head != fputc_tail)
    { fputc_tx(); }
}/*}}}*/
//--------------------------------------------------
static int fputc_tx(void)/*{{{*/
{
if(fputc_head == fputc_tail)
    { return 0; }

for(size_t i=0; fputc_tail != fputc_head; fputc_tail++, i++)
    {
    if(fputc_tail == FPUTC_BUFF_SIZE)
	{ fputc_tail = 0; }

    UserTxBufferFS[i] = fputc_buff[fputc_tail];
    }

USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, FPUTC_BUFF_SIZE);
USBD_CDC_TransmitPacket(&hUsbDeviceFS);

fputc_clock = HAL_GetTick();
return 0;
}/*}}}*/
//--------------------------------------------------
