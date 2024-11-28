//--------------------------------------------------
// Состояния, через которые проходят все процессы
// обновления прошивки барьера
//--------------------------------------------------
#include "KUpdateLpaState.hpp"

#include "app_config.h"
#include "firmware.h"
#include "isp_if.h"

// static members
//--------------------------------------------------
static KUpdateLpaIdle     update_idle;
static KUpdateLpaEnter    update_enter;
static KUpdateLpaSigCheck update_sig_check;

static KUpdateLpaDispatch update_dispatch;

static KUpdateLpaBurnLock update_burn_lock;
static KUpdateLpaBurnFuse update_burn_fuse;

static KUpdateLpaBurnEEFactory   update_burn_ee_factory;
static KUpdateLpaBurnEEUser      update_burn_ee_user;
static KUpdateLpaVerifyEEFactory update_verify_ee_factory;
static KUpdateLpaVerifyEEUser    update_verify_ee_user;

static KUpdateLpaBurnFlash   update_burn_flash;
static KUpdateLpaVerifyFlash update_verify_flash;

static KUpdateLpaExit update_exit;
//--------------------------------------------------
KUpdateLpaState *upd_idle = &update_idle;
//--------------------------------------------------
uint8_t KUpdateLpaState::f[2] = { 0xFF, 0xFF };
KUpdateLpaState *KUpdateLpaState::m_current_state = &update_idle;
cmd_status_t KUpdateLpaState::m_result = CMD_STATUS_FAILED;
//--------------------------------------------------
KUpdateLpaState::KUpdateLpaState() :/*{{{*/
	idle(&update_idle),
	enter(&update_enter),
	sig_check(&update_sig_check),
	dispatch(&update_dispatch),

	burn_lock(&update_burn_lock),
	burn_fuse(&update_burn_fuse),

	burn_ee_factory(&update_burn_ee_factory),
	burn_ee_user(&update_burn_ee_user),
	verify_ee_factory(&update_verify_ee_factory),
	verify_ee_user(&update_verify_ee_user),

	burn_flash(&update_burn_flash),
	verify_flash(&update_verify_flash),

	exit(&update_exit)
{ }/*}}}*/
//--------------------------------------------------
KUpdateLpaState::~KUpdateLpaState()
{ }
//--------------------------------------------------
cmd_status_t KUpdateLpaState::operator()(data_id_t data)/*{{{*/
{
if(m_current_state)
    { return m_current_state->functor(data); }

printf("UpdateLpa.failed.current state null.\n");
return CMD_STATUS_FAILED;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaState::result() const/*{{{*/
{
switch(m_result)
    {
    case CMD_STATUS_DONE:
	printf("UpdateLpa.ok.\n");
	break;

    case CMD_STATUS_FAILED:
	break;

    default:
	printf("UpdateLpa.failed.unknown result.\n");
	break;
    };

return m_result;
}/*}}}*/
//--------------------------------------------------
void KUpdateLpaState::set_result(cmd_status_t s)
{ m_result = s; }
//--------------------------------------------------
/* transitions here {{{*/
void KUpdateLpaState::to_idle()
{ m_current_state = idle; }
//--------------------------------------------------
void KUpdateLpaState::to_enter()
{ m_current_state = enter; }
//--------------------------------------------------
void KUpdateLpaState::to_sig_check()
{ m_current_state = sig_check; }
//--------------------------------------------------
void KUpdateLpaState::to_dispatch()
{ m_current_state = dispatch; }
//--------------------------------------------------
void KUpdateLpaState::to_burn_lock()
{ m_current_state = burn_lock; }
//--------------------------------------------------
void KUpdateLpaState::to_burn_fuse()
{ m_current_state = burn_fuse; }
//--------------------------------------------------
void KUpdateLpaState::to_burn_ee_factory()
{ m_current_state = burn_ee_factory; }
//--------------------------------------------------
void KUpdateLpaState::to_burn_ee_user()
{ m_current_state = burn_ee_user; }
//--------------------------------------------------
void KUpdateLpaState::to_verify_ee_factory()
{ m_current_state = verify_ee_factory; }
//--------------------------------------------------
void KUpdateLpaState::to_verify_ee_user()
{ m_current_state = verify_ee_user; }
//--------------------------------------------------
void KUpdateLpaState::to_burn_flash()
{ m_current_state = burn_flash; }
//--------------------------------------------------
void KUpdateLpaState::to_verify_flash()
{ m_current_state = verify_flash; }
//--------------------------------------------------
void KUpdateLpaState::to_exit()
{ m_current_state = exit; }/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaIdle::functor(data_id_t data)/*{{{*/
{
set_result(CMD_STATUS_FAILED);
isp_go_fast();
to_enter();
return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaEnter::functor(data_id_t)/*{{{*/
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
    isp_parse_error("UpdateLpa", val);
    to_exit();
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaSigCheck::functor(data_id_t)/*{{{*/
{
uint8_t val;
uint32_t sig;

sig = 0;
val = isp_read_signature_nb(&sig);

if(val == 0)
    { return CMD_STATUS_REPEAT; }
else if(val != 255)
    {
    printf("UpdateLpa.failed.unable to read signature.\n");
    to_exit();
    }
else if(sig == isp_atmega32u4_signature ||
	sig == isp_atmega16u4_signature)
    { to_dispatch(); }
else
    {
    if(isp_is_fast() == 1)
	{ isp_go_slow(); }
    else
	{
	printf("UpdateLpa.failed.invalid device signature %06X.\n", sig);
	to_exit();
	}
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaDispatch::functor(data_id_t d)/*{{{*/
{
switch(d)
    {
    case TM_151_DATA_LOCK:           to_burn_lock(); break;

    case TM_151_DATA_FUSE_WORK:
    case TM_151_DATA_FUSE_CAL:       to_burn_fuse(); break;

    case TM_151_DATA_EEPROM_FACTORY: to_burn_ee_factory(); break;
    case TM_151_DATA_EEPROM_USER:    to_burn_ee_user();    break;

    case TM_151_DATA_FLASH_WORK_FW:
    case TM_151_DATA_FLASH_CAL_FW:
    case TM_151_DATA_FLASH_BL:
    case TM_151_DATA_CALIBRATIONS:   to_burn_flash(); break;

    default:
	printf("UpdateLpa.failed.incorrect data id.\n");
	to_exit();
	break;
    };

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaBurnLock::functor(data_id_t)/*{{{*/
{
uint8_t val;
uint8_t lock;

val = 254;
if(firmware_get(TM_151_DATA_LOCK, &lock, 1, 0) == QSPI_OK)
    { val = isp_write_lock_nb(lock); }

if(val != 0)
    {
    if(val == 255)
	{ set_result(CMD_STATUS_DONE); }
    else
	{ printf("UpdateLpa.failed.burn lock error %d.\n", val); }

    to_exit();
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaBurnFuse::functor(data_id_t d)/*{{{*/
{
static enum {
    BURN_FUSE_LOW,
    BURN_FUSE_ENTER_HIGH,
    BURN_FUSE_HIGH,
    BURN_FUSE_ENTER_EXT,
    BURN_FUSE_EXT
} state = BURN_FUSE_LOW;

uint8_t val;
uint8_t f;

val = 254;
switch(state)
    {
    case BURN_FUSE_LOW:
	if(firmware_get(d, &f, 1, 0) == QSPI_OK)
	    { val = isp_write_low_fuse_nb(f); }

	if(val != 0)
	    {
	    if(val == 255)
		{
		isp_exit();
		state = BURN_FUSE_ENTER_HIGH;
		}
	    else
		{
		printf("UpdateLpa.failed.unable to write low fuse.\n");
		to_exit();
		}
	    }
	break;

    case BURN_FUSE_ENTER_HIGH:
	val = isp_enter_nb();
	if(val == 255)
	    { state = BURN_FUSE_HIGH; }
	else if(val != 0)
	    {
	    printf("UpdateLpa.failed.unable to enter isp.\n");
	    state = BURN_FUSE_LOW;
	    to_exit();
	    }
	break;

    case BURN_FUSE_HIGH:
	if(firmware_get(d, &f, 1, 1) == QSPI_OK)
	    { val = isp_write_high_fuse_nb(f); }

	if(val != 0)
	    {
	    if(val == 255)
		{
		isp_exit();
		state = BURN_FUSE_ENTER_EXT;
		}
	    else
		{
		printf("UpdateLpa.failed.unable to write high fuse.\n");
		state = BURN_FUSE_LOW;
		to_exit();
		}
	    }
	break;

    case BURN_FUSE_ENTER_EXT:
	val = isp_enter_nb();
	if(val == 255)
	    { state = BURN_FUSE_EXT; }
	else if(val != 0)
	    {
	    printf("UpdateLpa.failed.unable to enter isp.\n");
	    state = BURN_FUSE_LOW;
	    to_exit();
	    }
	break;

    case BURN_FUSE_EXT:
	if(firmware_get(d, &f, 1, 2) == QSPI_OK)
	    { val = isp_write_ext_fuse_nb(f); }

	if(val != 0)
	    {
	    if(val == 255)
		{ set_result(CMD_STATUS_DONE); }
	    else
		{
		printf("UpdateLpa.failed.unable to write extended fuse.\n");
		}

	    state = BURN_FUSE_LOW;
	    to_exit();
	    }
	break;
    };

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaBurnEEFactory::functor(data_id_t)/*{{{*/
{
static uint16_t addr = EEPROM_FACTORY_START;

uint8_t val;
uint8_t ee;

val = 254;
if(firmware_get(TM_151_DATA_EEPROM_FACTORY, &ee, 1, addr - EEPROM_FACTORY_START) == QSPI_OK)
    {
    if(ee == 0xFF)
	{ val = 255; } // skip eeprom erase value
    else
	{ val = isp_write_eeprom_nb(ee, addr); }
    }

if(val != 0)
    {
    if(val != 255)
	{
	printf("UpdateLpa.failed.burn eeprom returned %d at address %d.\n", val, addr);
	addr = EEPROM_FACTORY_START;
	to_exit();
	}
    else
	{
	addr++;
	if(addr == EEPROM_FACTORY_START + EEPROM_FACTORY_SIZE)
	    {
	    set_result(CMD_STATUS_DONE);
	    addr = EEPROM_FACTORY_START;
	    to_verify_ee_factory();
	    }
	}
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaBurnEEUser::functor(data_id_t)/*{{{*/
{
static uint16_t addr = EEPROM_USER_START;

uint8_t val;
uint8_t ee;

val = 254;
if(firmware_get(TM_151_DATA_EEPROM_USER, &ee, 1, addr - EEPROM_USER_START) == QSPI_OK)
    {
    if(ee == 0xFF)
	{ val = 255; } // skip eeprom erase value
    else
	{ val = isp_write_eeprom_nb(ee, addr); }
    }

if(val != 0)
    {
    if(val != 255)
	{
	printf("UpdateLpa.failed.burn eeprom returned %d at address %d\n", val, addr);
	addr = EEPROM_USER_START;
	to_exit();
	}
    else
	{
	addr++;
	if(addr == EEPROM_USER_START + EEPROM_USER_SIZE)
	    {
	    set_result(CMD_STATUS_DONE);
	    addr = EEPROM_USER_START;
	    to_verify_ee_user();
	    }
	}
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaVerifyEEFactory::functor(data_id_t)/*{{{*/
{
static uint16_t addr = EEPROM_FACTORY_START;

uint8_t val;
uint8_t ee;
uint8_t ee_lpa;

val = 254;
if(firmware_get(TM_151_DATA_EEPROM_FACTORY, &ee, 1, addr - EEPROM_FACTORY_START) == QSPI_OK)
    { val = isp_read_eeprom_nb(addr, &ee_lpa); }

if(val != 0)
    {
    if(val != 255)
	{
	printf("UpdateLpa.failed.read eeprom returned %d at address %d.\n", val, addr);
	addr = EEPROM_FACTORY_START;
	to_exit();
	}
    else
	{
	if(ee_lpa != ee)
	    {
	    printf("UpdateLpa.failed.verification error at address %d.\n", addr);
	    addr = EEPROM_FACTORY_START;
	    to_exit();
	    }
	else
	    {
	    addr++;
	    if(addr == EEPROM_FACTORY_START + EEPROM_FACTORY_SIZE)
		{
		set_result(CMD_STATUS_DONE);
		addr = EEPROM_FACTORY_START;
		to_exit();
		}
	    }
	}
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaVerifyEEUser::functor(data_id_t)/*{{{*/
{
static uint16_t addr = EEPROM_USER_START;

uint8_t val;
uint8_t ee;
uint8_t ee_lpa;

val = 254;
if(firmware_get(TM_151_DATA_EEPROM_USER, &ee, 1, addr - EEPROM_USER_START) == QSPI_OK)
    { val = isp_read_eeprom_nb(addr, &ee_lpa); }

if(val != 0)
    {
    if(val != 255)
	{
	printf("UpdateLpa.failed.read eeprom returned %d at address %d.\n", val, addr);
	addr = EEPROM_USER_START;
	to_exit();
	}
    else
	{
	if(ee_lpa != ee)
	    {
	    printf("UpdateLpa.failed.verification error at address %d.\n", addr);
	    addr = EEPROM_USER_START;
	    to_exit();
	    }
	else
	    {
	    addr++;
	    if(addr == EEPROM_USER_START + EEPROM_USER_SIZE)
		{
		set_result(CMD_STATUS_DONE);
		addr = EEPROM_USER_START;
		to_exit();
		}
	    }
	}
    }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaBurnFlash::functor(data_id_t d)/*{{{*/
{
static enum {
    BURN_FLASH_LEA,
    BURN_FLASH_READ_WORD,
    BURN_FLASH_PAGE_LOAD_LOW,
    BURN_FLASH_PAGE_LOAD_HIGH,
    BURN_FLASH_PAGE_DECISION,
    BURN_FLASH_PAGE_BURN,
    BURN_FLASH_PAGE_DECISION2
} state = BURN_FLASH_LEA;

static uint16_t addr;

uint8_t val;
uint16_t start;
uint16_t limit;

val = 254;
if(d == TM_151_DATA_FLASH_BL)
    {
    start = FLASH_BL_START;
    limit = FLASH_BL_SIZE;
    }
else if(d == TM_151_DATA_CALIBRATIONS)
    {
    start = FLASH_CALIBRATIONS_START;
    limit = FLASH_CALIBRATIONS_TOTAL_SIZE;
    }
else
    {
    start = FLASH_FW_START;
    limit = FLASH_FW_SIZE;
    }
switch(state)
    {
    case BURN_FLASH_LEA:
	val = isp_load_ext_addr_nb(0x0000);

	if(val != 0)
	    {
	    if(val == 255)
		{
		addr = start;
		state = BURN_FLASH_READ_WORD;
		}
	    else
		{
		printf("UpdateLpa.failed.load ext addr failed.\n");
		to_exit();
		}
	    }
	break;

    case BURN_FLASH_READ_WORD:
	if(firmware_get(d, f, 2, addr - start) == QSPI_OK)
	    {
	    if(f[0] == 0xFF && f[1] == 0xFF)
		{ state = BURN_FLASH_PAGE_DECISION; }
	    else
		{ state = BURN_FLASH_PAGE_LOAD_LOW; }
	    }
	else
	    {
	    printf("UpdateLpa.failed.error reading flash word at %04X.\n", addr - start);
	    state = BURN_FLASH_LEA;
	    to_exit();
	    }
	break;

    case BURN_FLASH_PAGE_LOAD_LOW:
	val = isp_load_flash_nb(f[0], addr/2, 0);

	if(val != 0)
	    {
	    if(val == 255)
		{ state = BURN_FLASH_PAGE_LOAD_HIGH; }
	    else
		{
		printf("UpdateLpa.failed.error loading low byte %d at addr %04X.\n", f, addr);
		state = BURN_FLASH_LEA;
		to_exit();
		}
	    }
	break;

    case BURN_FLASH_PAGE_LOAD_HIGH:
	val = isp_load_flash_nb(f[1], addr/2, 1);

	if(val != 0)
	    {
	    if(val == 255)
		{ state = BURN_FLASH_PAGE_DECISION; }
	    else
		{
		printf("UpdateLpa.failed.error loading high byte %d at addr %04X.\n", f, addr);
		state = BURN_FLASH_LEA;
		to_exit();
		}
	    }
	break;

    case BURN_FLASH_PAGE_DECISION:
	addr += 2;
	if(addr % FLASH_FW_PAGE_SIZE == 0)
	    { state = BURN_FLASH_PAGE_BURN; }
	else
	    { state = BURN_FLASH_READ_WORD; }
	break;

    case BURN_FLASH_PAGE_BURN:
	val = isp_burn_flash_nb((addr - FLASH_FW_PAGE_SIZE)/2); // burn to start of page, word address

	if(val != 0)
	    {
	    if(val == 255)
		{ state = BURN_FLASH_PAGE_DECISION2; }
	    else
		{
		printf("UpdateLpa.failed.unable to burn flash page at addr %04X.\n", addr);
		state = BURN_FLASH_LEA;
		to_exit();
		}
	    }
	break;

    case BURN_FLASH_PAGE_DECISION2:
	if(addr < start + limit)
	    { state = BURN_FLASH_READ_WORD; }
	else
	    {
	    set_result(CMD_STATUS_DONE);
	    state = BURN_FLASH_LEA;
	    to_verify_flash();
	    }
	break;
    };

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaVerifyFlash::functor(data_id_t d)/*{{{*/
{
static enum {
    VERIFY_FLASH_LEA,
    VERIFY_FLASH_READ_LOW,
    VERIFY_FLASH_READ_HIGH,
    VERIFY_FLASH_VERIFY,
} state = VERIFY_FLASH_LEA;

static uint16_t addr;

uint8_t val;
uint8_t f_good[2];
uint16_t start;
uint16_t limit;

val = 254;
if(d == TM_151_DATA_FLASH_BL)
    {
    start = FLASH_BL_START;
    limit = FLASH_BL_SIZE;
    }
else if(d == TM_151_DATA_CALIBRATIONS)
    {
    start = FLASH_CALIBRATIONS_START;
    limit = FLASH_CALIBRATIONS_TOTAL_SIZE;
    }
else
    {
    start = FLASH_FW_START;
    limit = FLASH_FW_SIZE;
    }
switch(state)
    {
    case VERIFY_FLASH_LEA:
	val = isp_load_ext_addr_nb(0x0000);

	if(val != 0)
	    {
	    if(val == 255)
		{
		addr = start;
		state = VERIFY_FLASH_READ_LOW;
		}
	    else
		{
		printf("UpdateLpa.failed.load ext addr failed.\n");
		to_exit();
		}
	    }
	break;

    case VERIFY_FLASH_READ_LOW:
	val = isp_read_flash_nb(addr/2, 0, &(f[0]));

	if(val != 0)
	    {
	    if(val == 255)
		{ state = VERIFY_FLASH_READ_HIGH; }
	    else
		{
		printf("UpdateLpa.failed.error reading flash word low byte at %04X.\n", addr - start);
		state = VERIFY_FLASH_LEA;
		to_exit();
		}
	    }
	break;

    case VERIFY_FLASH_READ_HIGH:
	val = isp_read_flash_nb(addr/2, 1, &(f[1]));

	if(val != 0)
	    {
	    if(val == 255)
		{ state = VERIFY_FLASH_VERIFY; }
	    else
		{
		printf("UpdateLpa.failed.error reading flash word high byte at %04X.\n", addr - start);
		state = VERIFY_FLASH_LEA;
		to_exit();
		}
	    }
	break;

    case VERIFY_FLASH_VERIFY:
	if(firmware_get(d, f_good, 2, addr - start) == QSPI_OK)
	    {
	    if(f[0] == f_good[0] && f[1] == f_good[1])
		{
		addr += 2;
		if(addr < start + limit)
		    { state = VERIFY_FLASH_READ_LOW; }
		else
		    {
		    set_result(CMD_STATUS_DONE);
		    state = VERIFY_FLASH_LEA;
		    to_exit();
		    }
		}
	    else
		{
		printf("UpdateLpa.failed.verification failed at address %04X.\n", addr);
		state = VERIFY_FLASH_LEA;
		to_exit();
		}
	    }
	else
	    {
	    printf("UpdateLpa.failed.error reading flash word at %04X.\n", addr - start);
	    state = VERIFY_FLASH_LEA;
	    to_exit();
	    }
	break;
    };

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
cmd_status_t KUpdateLpaExit::functor(data_id_t)/*{{{*/
{
isp_exit();
to_idle();
return result();
}/*}}}*/
//--------------------------------------------------
