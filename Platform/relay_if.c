//--------------------------------------------------
// Модуль управления дискретными выходами
//--------------------------------------------------
#include "usbh_core.h"
#include "usb_host.h"

#include "gpio_if.h"
#include "relay_if.h"

#include "app_config.h"
#include "KModuleState151.h"

//--------------------------------------------------
typedef enum {/*{{{*/
    RELAY_USB_START = 0,
    RELAY_USB_WAIT1,
    RELAY_USB_WAIT2,
    RELAY_USB_WAIT3,
    RELAY_USB_WAIT4,
    RELAY_USB_WAIT5,
} relay_usb_state_t;/*}}}*/
//--------------------------------------------------
extern USBH_HandleTypeDef hUsbHostHS;
//--------------------------------------------------
const static uint32_t relay_to =  30; // ms
const static uint32_t usb_to   = 100; // ms
//--------------------------------------------------
static uint32_t relay_timer;
static uint32_t clock;

static relay_usb_state_t relay_usb_state = RELAY_USB_START;
//--------------------------------------------------
static void map_relay(relay_id_t relay, channels_t ch, state_t sw, state_t state);
static uint8_t set_usb_relay(state_t sw);
//--------------------------------------------------
void relay_init(void)/*{{{*/
{
/* isp is controlled by module itself, not directly by user */
LPA_ISP_DISABLE;
LPA_ISP_UNRESET;
/* keep the state of the usb power, do not touch it here */
}/*}}}*/
//--------------------------------------------------
uint8_t relay_set(const relay_id_t relay, const channels_t ch, const state_t state)/*{{{*/
{
if(relay == TM_151_RELAY_LPA_USB)
    { return set_usb_relay(state); }

RELAY_EN;
map_relay(relay, ch, state, STATE_ON);
relay_timer = relay_to;

update_state(TM_151_RELAYS, state, relay);
return 255;
}/*}}}*/
//--------------------------------------------------
void relay_systick_isr()/*{{{*/
{
if(relay_timer == 0 ||
   relay_timer > relay_to)
    {/*{{{*/
    // just clear all relay pins
    GPIOB->BSRR =
	GPIO_BSRR_BR_0  |
	GPIO_BSRR_BR_1  |
	GPIO_BSRR_BR_8  |
	GPIO_BSRR_BR_9  |
	GPIO_BSRR_BR_12 |
	GPIO_BSRR_BR_13;
    GPIOC->BSRR =
	GPIO_BSRR_BR_0  |
	GPIO_BSRR_BR_1  |
	GPIO_BSRR_BR_2  |
	GPIO_BSRR_BR_3  |
	GPIO_BSRR_BR_10 |
	GPIO_BSRR_BR_11 |
	GPIO_BSRR_BR_12;
    GPIOD->BSRR =
	GPIO_BSRR_BR_0  |
	GPIO_BSRR_BR_1  |
	GPIO_BSRR_BR_2  |
	GPIO_BSRR_BR_4  |
	GPIO_BSRR_BR_5  |
	GPIO_BSRR_BR_6  |
	GPIO_BSRR_BR_7  |
	GPIO_BSRR_BR_14 |
	GPIO_BSRR_BR_15;
    GPIOE->BSRR =
	GPIO_BSRR_BR_0  |
	GPIO_BSRR_BR_1  |
	GPIO_BSRR_BR_7  |
	GPIO_BSRR_BR_8  |
	GPIO_BSRR_BR_9  |
	GPIO_BSRR_BR_10 |
	GPIO_BSRR_BR_11 |
	GPIO_BSRR_BR_12 |
	GPIO_BSRR_BR_13 |
	GPIO_BSRR_BR_14 |
	GPIO_BSRR_BR_15;
    GPIOF->BSRR =
	GPIO_BSRR_BR_0  |
	GPIO_BSRR_BR_1  |
	GPIO_BSRR_BR_2  |
	GPIO_BSRR_BR_3  |
	GPIO_BSRR_BR_4  |
	GPIO_BSRR_BR_11 |
	GPIO_BSRR_BR_12 |
	GPIO_BSRR_BR_13 |
	GPIO_BSRR_BR_14 |
	GPIO_BSRR_BR_15;
    GPIOH->BSRR =
	GPIO_BSRR_BR_2  |
	GPIO_BSRR_BR_3  |
	GPIO_BSRR_BR_4  |
	GPIO_BSRR_BR_5  |
	GPIO_BSRR_BR_6  |
	GPIO_BSRR_BR_7  |
	GPIO_BSRR_BR_8  |
	GPIO_BSRR_BR_9  |
	GPIO_BSRR_BR_10 |
	GPIO_BSRR_BR_11 |
	GPIO_BSRR_BR_12;
    GPIOI->BSRR =
	GPIO_BSRR_BR_4  |
	GPIO_BSRR_BR_5  |
	GPIO_BSRR_BR_6  |
	GPIO_BSRR_BR_7  |
	GPIO_BSRR_BR_8  |
	GPIO_BSRR_BR_9  |
	GPIO_BSRR_BR_10 |
	GPIO_BSRR_BR_11 |
	GPIO_BSRR_BR_12 |
	GPIO_BSRR_BR_13 |
	GPIO_BSRR_BR_14 |
	GPIO_BSRR_BR_15;

    RELAY_DIS;
    relay_timer = 0;
    }/*}}}*/
else
    { relay_timer--; }
}/*}}}*/
//--------------------------------------------------
// put relay relay, switch sw (set or reset) into state state (on or off)
static void map_relay(relay_id_t relay, channels_t ch, state_t sw, state_t state)/*{{{*/
{
switch(relay)
    {
    case TM_151_RELAY_SHORT:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOC->BSRR = GPIO_BSRR_BS_10; }
	    else
		{ GPIOC->BSRR = GPIO_BSRR_BS_11; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOC->BSRR = GPIO_BSRR_BR_10; }
	    else
		{ GPIOC->BSRR = GPIO_BSRR_BR_11; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_R1:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOE->BSRR = GPIO_BSRR_BS_12; }
	    else
		{ GPIOE->BSRR = GPIO_BSRR_BS_11; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOE->BSRR = GPIO_BSRR_BR_12; }
	    else
		{ GPIOE->BSRR = GPIO_BSRR_BR_11; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_R2:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOE->BSRR = GPIO_BSRR_BS_14; }
	    else
		{ GPIOE->BSRR = GPIO_BSRR_BS_13; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOE->BSRR = GPIO_BSRR_BR_14; }
	    else
		{ GPIOE->BSRR = GPIO_BSRR_BR_13; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_R3:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BS_6; }
	    else
		{ GPIOE->BSRR = GPIO_BSRR_BS_15; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BR_6; }
	    else
		{ GPIOE->BSRR = GPIO_BSRR_BR_15; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_R4:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BS_8; }
	    else
		{ GPIOH->BSRR = GPIO_BSRR_BS_7; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BR_8; }
	    else
		{ GPIOH->BSRR = GPIO_BSRR_BR_7; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_R5:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BS_10; }
	    else
		{ GPIOH->BSRR = GPIO_BSRR_BS_9; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BR_10; }
	    else
		{ GPIOH->BSRR = GPIO_BSRR_BR_9; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_R6:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BS_12; }
	    else
		{ GPIOH->BSRR = GPIO_BSRR_BS_11; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOH->BSRR = GPIO_BSRR_BR_12; }
	    else
		{ GPIOH->BSRR = GPIO_BSRR_BR_11; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_EMUU_POSITIVE:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOC->BSRR = GPIO_BSRR_BS_12; }
	    else
		{ GPIOD->BSRR = GPIO_BSRR_BS_0; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOC->BSRR = GPIO_BSRR_BR_12; }
	    else
		{ GPIOD->BSRR = GPIO_BSRR_BR_0; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_EMUU_NEGATIVE:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOD->BSRR = GPIO_BSRR_BS_1; }
	    else
		{ GPIOD->BSRR = GPIO_BSRR_BS_2; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOD->BSRR = GPIO_BSRR_BR_1; }
	    else
		{ GPIOD->BSRR = GPIO_BSRR_BR_2; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_LPA_POWER:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOD->BSRR = GPIO_BSRR_BS_14; }
	    else
		{ GPIOD->BSRR = GPIO_BSRR_BS_15; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOD->BSRR = GPIO_BSRR_BR_14; }
	    else
		{ GPIOD->BSRR = GPIO_BSRR_BR_15; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_LPA_USB:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOB->BSRR = GPIO_BSRR_BS_12; }
	    else
		{ GPIOB->BSRR = GPIO_BSRR_BS_13; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOB->BSRR = GPIO_BSRR_BR_12; }
	    else
		{ GPIOB->BSRR = GPIO_BSRR_BR_13; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_SENSE_AG420_1:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOB->BSRR = GPIO_BSRR_BS_8; }
	    else
		{ GPIOB->BSRR = GPIO_BSRR_BS_9; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOB->BSRR = GPIO_BSRR_BR_8; }
	    else
		{ GPIOB->BSRR = GPIO_BSRR_BR_9; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_SENSE_AG420_2:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{ GPIOF->BSRR = GPIO_BSRR_BS_2; }
	    else
		{ GPIOI->BSRR = GPIO_BSRR_BS_12; }
	    }
	else
	    {
	    if(sw == STATE_ON)
		{ GPIOF->BSRR = GPIO_BSRR_BR_2; }
	    else
		{ GPIOI->BSRR = GPIO_BSRR_BR_12; }
	    }
	break;
/*}}}*/
    case TM_151_RELAY_LPA_IN_AG_PLUS :/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BS_12; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BS_2; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BS_13; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BS_3; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BR_12; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BR_2; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BR_13; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BR_3; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_LPA_IN_AG_MINUS:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BS_1; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BS_10; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BS_0; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BS_11; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BR_1; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BR_10; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOF->BSRR = GPIO_BSRR_BR_0; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BR_11; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_AG420_1:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BS_4; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BS_14; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BS_5; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BS_15; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BR_4; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BR_14; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BR_5; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BR_15; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_AG420_2:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BS_3; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BS_5; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BS_2; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BS_4; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BR_3; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BR_5; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BR_2; }
		else
		    { GPIOH->BSRR = GPIO_BSRR_BR_4; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_AG420_1_TR:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BS_10; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BS_3; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BS_9; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BS_4; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BR_10; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BR_3; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BR_9; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BR_4; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_AG420_2_TR:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BS_1; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BS_13; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BS_0; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BS_14; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BR_1; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BR_13; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOE->BSRR = GPIO_BSRR_BR_0; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BR_14; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_AG401_1:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BS_9; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BS_15; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BS_8; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BS_11; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BR_9; }
		else
		    { GPIOI->BSRR = GPIO_BSRR_BR_15; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BR_8; }
		else
		    { GPIOF->BSRR = GPIO_BSRR_BR_11; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_AG401_2:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BS_6; }
		else
		    { GPIOB->BSRR = GPIO_BSRR_BS_0; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BS_7; }
		else
		    { GPIOB->BSRR = GPIO_BSRR_BS_1; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BR_6; }
		else
		    { GPIOB->BSRR = GPIO_BSRR_BR_0; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOI->BSRR = GPIO_BSRR_BR_7; }
		else
		    { GPIOB->BSRR = GPIO_BSRR_BR_1; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_WARMUP:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOD->BSRR = GPIO_BSRR_BS_6; }
		else
		    { GPIOE->BSRR = GPIO_BSRR_BS_7; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOD->BSRR = GPIO_BSRR_BS_7; }
		else
		    { GPIOE->BSRR = GPIO_BSRR_BS_8; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOD->BSRR = GPIO_BSRR_BR_6; }
		else
		    { GPIOE->BSRR = GPIO_BSRR_BR_7; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOD->BSRR = GPIO_BSRR_BR_7; }
		else
		    { GPIOE->BSRR = GPIO_BSRR_BR_8; }
		}
	    }
	break;
