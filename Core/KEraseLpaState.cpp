//--------------------------------------------------
// Состояния, через которые проходит стирание памяти
// барьера
//--------------------------------------------------
#include "firmware.h"
#include "isp_if.h"

#include "KEraseLpaState.hpp"

// static members
//--------------------------------------------------
static KEraseLpaIdle      erase_idle;
static KEraseLpaEnter     erase_enter;
static KEraseLpaSigCheck  erase_sig_check;
static KEraseLpaChipErase erase_chip_erase;
static KEraseLpaExit      erase_exit;
//--------------------------------------------------
KEraseLpaState *ers_idle = &erase_idle;
//--------------------------------------------------
KEraseLpaState *KEraseLpaState::m_current_state = &erase_idle;
cmd_status_t KEraseLpaState::m_result = CMD_STATUS_FAILED;
//--------------------------------------------------
KEraseLpaState::KEraseLpaState() :/*{{{*/
	idle(&erase_idle),
	enter(&erase_enter),
	sig_check(&erase_sig_check),
	chip_erase(&erase_chip_erase),
	exit(&erase_exit)
{ }/*}}}*/
//--------------------------------------------------
KEraseLpaState::~KEraseLpaState()
{ }
//--------------------------------------------------
cmd_status_t KEraseLpaState::operator()()/*{{{*/
{
if(m_current_state)
    { return m_current_state->functor(); }

printf("EraseLpa.failed.current state null.\n");
return CMD_STATUS_FAILED;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KEraseLpaState::result() const/*{{{*/
{
switch(m_result)
    {
    case CMD_STATUS_DONE:
	printf("EraseLpa.ok.\n");
	break;

    case CMD_STATUS_FAILED:
	break;

    default:
	printf("EraseLpa.failed.unknown result.\n");
	break;
    };

return m_result;
}/*}}}*/
//--------------------------------------------------
void KEraseLpaState::set_result(cmd_status_t s)
{ m_result = s; }
//--------------------------------------------------
/* transitions here {{{*/
void KEraseLpaState::to_idle()
{ m_current_state = idle; }
//--------------------------------------------------
void KEraseLpaState::to_enter()
{ m_current_state = enter; }
//--------------------------------------------------
void KEraseLpaState::to_sig_check()
{ m_current_state = sig_check; }
//--------------------------------------------------
void KEraseLpaState::to_chip_erase()
{ m_current_state = chip_erase; }
//--------------------------------------------------
void KEraseLpaState::to_exit()
{ m_current_state = exit; }/*}}}*/
//--------------------------------------------------
cmd_status_t KEraseLpaIdle::functor()/*{{{*/
{
set_result(CMD_STATUS_FAILED);
isp_go_fast();
to_enter();
return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KEraseLpaEnter::functor()/*{{{*/
{
uint8_t val;

val = isp_enter_nb();
if(val == 255)
    { to_sig_check(); }
else if(val == 0)
    { return CMD_STATUS_REPEAT; }
else if(isp_is_fast() == 1)
    { isp_go_slow(); }
else
    {
    isp_parse_error("EraseLpa", val);
    to_exit();
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KEraseLpaSigCheck::functor()/*{{{*/
{
uint8_t val;
uint32_t sig;

sig = 0;
val = isp_read_signature_nb(&sig);

if(val == 0)
    { return CMD_STATUS_REPEAT; }
else if(val != 255)
    {
    printf("EraseLpa.failed.unable to read signature.\n");
    to_exit();
    }
else if(sig == isp_atmega32u4_signature ||
	sig == isp_atmega16u4_signature)
    { to_chip_erase(); }
else
    {
    if(isp_is_fast() == 1)
	{ isp_go_slow(); }
    else
	{
	printf("EraseLpa.failed.invalid device signature %06X.\n", sig);
	to_exit();
	}
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KEraseLpaChipErase::functor()/*{{{*/
{
uint8_t val;

val = isp_chip_erase_nb();
if(val != 0)
    {
    if(val == 255)
	{ set_result(CMD_STATUS_DONE); }
    else
	{ printf("EraseLpa.failed.chip erase error %d.\n", val); }

    to_exit();
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KEraseLpaExit::functor()/*{{{*/
{
isp_exit();
to_idle();
return result();
}/*}}}*/
//--------------------------------------------------
