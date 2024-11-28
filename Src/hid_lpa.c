//--------------------------------------------------
#include <string.h>

#include "app_export.h"
#include "gpio_if.h"
#include "hid_lpa.h"
#include "usbh_hid_lpa.h"

//--------------------------------------------------
static uint8_t hid_flag;
static uint8_t hid_ep[HID_LPA_EP_SIZE];
static uint8_t hid_recv_count;
static uint8_t hid_report[HID_LPA_REPORT_SIZE];
static uint8_t hid_request[2*HID_LPA_REPORT_SIZE];

static const uint8_t hid_zeros[2*HID_LPA_REPORT_SIZE] = { 0 };

//--------------------------------------------------
extern USBH_HandleTypeDef hUsbHostHS;

//--------------------------------------------------
USBH_StatusTypeDef USBH_HID_LPAInit(USBH_HandleTypeDef *phost)/*{{{*/
{
HID_HandleTypeDef *HID_Handle;

hid_flag = 0;
hid_recv_count = 0;
HID_Handle = (HID_HandleTypeDef *)phost->pActiveClass->pData;  
HID_Handle->pData = hid_ep;
fifo_init(&HID_Handle->fifo, hid_request, 2*HID_LPA_REPORT_SIZE);
return USBH_OK;    
}/*}}}*/
//--------------------------------------------------
void USBH_HID_LPASendData(const uint8_t *data, uint8_t size)/*{{{*/
{
USBH_HID_SendData(&hUsbHostHS, data, size);
USBH_HID_SendData(&hUsbHostHS, hid_zeros, HID_LPA_REPORT_SIZE - size);

hid_recv_count = 0;
hid_flag = 0;
}/*}}}*/
//--------------------------------------------------
uint8_t USBH_HID_LPAGetFlag(void)
{ return hid_flag; }
//--------------------------------------------------
uint8_t *USBH_HID_LPAGetData(void)
{ return hid_report; }
//--------------------------------------------------
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)/*{{{*/
{
memcpy(hid_report + HID_LPA_EP_SIZE * hid_recv_count, hid_ep, HID_LPA_EP_SIZE);

hid_recv_count++;
if(hid_recv_count == (HID_LPA_REPORT_SIZE + HID_LPA_EP_SIZE - 1) / HID_LPA_EP_SIZE) // round upwards
    {
    hid_recv_count = 0;
    hid_flag = 1;
    }
}/*}}}*/
//--------------------------------------------------
