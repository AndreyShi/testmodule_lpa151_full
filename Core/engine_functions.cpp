//--------------------------------------------------
// Engine commands
//--------------------------------------------------
#include <stdio.h>

#include "stm32f7xx_hal.h"

#include "module_commands.hpp"
#include "command_macros.hpp"

//--------------------------------------------------
cmd_status_t KNopCmd::operator()()/*{{{*/
{
printf("Nop.ok.\n");
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KStageCmd::operator()()/*{{{*/
{
stage() = annotation();
printf("SetStage.%s.ok.\n", annotation().c_str());
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KStartCmd::operator()()/*{{{*/
{
if(E->load_script(script_no()) == CMD_STATUS_DONE)
    {
    printf("Start.ok.\n");
    return CMD_STATUS_DONE;
    }
else
    {
    printf("Start.failed.\n");
    return CMD_STATUS_FAILED;
    }
}/*}}}*/
//--------------------------------------------------
cmd_status_t KStopCmd::operator()()/*{{{*/
{
printf("Stop.ok.\n");
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KEndCmd::operator()()/*{{{*/
{
printf("End.ok.\n");
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KWaitCmd::operator()()/*{{{*/
{
uint32_t now = HAL_GetTick();

if( !mark_set )
    {
    mark = now;
    mark_set = true;
    }

if(now - mark < delay())
    { return CMD_STATUS_REPEAT; }

mark_set = false;
printf("Wait.%d.ok.\n", delay());
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KLessCmd::operator()()/*{{{*/
{
if(i_registers()[engine_register()] >= reference())
    {
    e_register() = error_code();
    printf("Less.failed.not less.\n");
    }
else
    { printf("Less.ok.\n"); }

return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KEqualCmd::operator()()/*{{{*/
{
if(i_registers()[engine_register()] != reference())
    {
    e_register() = error_code();
    printf("Equal.failed.not equal.\n");
    }
else
    { printf("Equal.ok.\n"); }

return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KMoreCmd::operator()()/*{{{*/
{
if(i_registers()[engine_register()] <= reference())
    {
    e_register() = error_code();
    printf("More.failed.not more.\n");
    }
else
    { printf("More.ok.\n"); }

return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KFLessCmd::operator()()/*{{{*/
{
if(f_registers()[engine_register()] >= reference())
    {
    e_register() = error_code();
    printf("FLess.failed.not less.\n");
    }
else
    { printf("FLess.ok.\n"); }

return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KFEqualCmd::operator()()/*{{{*/
{
if(f_registers()[engine_register()] != reference())
    {
    e_register() = error_code();
    printf("FEqual.failed.not equal.\n");
    }
else
    { printf("FEqual.ok.\n"); }

return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KFMoreCmd::operator()()/*{{{*/
{
if(f_registers()[engine_register()] <= reference())
    {
    e_register() = error_code();
    printf("FMore.failed.not more.\n");
    }
else
    { printf("FMore.ok.\n"); }

return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KSkipChCmd::operator()()/*{{{*/
{
if(E->channel_count() == 1)
    {
    next_step() = skip();
    printf("SkipCh.ok.\n");
    }
else
    { printf("SkipCh.failed.no skip taken.\n"); }

return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KDumpCmd::operator()()/*{{{*/
{
std::string tmp;

printf("Dump.%d.", reg());
if(reg() > 2+E->i_registers().size()+E->f_registers().size() &&
   reg() < 0xFF)
    {
    printf("failed.\n");
    return CMD_STATUS_FAILED;
    } 
else
    { printf("ok."); }

if(reg() == 0 || reg() == 0xFF)
    { printf("%d.", E->s_register()); }
if(reg() == 1 || reg() == 0xFF)
    { printf("%d.", E->e_register()); }
if(reg() == 2 || reg() == 0xFF)
    {
    tmp = E->u_register();
    if(tmp.size() == 0)
	{ printf("''"); }
    else if(tmp.size() == 1)
	{ printf("%02X", tmp[0]); }
    else
	{
	printf("%02X", tmp[0]);
	for(uint8_t i=1; i<tmp.size(); i++)
	    { printf(" %02X", tmp[i]); }
	}
    printf(".");
    }
if((reg() > 2 &&
	 reg() < 2+E->i_registers().size()) ||
	reg() == 0xFF)
    {
    if(reg() != 0xFF)
	{ printf("%d.", E->i_registers()[reg()-3]); }
    else
	{
	for(uint8_t i=0; i<E->i_registers().size(); i++)
	    { printf("%d.", E->i_registers()[i]); }
	}
    }
if((reg() > 2+E->i_registers().size() &&
	 reg() < 2+E->i_registers().size()+E->f_registers().size()) ||
	reg() == 0xFF)
    {
    if(reg() != 0xFF)
	{ printf("%f.", E->f_registers()[reg()-3]); }
    else
	{
	for(uint8_t i=0; i<E->f_registers().size(); i++)
	    { printf("%f.", E->f_registers()[i]); }
	}
    }

printf("\n");
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
