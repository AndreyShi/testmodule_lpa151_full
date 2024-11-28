//--------------------------------------------------
// Low-level initialization of hid class for LPA
// device
//--------------------------------------------------
#ifndef HID_LPA_H
#define HID_LPA_H

//--------------------------------------------------
#include "usbh_core.h"

//--------------------------------------------------
#define HID_LPA_EP_SIZE 64
#define HID_LPA_REPORT_SIZE 255

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern USBH_StatusTypeDef USBH_HID_LPAInit(USBH_HandleTypeDef *phost);

extern void USBH_HID_LPASendData(const uint8_t *data, uint8_t size);
extern uint8_t  USBH_HID_LPAGetFlag(void);
extern uint8_t *USBH_HID_LPAGetData(void);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
