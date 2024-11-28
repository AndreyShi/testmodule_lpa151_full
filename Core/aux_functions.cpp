//--------------------------------------------------
// Auxilliary commands
//--------------------------------------------------
#include <stdio.h>

#include "app_export.h"


#include "module_types.hpp"
#include "module_commands.hpp"

#include "firmware.h"

#include "version.hpp"

//--------------------------------------------------
cmd_status_t KSetChannelCountCmd::operator()()/*{{{*/
{
if(count() > module_info<MODULE_LPA_151>::max_channels)
    {
    printf("SetChannelCount.%d.failed.exceeded.\n", count());
    return CMD_STATUS_FAILED;
    }

channel_count() = count();
printf("SetChannelCount.%d.ok.\n", count());
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KReadVersionCmd::operator()()/*{{{*/
{
char buff[15];

switch(version_id())
    {
    case TM_151_VERSION_MODULE:
	sprintf(buff, "%d.%d.%d.%d",
		module_version.major(),
		module_version.minor(),
		module_version.patch(),
		module_version.build());
	break;
	
    default:
	sprintf(buff, "0.0.0.0");
	break;
    };

u_register().clear();
u_register().append(buff);

printf("ReadVersion.%s.ok.\n", buff);
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KReadDataCmd::operator()()/*{{{*/
{
char *buff;
uint32_t sz;

buff = 0;
switch(data_id())
    {
    case TM_151_DATA_LOCK:
	buff = new char[FLASH_LOCK_SIZE];
	sz = FLASH_LOCK_SIZE;
	break;

    case TM_151_DATA_FUSE_WORK:
    case TM_151_DATA_FUSE_CAL:
	buff = new char[FLASH_FUSE_SIZE];
	sz = FLASH_FUSE_SIZE;
	break;

    case TM_151_DATA_EEPROM_FACTORY:
	buff = new char[EEPROM_FACTORY_SIZE];
	sz = EEPROM_FACTORY_SIZE;
	break;

    case TM_151_DATA_EEPROM_USER:
	buff = new char[EEPROM_USER_SIZE];
	sz = EEPROM_USER_SIZE;
	break;

    case TM_151_DATA_FLASH_WORK_FW:
    case TM_151_DATA_FLASH_CAL_FW:
	buff = new char[FLASH_FW_SIZE];
	sz = FLASH_FW_SIZE;
	break;

    case TM_151_DATA_FLASH_BL:
	buff = new char[FLASH_BL_SIZE];
	sz = FLASH_BL_SIZE;
	break;

    default:
	printf("ReadData.failed.invalid id.\n");
	return CMD_STATUS_FAILED;
    };

if(firmware_get(data_id(), (uint8_t *)buff, sz, 0) != QSPI_OK)
    {
    printf("ReadData.failed.firmware_get.\n");
    delete[] buff;
    return CMD_STATUS_FAILED;
    }

u_register().clear();
u_register().append(buff, sz);

printf("ReadData.%d.ok.\n", data_id());
delete[] buff;
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KWriteDataCmd::operator()()/*{{{*/
{
if(firmware_put(data_id(), (const uint8_t *)data().c_str(), data().size(), offset()) != QSPI_OK)
    {
    printf("WriteData.failed.\n");
    return CMD_STATUS_FAILED;
    }

printf("WriteData.%d.%d.%d.ok.\n", data_id(), offset(), data().size());
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KBurnDataCmd::operator()()/*{{{*/
{
uint8_t val;

val = firmware_save_nb();
if(val == 0)
    {
    QSPI_Flash_Task();
    return CMD_STATUS_REPEAT;
    }

if(val == 255)
    {
    printf("BurnData.ok.\n");
    return CMD_STATUS_DONE;
    }
else
    {
    printf("BurnData.failed.%d.\n", val);
    return CMD_STATUS_FAILED;
    }
}/*}}}*/
//--------------------------------------------------
cmd_status_t KReadScriptCmd::operator()()/*{{{*/
{
char *buff;

if(script_id() - SCRIPT_ID_BASE <= TM_151_SCRIPTS_COUNT && size() <= TM_151_SCRIPT_SIZE)
    { buff = (char *)firmware_get_script(script_id()); }
else
    {
    printf("ReadScript.failed.invalid id.\n");
    return CMD_STATUS_FAILED;
    }

u_register().clear();
u_register().append(buff, TM_151_SCRIPT_SIZE);

printf("ReadScript.%d.ok.\n", script_id());
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KWriteScriptCmd::operator()()/*{{{*/
{
// TODO: offset?
if(script_id() - SCRIPT_ID_BASE <= TM_151_SCRIPTS_COUNT && size() <= TM_151_SCRIPT_SIZE)
    { firmware_put_script(script_id(), (enum cmd_id_t *)script().c_str(), script().size()); }
else
    {
    printf("WriteScript.failed.invalid id.\n");
    return CMD_STATUS_FAILED;
    }

printf("WriteScript.ok.\n");
return CMD_STATUS_DONE;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KBurnScriptCmd::operator()()/*{{{*/
{
uint8_t val;

val = firmware_save_nb();
if(val == 0)
    {
    QSPI_Flash_Task();
    return CMD_STATUS_REPEAT;
    }

if(val == 255)
    {
    printf("BurnScript.ok.\n");
    return CMD_STATUS_DONE;
    }
else
    {
    printf("BurnScript.failed.%d.\n", val);
    return CMD_STATUS_FAILED;
    }
}/*}}}*/
//--------------------------------------------------
cmd_status_t KReadArray32Cmd::operator()()/*{{{*/
{
printf("ReadArray32.failed.not implemented.\n");
return CMD_STATUS_FAILED;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KWriteArray32Cmd::operator()()/*{{{*/
{
printf("WriteArray32.failed.not implemented\n");
return CMD_STATUS_FAILED;
}/*}}}*/
//--------------------------------------------------
