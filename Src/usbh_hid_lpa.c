//--------------------------------------------------
// Custom HID class to handle LPA devices like LPA-151
//--------------------------------------------------
#include "usbh_hid_lpa.h"
#include "usbh_ioreq.h"

#include "app_export.h"
#include "gpio_if.h"
#include "hid_lpa.h"

//--------------------------------------------------
static USBH_StatusTypeDef USBH_HID_InterfaceInit  (USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HID_InterfaceDeInit(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_HID_ClassRequest(USBH_HandleTypeDef *phost);
static USBH_StatusTypeDef USBH_HID_Process(USBH_HandleTypeDef *phost);

static USBH_StatusTypeDef USBH_HID_SOFProcess(USBH_HandleTypeDef *phost);

static void USBH_HID_ParseHIDDesc(HID_DescTypeDef *desc, uint8_t *buf);

// The class driver itself
//--------------------------------------------------
USBH_ClassTypeDef HID_Class = { /*{{{*/
    "LPA HID",
    USB_HID_CLASS,
    USBH_HID_InterfaceInit,
    USBH_HID_InterfaceDeInit,
    USBH_HID_ClassRequest,
    USBH_HID_Process,
    USBH_HID_SOFProcess,
    NULL,
};/*}}}*/
//--------------------------------------------------
// Functions of the driver
//--------------------------------------------------
static USBH_StatusTypeDef USBH_HID_InterfaceInit(USBH_HandleTypeDef *phost)/*{{{*/
{	
uint8_t max_ep;
uint8_t num = 0;
uint8_t interface;

USBH_StatusTypeDef status = USBH_FAIL;
HID_HandleTypeDef *HID_Handle;

// search for standard class and custom subclass interface
interface = USBH_FindInterface(phost, phost->pActiveClass->ClassCode, 0x00, 0xFF);

if(interface == 0xFF) /* No Valid Interface */
    { USBH_DbgLog("Cannot Find the interface for %s class.", phost->pActiveClass->Name); }
else
    {
    USBH_SelectInterface(phost, interface);
    phost->pActiveClass->pData = (HID_HandleTypeDef *)USBH_malloc(sizeof(HID_HandleTypeDef));
    HID_Handle = (HID_HandleTypeDef *)phost->pActiveClass->pData;
    HID_Handle->state = HID_ERROR;

    /* Do not decode bootclass protocol, just assume it's Kupriyanov protocol */
    USBH_UsrLog("LPA device found!"); 
    HID_Handle->Init = USBH_HID_LPAInit;     

    HID_Handle->state     = HID_INIT;
    HID_Handle->ctl_state = HID_REQ_INIT;
    HID_Handle->ep_addr   = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bEndpointAddress;
    HID_Handle->length    = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].wMaxPacketSize;
    HID_Handle->poll      = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[0].bInterval;

    if(HID_Handle->poll < HID_MIN_POLL) 
	{ HID_Handle->poll = HID_MIN_POLL; }

    /* Check fo available number of endpoints */
    /* Find the number of EPs in the Interface Descriptor */      
    /* Choose the lower number in order not to overrun the buffer allocated */
    max_ep = ( (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bNumEndpoints <= USBH_MAX_NUM_ENDPOINTS) ? 
	      phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bNumEndpoints :
	      USBH_MAX_NUM_ENDPOINTS); 

    /* Decode endpoint IN and OUT address from interface descriptor */
    for( ;num < max_ep; num++)
	{
	if(phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress & 0x80)
	    {
	    HID_Handle->InEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress);
	    HID_Handle->InPipe = USBH_AllocPipe(phost, HID_Handle->InEp);

	    /* Open pipe for IN endpoint */
	    USBH_OpenPipe(phost,
			  HID_Handle->InPipe,
			  HID_Handle->InEp,
			  phost->device.address,
			  phost->device.speed,
			  USB_EP_TYPE_INTR,
			  HID_Handle->length); 

	    USBH_LL_SetToggle(phost, HID_Handle->InPipe, 0);
	    }
	else
	    {
	    HID_Handle->OutEp = (phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].Ep_Desc[num].bEndpointAddress);
	    HID_Handle->OutPipe = USBH_AllocPipe(phost, HID_Handle->OutEp);

	    /* Open pipe for OUT endpoint */
	    USBH_OpenPipe(phost,
			  HID_Handle->OutPipe,
			  HID_Handle->OutEp,                            
			  phost->device.address,
			  phost->device.speed,
			  USB_EP_TYPE_INTR,
			  HID_Handle->length); 

	    USBH_LL_SetToggle(phost, HID_Handle->OutPipe, 0);        
	    }
	}  

    status = USBH_OK;
    }

