//--------------------------------------------------
// Device-specific commands
//--------------------------------------------------
#include <stdio.h>

#include "module_commands.hpp"

//--------------------------------------------------
cmd_status_t KReadModuleCountCmd::operator()()
{
printf("ReadModuleCount.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t KControlMeterCmd::operator()()
{
printf("ControlMeter.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t K142AdjustCmd::operator()()
{
printf("142Adjust.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t KFireCmd::operator()()
{
printf("FireCmd.failed.not implemented.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
