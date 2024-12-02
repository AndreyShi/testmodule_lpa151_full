//--------------------------------------------------
// N25Q256A quad spi flash memory low-level i-face
//--------------------------------------------------
#include <string.h>

#include "qspi_flash.h"
#include "common.h"
#include "app_export.h"
//--------------------------------------------------
static qspi_flash_state_t flash_state;
static uint8_t flash_status;

static uint32_t flash_addr;
static uint8_t *flash_buff;
static uint32_t flash_size;

static uint32_t flash_clock;
static uint32_t flash_timeout;

//--------------------------------------------------
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *qspi, uint32_t Timeout);
static uint8_t QSPI_AutoPollingMemReady_IT(QSPI_HandleTypeDef *qspi);
static uint8_t QSPI_AutoPollingWriteEnabled(QSPI_HandleTypeDef *qspi, uint32_t Timeout);
static uint8_t QSPI_AutoPollingWriteEnabled_IT(QSPI_HandleTypeDef *qspi);

static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *qspi);

static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *qspi);
static uint8_t QSPI_WriteEnable_IT(QSPI_HandleTypeDef *qspi);

static uint8_t QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *qspi);

static uint8_t Abort(void);
static void    Reset_clock(const uint32_t to);
static uint8_t Check_clock(void);

//--------------------------------------------------
void QSPI_Flash_Init()/*{{{*/
{
flash_state = QSPI_FLASH_IDLE;
flash_status = QSPI_OK;
flash_buff = 0;
flash_size = 0;

Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
if(QSPI_ResetMemory(&hqspi) != QSPI_OK)
    {
    flash_status = QSPI_ERROR;
    return;
    }

if(QSPI_DummyCyclesCfg(&hqspi) != QSPI_OK)
    {
    flash_status = QSPI_ERROR;
    return;
    }
}/*}}}*/
//--------------------------------------------------
void QSPI_Flash_Task(void)/*{{{*/
{
QSPI_CommandTypeDef cmd;

switch(flash_state)
    {
    default:
	break;

    case QSPI_FLASH_PAGE_SEND_CMD:
	cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction       = QUAD_IN_FAST_PROG_CMD;
	cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
	cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode          = QSPI_DATA_4_LINES;
	cmd.DummyCycles       = 0;
	cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	cmd.Address = flash_addr;
	cmd.NbData  = N25Q128A_PAGE_SIZE;

	if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	    Abort();
	    break;
	    }

	flash_state  = QSPI_FLASH_PAGE_DATA_SENT;
	flash_status = QSPI_BUSY;
	if(HAL_QSPI_Transmit_IT(&hqspi, flash_buff) == HAL_OK)
	    { Reset_clock(N25Q128A_PAGE_WRITE_MAX_TIME); }
	else
	    { Abort(); }
	break;

    case QSPI_FLASH_LAST_PAGE_SEND_CMD:
	cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction       = QUAD_IN_FAST_PROG_CMD;
	cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
	cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode          = QSPI_DATA_4_LINES;
	cmd.DummyCycles       = 0;
	cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	cmd.Address = flash_addr;
	cmd.NbData  = flash_size;

	if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	    Abort();
	    break;
	    }

	flash_state  = QSPI_FLASH_LAST_PAGE_DATA_SENT;
	flash_status = QSPI_BUSY;
	if(HAL_QSPI_Transmit_IT(&hqspi, flash_buff) == HAL_OK)
	    { Reset_clock(N25Q128A_PAGE_WRITE_MAX_TIME); }
	else
	    { Abort(); }
	break;
    };

if(flash_state != QSPI_FLASH_IDLE && Check_clock() == 1)
    { Abort(); }
}/*}}}*/
//--------------------------------------------------
// non-blocking interface
//--------------------------------------------------
qspi_flash_state_t QSPI_Flash_State(void)
{ return flash_state; }
//--------------------------------------------------
uint8_t QSPI_Flash_Status(void)
{ return flash_status; }
//--------------------------------------------------
/* TODO: make reads in chunks of 64k - 1, qspi size register is only 16bits long */
uint8_t QSPI_Read_IT(uint32_t addr, uint8_t *buff, uint32_t sz)/*{{{*/
{
QSPI_CommandTypeDef cmd;

if(flash_state != QSPI_FLASH_IDLE)
    { return QSPI_BUSY; }

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = QUAD_OUT_FAST_READ_CMD;
cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_4_LINES;
cmd.DummyCycles       = N25Q128A_DUMMY_CYCLES_READ_QUAD;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

cmd.Address = addr;
cmd.NbData  = sz;

flash_buff = buff;
flash_size = sz;
if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return Abort(); }