return status;
}/*}}}*/
//--------------------------------------------------
static USBH_StatusTypeDef USBH_HID_InterfaceDeInit(USBH_HandleTypeDef *phost)/*{{{*/
{
HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *)phost->pActiveClass->pData;

if(HID_Handle->InPipe != 0x00)
    {
    USBH_ClosePipe(phost, HID_Handle->InPipe);
    USBH_FreePipe (phost, HID_Handle->InPipe);
    HID_Handle->InPipe = 0;
    }

if(HID_Handle->OutPipe != 0x00)
    {
    USBH_ClosePipe(phost, HID_Handle->OutPipe);
    USBH_FreePipe (phost, HID_Handle->OutPipe);
    HID_Handle->OutPipe = 0;
    }

if(phost->pActiveClass->pData)
    { USBH_free(phost->pActiveClass->pData); }

return USBH_OK;
}/*}}}*/
//--------------------------------------------------
static USBH_StatusTypeDef USBH_HID_ClassRequest(USBH_HandleTypeDef *phost)/*{{{*/
{
USBH_StatusTypeDef status = USBH_BUSY;
HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

switch(HID_Handle->ctl_state)
    {
    case HID_REQ_INIT:
    case HID_REQ_GET_HID_DESC:
	if(USBH_HID_GetHIDDescriptor(phost, USB_HID_DESC_SIZE) == USBH_OK)
	    {
	    USBH_HID_ParseHIDDesc(&HID_Handle->HID_Desc, phost->device.Data);
	    HID_Handle->ctl_state = HID_REQ_GET_REPORT_DESC;
	    }
	break;

    case HID_REQ_GET_REPORT_DESC:
	if(USBH_HID_GetHIDReportDescriptor(phost, HID_Handle->HID_Desc.wItemLength) == USBH_OK)
	    {
	    HID_Handle->ctl_state = HID_REQ_IDLE;
	    phost->pUser(phost, HOST_USER_CLASS_ACTIVE); 
	    status = USBH_OK;
	    }
	break;

    case HID_REQ_IDLE:
    default:
	break;
    }

return status; 
}/*}}}*/
//--------------------------------------------------
static USBH_StatusTypeDef USBH_HID_Process(USBH_HandleTypeDef *phost)/*{{{*/
{
uint8_t tgl;

USBH_StatusTypeDef status = USBH_OK;
USBH_URBStateTypeDef urb_st;
HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *)phost->pActiveClass->pData;

switch(HID_Handle->state)
    {
    case HID_INIT:
	HID_Handle->Init(phost); 
	HID_Handle->state = HID_SYNC;
	break;

    /* Sync with start of even frame */
    case HID_SYNC:
	if(phost->Timer & 1)
	    { HID_Handle->state = HID_GET_DATA; }
#if (USBH_USE_OS == 1)
	osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
#endif   
	break;

    case HID_SEND_DATA:
	USBH_InterruptSendData(phost,
			       HID_Handle->pData,
			       HID_Handle->length,
			       HID_Handle->OutPipe);
	HID_Handle->state = HID_BUSY;
	break;

    case HID_BUSY:
	urb_st = USBH_LL_GetURBState(phost, HID_Handle->OutPipe);
	switch(urb_st)
	    {
	    case USBH_URB_NOTREADY:
		HID_Handle->state = HID_SEND_DATA;
		break;

	    default:
	    case USBH_URB_DONE:
		HID_Handle->state = HID_GET_DATA;
		tgl = USBH_LL_GetToggle(phost, HID_Handle->OutPipe);
		USBH_LL_SetToggle(phost, HID_Handle->OutPipe, 1 - tgl);
		break;
	    };
	break;

    case HID_GET_DATA:
	USBH_InterruptReceiveData(phost,
				  HID_Handle->pData,
				  HID_Handle->length,
				  HID_Handle->InPipe);

	HID_Handle->DataReady = 0;
	HID_Handle->state = HID_POLL;
	break;

    case HID_POLL:
	urb_st = USBH_LL_GetURBState(phost, HID_Handle->InPipe);
	switch(urb_st)
	    {
	    case USBH_URB_DONE:
		if(HID_Handle->DataReady == 0)
		    {
		    HID_Handle->DataReady = 1;
		    USBH_HID_EventCallback(phost);

#if (USBH_USE_OS == 1)
		    osMessagePut(phost->os_event, USBH_URB_EVENT, 0);
#endif          
		    }
		break;

	    case USBH_URB_STALL:
		if(USBH_ClrFeature(phost, HID_Handle->ep_addr) == USBH_OK)
		    { HID_Handle->state = HID_GET_DATA; }
		break;
	    };
	break;

    default:
	break;
    }