/*}}}*/
    case TM_151_RELAY_SENSE:/*{{{*/
	if(state == STATE_ON)
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BS_0; }
		else
		    { GPIOD->BSRR = GPIO_BSRR_BS_4; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BS_1; }
		else
		    { GPIOD->BSRR = GPIO_BSRR_BS_5; }
		}
	    }
	else
	    {
	    if(sw == STATE_ON)
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BR_0; }
		else
		    { GPIOD->BSRR = GPIO_BSRR_BR_4; }
		}
	    else
		{
		if(ch == CH_1)
		    { GPIOC->BSRR = GPIO_BSRR_BR_1; }
		else
		    { GPIOD->BSRR = GPIO_BSRR_BR_5; }
		}
	    }
	break;
/*}}}*/
    default:
	break;
    };
}/*}}}*/
//--------------------------------------------------
uint8_t set_usb_relay(state_t sw)/*{{{*/
{
switch(relay_usb_state)
    {
    default:
    case RELAY_USB_START:
	if(sw == STATE_ON)
	    {
	    RELAY_EN;
	    map_relay(TM_151_RELAY_LPA_USB, CH_ALL, STATE_ON, STATE_ON);
	    relay_timer = relay_to;

	    relay_usb_state = RELAY_USB_WAIT1;
	    }
	else
	    {
	    USBH_Stop(&hUsbHostHS);

	    relay_usb_state = RELAY_USB_WAIT3;
	    }

	clock = HAL_GetTick();
	break;

    // turn on sequence
    case RELAY_USB_WAIT1:
	if(HAL_GetTick() - clock < relay_to)
	    { break; }

	map_relay(TM_151_RELAY_LPA_USB, CH_ALL, STATE_ON, STATE_OFF);
	RELAY_DIS;

	clock = HAL_GetTick();
	relay_usb_state = RELAY_USB_WAIT2;
	break;

    case RELAY_USB_WAIT2:
	if(HAL_GetTick() - clock < usb_to)
	    { break; }

	MX_USB_HOST_Init();
	update_state(TM_151_RELAYS, STATE_ON, TM_151_RELAY_LPA_USB);
	relay_usb_state = RELAY_USB_START;
	return 255;

    // turn off sequence
    case RELAY_USB_WAIT3:
	if(HAL_GetTick() - clock < 10)
	    { break; }

	USBH_DeInit(&hUsbHostHS);
	clock = HAL_GetTick();
	relay_usb_state = RELAY_USB_WAIT4;
	break;

    case RELAY_USB_WAIT4:
	if(HAL_GetTick() - clock < 10)
	    { break; }

	RELAY_EN;
	map_relay(TM_151_RELAY_LPA_USB, CH_ALL, STATE_OFF, STATE_ON);
	relay_timer = relay_to;

	clock = HAL_GetTick();
	relay_usb_state = RELAY_USB_WAIT5;
	break;

    case RELAY_USB_WAIT5:
	if(HAL_GetTick() - clock < relay_to)
	    { break; }

	map_relay(TM_151_RELAY_LPA_USB, CH_ALL, STATE_OFF, STATE_OFF);
	RELAY_DIS;
	update_state(TM_151_RELAYS, STATE_OFF, TM_151_RELAY_LPA_USB);
	relay_usb_state = RELAY_USB_START;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
