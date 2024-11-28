//--------------------------------------------------
// Module read commands
//--------------------------------------------------
#include <stdio.h>

#include "module_commands.hpp"

//--------------------------------------------------
cmd_status_t KReadButtonCmd::operator()()
{
printf("ReadButton.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t KReadInputCmd::operator()()
{
printf("ReadInput.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t KReadAdcCmd::operator()()
{
printf("ReadAdc.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t KReadDelayCmd::operator()()
{
printf("ReadDelay.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