return status;
}/*}}}*/
//--------------------------------------------------
static USBH_StatusTypeDef USBH_HID_SOFProcess(USBH_HandleTypeDef *phost)/*{{{*/
{
HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *)phost->pActiveClass->pData;

if(HID_Handle->state == HID_POLL)
    {
    if((phost->Timer - HID_Handle->timer) >= HID_Handle->poll)
	{
	HID_Handle->timer = phost->Timer;

	if( fifo_is_empty(&HID_Handle->fifo) )
	    { HID_Handle->state = HID_GET_DATA; }
	else
	    {
	    fifo_read(&HID_Handle->fifo, HID_Handle->pData, HID_Handle->length);
	    HID_Handle->state = HID_SEND_DATA;
	    }

#if (USBH_USE_OS == 1)
	osMessagePut(phost->os_event, USBH_URB_EVENT, 0);
#endif       
	}
    }
return USBH_OK;
}/*}}}*/
//--------------------------------------------------
// External functions
//--------------------------------------------------
uint16_t USBH_HID_SendData(USBH_HandleTypeDef *phost, const void *buf, uint16_t nbytes)/*{{{*/
{
HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *)phost->pActiveClass->pData;

return fifo_write(&HID_Handle->fifo, buf, nbytes);
}/*}}}*/
//--------------------------------------------------
// Report desciptor functions
//--------------------------------------------------
USBH_StatusTypeDef USBH_HID_GetHIDDescriptor(USBH_HandleTypeDef *phost, uint16_t length)/*{{{*/
{
USBH_StatusTypeDef status;
status = USBH_GetDescriptor(phost,
			    USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD,                                  
			    USB_DESC_HID,
			    phost->device.Data,
			    length);
return status;
}/*}}}*/
//--------------------------------------------------
USBH_StatusTypeDef USBH_HID_GetHIDReportDescriptor(USBH_HandleTypeDef *phost, uint16_t length)/*{{{*/
{
USBH_StatusTypeDef status;

status = USBH_GetDescriptor(phost,
			    USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_STANDARD,                                  
			    USB_DESC_HID_REPORT, 
			    phost->device.Data,
			    length);
return status;
}/*}}}*/
//--------------------------------------------------
USBH_StatusTypeDef USBH_HID_SetIdle(USBH_HandleTypeDef *phost, uint8_t duration, uint8_t reportId)/*{{{*/
{
phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |\
				       USB_REQ_TYPE_CLASS;

phost->Control.setup.b.bRequest = USB_HID_SET_IDLE;
phost->Control.setup.b.wValue.w = (duration << 8 ) | reportId;

phost->Control.setup.b.wIndex.w = 0;
phost->Control.setup.b.wLength.w = 0;

return USBH_CtlReq(phost, 0, 0);
}/*}}}*/
//--------------------------------------------------
USBH_StatusTypeDef USBH_HID_SetProtocol(USBH_HandleTypeDef *phost, uint8_t protocol)/*{{{*/
{
phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |\
				       USB_REQ_TYPE_CLASS;

phost->Control.setup.b.bRequest = USB_HID_SET_PROTOCOL;
phost->Control.setup.b.wValue.w = protocol != 0 ? 0 : 1;
phost->Control.setup.b.wIndex.w = 0;
phost->Control.setup.b.wLength.w = 0;

return USBH_CtlReq(phost, 0 , 0 );
}/*}}}*/
//--------------------------------------------------
USBH_StatusTypeDef USBH_HID_GetReport(USBH_HandleTypeDef *phost,/*{{{*/
				      uint8_t reportType,
				      uint8_t reportId,
				      uint8_t* reportBuff,
				      uint8_t reportLen)
{
phost->Control.setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE |\
				       USB_REQ_TYPE_CLASS;

phost->Control.setup.b.bRequest = USB_HID_GET_REPORT;
phost->Control.setup.b.wValue.w = (reportType << 8 ) | reportId;

phost->Control.setup.b.wIndex.w  = 0;
phost->Control.setup.b.wLength.w = reportLen;

return USBH_CtlReq(phost, reportBuff, reportLen);
}/*}}}*/
//--------------------------------------------------
USBH_StatusTypeDef USBH_HID_SetReport(USBH_HandleTypeDef *phost,/*{{{*/
				      uint8_t reportType,
				      uint8_t reportId,
				      uint8_t* reportBuff,
				      uint8_t reportLen)
{
phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE |
				       USB_REQ_TYPE_CLASS;

phost->Control.setup.b.bRequest = USB_HID_SET_REPORT;
phost->Control.setup.b.wValue.w = (reportType << 8 ) | reportId;

phost->Control.setup.b.wIndex.w  = 0;
phost->Control.setup.b.wLength.w = reportLen;

return USBH_CtlReq(phost, reportBuff, reportLen);
}/*}}}*/
//--------------------------------------------------
static void  USBH_HID_ParseHIDDesc(HID_DescTypeDef *desc, uint8_t *buf)/*{{{*/
{
desc->bLength               = *(uint8_t  *)(buf + 0);
desc->bDescriptorType       = *(uint8_t  *)(buf + 1);
desc->bcdHID                =  LE16(buf + 2);
desc->bCountryCode          = *(uint8_t  *)(buf + 4);
desc->bNumDescriptors       = *(uint8_t  *)(buf + 5);
desc->bReportDescriptorType = *(uint8_t  *)(buf + 6);
desc->wItemLength           =  LE16(buf + 7);
} /*}}}*/
//--------------------------------------------------
// FIFO functions
//--------------------------------------------------
void fifo_init(FIFO_TypeDef *f, uint8_t *buf, uint16_t size)/*{{{*/
{
f->head = 0;
f->tail = 0;
f->lock = 0;
f->size = size;
f->buf = buf;
}/*}}}*/
//--------------------------------------------------
uint16_t fifo_is_empty(FIFO_TypeDef *f)
{ return f->tail == f->head ? 1 : 0; }
//--------------------------------------------------
uint16_t fifo_read(FIFO_TypeDef *f, void *buf, uint16_t nbytes)/*{{{*/
{
uint16_t i;
uint8_t *p;

p = (uint8_t*) buf;
if(f->lock == 0)
    {
    f->lock = 1;
    for(i=0; i < nbytes; i++)
	{
	if( f->tail != f->head )
	    { 
	    *p++ = f->buf[f->tail];
	    f->tail++;

	    if( f->tail == f->size )
		{ f->tail = 0; }
	    }
	else
	    {
	    f->lock = 0;
	    return i;
	    }
	}
    }
else
    { return 0; }

f->lock = 0;
return nbytes;
}/*}}}*/
//--------------------------------------------------
uint16_t fifo_write(FIFO_TypeDef *f, const void *buf, uint16_t nbytes)/*{{{*/
{
const uint8_t *p;
uint16_t i;

p = (const uint8_t *)buf;
if(f->lock == 0)
    {
    f->lock = 1;
    for(i=0; i < nbytes; i++)
	{
	if( (f->head + 1 == f->tail) ||
	   ((f->head + 1 == f->size) && (f->tail == 0)) )
	    {
	    f->lock = 0;
	    return i;
	    } 
	else 
	    {
	    f->buf[f->head] = *p++;
	    f->head++;

	    if(f->head == f->size)
		{ f->head = 0; }
	    }
	}
    }
else
    { return 0; }

f->lock = 0;
return nbytes;
}/*}}}*/
//--------------------------------------------------
__weak void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) { }
//--------------------------------------------------
