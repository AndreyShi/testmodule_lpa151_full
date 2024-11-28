//--------------------------------------------------
// Module control commands
//--------------------------------------------------
#include <stdio.h>

#include "module_commands.hpp"
#include "emu_u.h"
#include "gpio_if.h"
#include "relay_if.h"

//--------------------------------------------------
cmd_status_t KSetLedCmd::operator()()
{
if(led_set(led(), state()))
    {
    if(state() == STATE_ON)
	{ printf("SetLed.%d.on.failed.\n", led() - LED_BASE); }
    else
	{ printf("SetLed.%d.off.failed.\n", led() - LED_BASE); }

    return CMD_STATUS_FAILED;
    }
else
    {
    if(state() == STATE_ON)
	{ printf("SetLed.%d.on.ok.\n", led() - LED_BASE); }
    else
	{ printf("SetLed.%d.off.ok.\n", led() - LED_BASE); }

    return CMD_STATUS_DONE;
    }
}
//--------------------------------------------------
cmd_status_t KSetRelayCmd::operator()()
{
uint8_t retval;

retval = relay_set(relay(), channel(), state());
if(retval == 255)
    {
    if(state() == STATE_ON)
	{ printf("SetRelay.%d.%d.on.ok.\n", relay() - RELAY_BASE, channel()); }
    else
	{ printf("SetRelay.%d.%d.off.ok.\n", relay() - RELAY_BASE, channel()); }
    }
else if(retval != 0)
    {
    if(state() == STATE_ON)
	{ printf("SetRelay.%d.%d.on.failed.\n", relay() - RELAY_BASE, channel()); }
    else
	{ printf("SetRelay.%d.%d.off.failed.\n", relay() - RELAY_BASE, channel()); }
    }
else
    { return CMD_STATUS_REPEAT; }

return CMD_STATUS_DONE;
}
//--------------------------------------------------
cmd_status_t KSetDacCmd::operator()()
{
switch(emuu_set(value()))
    {
    case 0:
	printf("SetDac.0.%d.ok.\n", value());
	return CMD_STATUS_DONE;

    case 1:
	printf("SetDac.failed.dac not ready.\n");
	break;

    case 2:
	printf("SetDac.failed.value out of range.\n");
	break;

    case 3:
	printf("SetDac.failed.data send error.\n");
	break;
    }

return CMD_STATUS_FAILED;
}
//--------------------------------------------------
cmd_status_t KSetFDacCmd::operator()()
{
printf("SetFDac.failed.dac output calibration not set.\n");
return CMD_STATUS_DONE;
}
//--------------------------------------------------