flash_state = QSPI_FLASH_READ_STARTED;
flash_status = QSPI_BUSY;
if(HAL_QSPI_Receive_IT(&hqspi, flash_buff) == HAL_OK)
    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
else
    { Abort(); }
return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
uint8_t QSPI_EraseSector_IT(uint32_t addr)/*{{{*/
{
if(flash_state != QSPI_FLASH_IDLE)
    { return QSPI_BUSY; }

flash_addr = addr;

flash_state = QSPI_FLASH_ERASE_WRITE_ENABLE_SENT;
flash_status = QSPI_BUSY;
if(QSPI_WriteEnable_IT(&hqspi) != QSPI_OK)
    { return Abort(); }

Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
uint8_t QSPI_Write_IT(uint32_t addr, uint8_t *buff, uint32_t sz)/*{{{*/
{
if(flash_state != QSPI_FLASH_IDLE)
    { return QSPI_BUSY; }

if(sz >= N25Q128A_PAGE_SIZE)
    {
    flash_state = QSPI_FLASH_PAGE_WRITE_ENABLE_SENT;
    flash_status = QSPI_BUSY;
    }
else
    {
    flash_state = QSPI_FLASH_LAST_PAGE_WRITE_ENABLE_SENT;
    flash_status = QSPI_BUSY;
    }

flash_addr = addr;
flash_buff = buff;
flash_size = sz;
if(QSPI_WriteEnable_IT(&hqspi) != QSPI_OK)
    { return Abort(); }

Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
// blocking interface
//--------------------------------------------------
uint8_t QSPI_Read(uint32_t addr, uint8_t *buff, uint32_t sz)/*{{{*/
{
if(flash_state != QSPI_FLASH_IDLE)
    { return QSPI_BUSY; }

uint16_t parts;
QSPI_CommandTypeDef cmd;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = QUAD_OUT_FAST_READ_CMD;
cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_4_LINES;
cmd.DummyCycles       = N25Q128A_DUMMY_CYCLES_READ_QUAD;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

/* workaround, qspi size register is 16bit only */
parts = sz/0xFFFF;
for(uint16_t i=0; i<parts; i++)
    {
    cmd.Address = addr;
    cmd.NbData  = 0xFFFF;

    if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{ return QSPI_ERROR; }

    if(HAL_QSPI_Receive(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{ return QSPI_ERROR; }

    addr += 0xFFFF;
    buff += 0xFFFF;
    sz   -= 0xFFFF;
    }

if(sz > 0)
    {
    cmd.Address = addr;
    cmd.NbData  = sz;

    if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{ return QSPI_ERROR; }

    if(HAL_QSPI_Receive(&hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{ return QSPI_ERROR; }
    }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
uint8_t QSPI_EraseSector(uint32_t addr)/*{{{*/
{
if(flash_state != QSPI_FLASH_IDLE)
    { return QSPI_BUSY; }

if(QSPI_WriteEnable(&hqspi) != QSPI_OK)
    { return QSPI_ERROR; }

if(QSPI_AutoPollingWriteEnabled(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    { return QSPI_ERROR; }

QSPI_CommandTypeDef     cmd;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = SECTOR_ERASE_CMD;
cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
cmd.Address           = addr;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_NONE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

if(HAL_QSPI_Command(&hqspi, &cmd, N25Q128A_SECTOR_ERASE_MAX_TIME) != HAL_OK)
    { return QSPI_ERROR; }

if(QSPI_AutoPollingMemReady(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
uint8_t QSPI_Write(uint32_t addr, uint8_t *buff, uint32_t sz)/*{{{*/
{
if(flash_state != QSPI_FLASH_IDLE)
    { return QSPI_BUSY; }

uint16_t pages;
QSPI_CommandTypeDef cmd;

pages = sz/N25Q128A_PAGE_SIZE;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = QUAD_IN_FAST_PROG_CMD;
cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_4_LINES;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
for(uint16_t i=0; i<pages; i++)
    {
    if(QSPI_WriteEnable(&hqspi) != QSPI_OK)
	{ return QSPI_ERROR; }

    if(QSPI_AutoPollingWriteEnabled(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{ return QSPI_ERROR; }

    cmd.Address = addr;
    cmd.NbData  = N25Q128A_PAGE_SIZE;

    if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{ return QSPI_ERROR; }

    if(HAL_QSPI_Transmit(&hqspi, buff, N25Q128A_PAGE_WRITE_MAX_TIME) != HAL_OK)
	{ return QSPI_ERROR; }

    if(QSPI_AutoPollingMemReady(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{ return QSPI_ERROR; }

    addr += N25Q128A_PAGE_SIZE;
    buff += N25Q128A_PAGE_SIZE;
    sz   -= N25Q128A_PAGE_SIZE;
    }

if(sz == 0)
    { return QSPI_OK; }

cmd.NbData  = sz;

if(QSPI_WriteEnable(&hqspi) != QSPI_OK)
    { return QSPI_ERROR; }

if(QSPI_AutoPollingWriteEnabled(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    { return QSPI_ERROR; }

cmd.Address = addr;
cmd.NbData  = N25Q128A_PAGE_SIZE;

if(HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

if(HAL_QSPI_Transmit(&hqspi, buff, N25Q128A_PAGE_WRITE_MAX_TIME) != HAL_OK)
    { return QSPI_ERROR; }

if(QSPI_AutoPollingMemReady(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    { return QSPI_ERROR; }
return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
// static functions
//--------------------------------------------------
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *qspi, uint32_t timeout)/*{{{*/
{
QSPI_CommandTypeDef     cmd;
QSPI_AutoPollingTypeDef cfg;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = READ_STATUS_REG_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_1_LINE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

cfg.Match           = 0;
cfg.Mask            = N25Q128A_SR_WIP;
cfg.MatchMode       = QSPI_MATCH_MODE_AND;
cfg.StatusBytesSize = 1;
cfg.Interval        = 0x10;
cfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

if(HAL_QSPI_AutoPolling(qspi, &cmd, &cfg, timeout) != HAL_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t QSPI_AutoPollingMemReady_IT(QSPI_HandleTypeDef *qspi)/*{{{*/
{
QSPI_CommandTypeDef     cmd;
QSPI_AutoPollingTypeDef cfg;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = READ_STATUS_REG_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_1_LINE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

cfg.Match           = 0;
cfg.Mask            = N25Q128A_SR_WIP;
cfg.MatchMode       = QSPI_MATCH_MODE_AND;
cfg.StatusBytesSize = 1;
cfg.Interval        = 0x10;
cfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

if(HAL_QSPI_AutoPolling_IT(qspi, &cmd, &cfg) != HAL_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t QSPI_AutoPollingWriteEnabled(QSPI_HandleTypeDef *qspi, uint32_t timeout)/*{{{*/
{
QSPI_CommandTypeDef     cmd;
QSPI_AutoPollingTypeDef cfg;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = READ_STATUS_REG_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_1_LINE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

cfg.Match           = N25Q128A_SR_WREN;
cfg.Mask            = N25Q128A_SR_WREN;
cfg.MatchMode       = QSPI_MATCH_MODE_AND;
cfg.StatusBytesSize = 1;
cfg.Interval        = 0x10;
cfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

if(HAL_QSPI_AutoPolling(qspi, &cmd, &cfg, timeout) != HAL_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t QSPI_AutoPollingWriteEnabled_IT(QSPI_HandleTypeDef *qspi)/*{{{*/
{
QSPI_CommandTypeDef     cmd;
QSPI_AutoPollingTypeDef cfg;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = READ_STATUS_REG_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_1_LINE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

cfg.Match           = N25Q128A_SR_WREN;
cfg.Mask            = N25Q128A_SR_WREN;
cfg.MatchMode       = QSPI_MATCH_MODE_AND;
cfg.StatusBytesSize = 1;
cfg.Interval        = 0x10;
cfg.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

if(HAL_QSPI_AutoPolling_IT(qspi, &cmd, &cfg) != HAL_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *qspi)/*{{{*/
{
QSPI_CommandTypeDef cmd;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = RESET_ENABLE_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_NONE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

if(HAL_QSPI_Command(qspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

cmd.Instruction = RESET_MEMORY_CMD;
if(HAL_QSPI_Command(qspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

if(QSPI_AutoPollingMemReady(qspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    { 
		_Error_Handler_new(TM_151_LED_BLUE);
		return QSPI_ERROR; 
	}

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *qspi)/*{{{*/
{
QSPI_CommandTypeDef     cmd;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = WRITE_ENABLE_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_NONE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

if(HAL_QSPI_Command(qspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t QSPI_WriteEnable_IT(QSPI_HandleTypeDef *qspi)/*{{{*/
{
QSPI_CommandTypeDef     cmd;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = WRITE_ENABLE_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_NONE;
cmd.DummyCycles       = 0;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

if(HAL_QSPI_Command_IT(qspi, &cmd) != HAL_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *qspi)/*{{{*/
{
uint8_t reg;
QSPI_CommandTypeDef cmd;

cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
cmd.Instruction       = READ_VOL_CFG_REG_CMD;
cmd.AddressMode       = QSPI_ADDRESS_NONE;
cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
cmd.DataMode          = QSPI_DATA_1_LINE;
cmd.DummyCycles       = 0;
cmd.NbData            = 1;
cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

if(HAL_QSPI_Command(qspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

if(HAL_QSPI_Receive(qspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

if(QSPI_WriteEnable(qspi) != QSPI_OK)
    { return QSPI_ERROR; }

if(QSPI_AutoPollingWriteEnabled(qspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    { return QSPI_ERROR; }

cmd.Instruction = WRITE_VOL_CFG_REG_CMD;
MODIFY_REG(reg, N25Q128A_VCR_NB_DUMMY, (N25Q128A_DUMMY_CYCLES_READ_QUAD << POSITION_VAL(N25Q128A_VCR_NB_DUMMY)));

if(HAL_QSPI_Command(qspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

if(HAL_QSPI_Transmit(qspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    { return QSPI_ERROR; }

return QSPI_OK;
}/*}}}*/
//--------------------------------------------------
static uint8_t Abort(void)/*{{{*/
{
flash_state = QSPI_FLASH_IDLE;
flash_status = QSPI_ERROR;
return QSPI_ERROR;
}/*}}}*/
//--------------------------------------------------
static void Reset_clock(const uint32_t to)/*{{{*/
{
flash_clock = HAL_GetTick();
flash_timeout = to;
}/*}}}*/
//--------------------------------------------------
static uint8_t Check_clock(void)/*{{{*/
{
if(HAL_GetTick() - flash_clock >= flash_timeout)
    { return 1; }

return 0;
}/*}}}*/
//--------------------------------------------------
// interrupt callbacks
//--------------------------------------------------
void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *qspi)/*{{{*/
{
if(qspi->Instance != QUADSPI)
    { return; }

switch(flash_state)
    {
    case QSPI_FLASH_ERASE_WRITE_ENABLE_SENT:
	flash_state = QSPI_FLASH_ERASE_WRITE_ENABLED;
	if(QSPI_AutoPollingWriteEnabled_IT(qspi) == QSPI_OK)
	    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
	else
	    { Abort(); }
	break;

    case QSPI_FLASH_ERASE_SENT:
	flash_state  = QSPI_FLASH_ERASED;
	flash_status = QSPI_BUSY;
	if(QSPI_AutoPollingMemReady_IT(&hqspi) == QSPI_OK)
	    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
	else
	    { Abort(); }
	break;

    case QSPI_FLASH_PAGE_WRITE_ENABLE_SENT:
	flash_state = QSPI_FLASH_PAGE_WRITE_ENABLED;
	if(QSPI_AutoPollingWriteEnabled_IT(qspi) == QSPI_OK)
	    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
	else
	    { Abort(); }
	break;

    case QSPI_FLASH_LAST_PAGE_WRITE_ENABLE_SENT:
	flash_state = QSPI_FLASH_LAST_PAGE_WRITE_ENABLED;
	if(QSPI_AutoPollingWriteEnabled_IT(qspi) == QSPI_OK)
	    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
	else
	    { Abort(); }
	break;

    };
}/*}}}*/
//--------------------------------------------------
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *qspi)/*{{{*/
{
if(qspi->Instance != QUADSPI)
    { return; }

if(flash_state != QSPI_FLASH_READ_STARTED)
    { return; }

flash_status = QSPI_OK;
flash_state = QSPI_FLASH_IDLE;
}/*}}}*/
//--------------------------------------------------
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *qspi)/*{{{*/
{
if(qspi->Instance != QUADSPI)
    { return; }

switch(flash_state)
    {
    default:
	break;

    case QSPI_FLASH_PAGE_DATA_SENT:
	flash_state  = QSPI_FLASH_PAGE_WRITTEN;
	flash_status = QSPI_BUSY;
	if(QSPI_AutoPollingMemReady_IT(&hqspi) == QSPI_OK)
	    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
	else
	    { Abort(); }
	break;

    case QSPI_FLASH_LAST_PAGE_DATA_SENT:
	flash_state  = QSPI_FLASH_LAST_PAGE_WRITTEN;
	flash_status = QSPI_BUSY;
	if(QSPI_AutoPollingMemReady_IT(&hqspi) == QSPI_OK)
	    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
	else
	    { Abort(); }
	break;
    };
}/*}}}*/
//--------------------------------------------------
void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *qspi)/*{{{*/
{
if(qspi->Instance != QUADSPI)
    { return; }

QSPI_CommandTypeDef cmd;
switch(flash_state)
    {
    default:
	break;

    case QSPI_FLASH_ERASE_WRITE_ENABLED:
	cmd.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction       = SECTOR_ERASE_CMD;
	cmd.AddressMode       = QSPI_ADDRESS_1_LINE;
	cmd.AddressSize       = QSPI_ADDRESS_24_BITS;
	cmd.Address           = flash_addr;
	cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode          = QSPI_DATA_NONE;
	cmd.DummyCycles       = 0;
	cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	flash_state  = QSPI_FLASH_ERASE_SENT;
	flash_status = QSPI_BUSY;
	if(HAL_QSPI_Command_IT(&hqspi, &cmd) == HAL_OK)
	    { Reset_clock(N25Q128A_SECTOR_ERASE_MAX_TIME); }
	else
	    { Abort(); }
	break;

    case QSPI_FLASH_ERASED:
    case QSPI_FLASH_LAST_PAGE_WRITTEN:
	flash_state  = QSPI_FLASH_IDLE;
	flash_status = QSPI_OK;
	break;

    case QSPI_FLASH_PAGE_WRITE_ENABLED:
	flash_state = QSPI_FLASH_PAGE_SEND_CMD;
	break;

    case QSPI_FLASH_PAGE_WRITTEN:
	flash_addr += N25Q128A_PAGE_SIZE;
	flash_buff += N25Q128A_PAGE_SIZE;
	flash_size -= N25Q128A_PAGE_SIZE;

	if(flash_size >= N25Q128A_PAGE_SIZE)
	    { flash_state  = QSPI_FLASH_PAGE_WRITE_ENABLE_SENT; }
	else if(flash_size > 0)
	    { flash_state = QSPI_FLASH_LAST_PAGE_WRITE_ENABLE_SENT; }
	else
	    {
	    flash_state = QSPI_FLASH_IDLE;
	    flash_status = QSPI_OK;
	    }

	if(QSPI_WriteEnable_IT(&hqspi) == QSPI_OK)
	    { Reset_clock(HAL_QPSI_TIMEOUT_DEFAULT_VALUE); }
	else
	    { Abort(); }
	break;

    case QSPI_FLASH_LAST_PAGE_WRITE_ENABLED:
	flash_state = QSPI_FLASH_LAST_PAGE_SEND_CMD;
	break;
    };
}/*}}}*/
//--------------------------------------------------
