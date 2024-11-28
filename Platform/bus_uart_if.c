//--------------------------------------------------
// Интерфейс к UART связи с шиной,
// используется для общения с внешним миром
//--------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "app_config.h"
#include "tim.h"
#include "usart.h"

#include "bus_uart_if.h"
#include "gpio_if.h"

//--------------------------------------------------
typedef enum _bus_state {/*{{{*/
    BUS_STARTUP,
    BUS_IDLE,
    BUS_FRAME,
} bus_state_t;/*}}}*/
//--------------------------------------------------
__attribute__((section(".busInBuffSection")))
static uint8_t  in_buff[MAX_BUS_BUFF_SIZE];
static uint16_t in_buff_payload;

__attribute__((section(".busOutBuffSection")))
static uint8_t  out_buff[MAX_BUS_BUFF_SIZE];

static bus_flags_t m_bus_flags;
static bus_state_t m_bus_state;
//--------------------------------------------------
void bus_init()/*{{{*/
{
in_buff_payload = 0;

m_bus_flags.incoming = 0;
m_bus_flags.outgoing = 0;

if(FRAME_IN_STATE == GPIO_PIN_SET)
    { m_bus_state = BUS_IDLE; }
else
    { m_bus_state = BUS_STARTUP; }

FRAME_STOP;
}/*}}}*/
//--------------------------------------------------
bus_flags_t bus_state(void)
{ return m_bus_flags; }
//--------------------------------------------------
void bus_frame_get(char **buff, uint32_t *sz)/*{{{*/
{
char *in;
in = malloc(in_buff_payload);
if(in == NULL)
    {
    *buff = 0;
    *sz   = 0;
    return;
    }

memcpy(in, in_buff, in_buff_payload);

*buff = in;
*sz   = in_buff_payload;

m_bus_flags.incoming = 0;
}/*}}}*/
//--------------------------------------------------
void bus_out_buff_get(char **buff)
{ (*buff) = (char *)out_buff; }
//--------------------------------------------------
bus_error_t bus_send(const uint32_t sz)/*{{{*/
{
if(sz > MAX_BUS_BUFF_SIZE)
    { return BUS_TOO_LONG; }

if(m_bus_flags.outgoing)
    { return BUS_BUSY; }

m_bus_flags.outgoing = 1;

FRAME_START;
HAL_UART_Transmit_IT(&bus_uart, out_buff, sz);
return BUS_OK;
}/*}}}*/
//--------------------------------------------------
void bus_gpio_isr(void)/*{{{*/
{
switch(m_bus_state)
    {
    case BUS_STARTUP:
	if(FRAME_IN_STATE == GPIO_PIN_SET)
	    { m_bus_state = BUS_IDLE; }
	break;

    default:
    case BUS_IDLE:
	if(FRAME_IN_STATE == GPIO_PIN_SET)
	    { break; }

	HAL_UART_Receive_DMA(&bus_uart, in_buff, MAX_BUS_BUFF_SIZE);
	m_bus_state = BUS_FRAME;
	break;

    case BUS_FRAME:
	if(FRAME_IN_STATE == GPIO_PIN_RESET)
	    { break; }

	in_buff_payload = MAX_BUS_BUFF_SIZE - bus_uart.hdmarx->Instance->NDTR;
	HAL_UART_DMAStop(&bus_uart);

	if(in_buff_payload > MAX_BUS_BUFF_SIZE)
	    { in_buff_payload = MAX_BUS_BUFF_SIZE; }

	if(in_buff_payload > 2)
	    { m_bus_flags.incoming = 1; } 

	m_bus_state = BUS_IDLE;
	break;
    };
}/*}}}*/
//--------------------------------------------------
void bus_uart_isr(void)/*{{{*/
{
HAL_TIM_Base_Start_IT(&bus_tim);

FRAME_STOP;
}/*}}}*/
//--------------------------------------------------
void bus_tim_isr(void)/*{{{*/
{
HAL_TIM_Base_Stop_IT(&bus_tim);

m_bus_flags.outgoing = 0;
}/*}}}*/
//--------------------------------------------------
