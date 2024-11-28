//--------------------------------------------------
// Менеджер общения с шиной.
// Разбирает входящие пакеты и управляет адресацией.
//--------------------------------------------------
#include <stdlib.h>

#include "stm32f7xx_hal.h"
#include "bus_uart_if.h"

#include "KBusManager.hpp"
#include "frame_headers.hpp"
#include "bus_frames.hpp"

//--------------------------------------------------
#define ADDRESS_REQUEST_TIMEOUT 500 // ms

//--------------------------------------------------
KBusManager::KBusManager() :
	m_clock(0)
{ }
//--------------------------------------------------
void KBusManager::request_address()/*{{{*/
{
if(bus_address() != ADDR_NOT_SET)
    { return; }

uint32_t now;

now = HAL_GetTick();
if(now - m_clock < ADDRESS_REQUEST_TIMEOUT)
    { return; }

m_clock = now;

KBusFrameHeader *header;
KNumRequestFrame *frame;

header = new KHeaderUnicast(ADDR_NOT_SET, ADDR_HEAD);
frame = new KNumRequestFrame();
frame->set_header(header);

enqueue_frame(frame);
}/*}}}*/
//--------------------------------------------------
void KBusManager::process_incoming()/*{{{*/
{
if(bus_state().incoming == 0)
    { return; }

bool ok;
char *cur;
char *buff;
uint32_t size;
KBusFrameHeader *header;
KBusFrame *frame;

ok = true;
header = NULL;
frame = NULL;

bus_frame_get(&cur, &size);
if(cur == 0 || size == 0)
    { return; }

buff = cur;
header = KBusFrameHeader::deserialize(&cur, size);
if(header == NULL)
    { ok = false; }

frame = KBusFrame::deserialize(&cur, size);
if(frame == NULL)
    { ok = false; }

if(ok)
    {
    // catch only relevant frames
    // irrelevant ones go directly out
    frame->set_header(header);
    if( frame->set_accepted(bus_address()) )
	{
	set_frame_number(header->number());
	in_traffic().push_back(frame);
	}
    else
	{ out_traffic().push_back(frame); }
    }
else
    {
    if(header != NULL)
	{ delete header; }
    if(frame != NULL)
	{ delete frame; }
    }

free(buff);
}/*}}}*/
//--------------------------------------------------
void KBusManager::process_outgoing()/*{{{*/
{
if(out_traffic().size() == 0)
    { return; }

if(bus_state().outgoing)
    { return; }

char *buff;
uint32_t size;
KBusFrame *frame;

frame = out_traffic().front();
out_traffic().pop_front();
if( !frame->header() )
    {
    delete frame;
    return;
    }

bus_out_buff_get(&buff);
size = MAX_BUS_BUFF_SIZE;
if(frame->header()->serialize(&buff, size))
    {
    if(frame->serialize(&buff, size))
	{
	size = MAX_BUS_BUFF_SIZE - size;
	bus_send(size);
	}
    }

delete frame;
}/*}}}*/
//--------------------------------------------------
