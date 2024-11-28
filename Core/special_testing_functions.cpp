//--------------------------------------------------
// Special testing commands
//--------------------------------------------------
#include "stm32f7xx_hal.h"
#include "boot.h"

#include "module_commands.hpp"

//--------------------------------------------------
cmd_status_t KModuleResetCmd::operator()()
{
printf("ModuleReset.ok.\n");
NVIC_SystemReset();
return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t KModuleEnterBootCmd::operator()()
{
printf("ModuleEnterBoot.ok.\n");
boot_enter();

/* never reached though */
return CMD_STATUS_DONE;
}
//--------------------------------------------------
