//--------------------------------------------------
// Платформо-зависимые функции для работы с isp
//--------------------------------------------------
#include "stm32f7xx_hal.h"

#include "app_config.h"
#include "app_export.h"
#include "gpio_if.h"
#include "isp_if_private.h"
#include "relay_if.h"
#include "spi.h"

//--------------------------------------------------
static uint32_t clock;

static isp_enter_state_t           isp_enter_state           = ISP_ENTER_START;
static isp_chiperase_state_t       isp_chiperase_state       = ISP_CHIPERASE_START;
static isp_lea_state_t             isp_lea_state             = ISP_LEA_START;
static isp_read_flash_state_t      isp_read_flash_state      = ISP_READ_FLASH_START;
static isp_load_flash_state_t      isp_load_flash_state      = ISP_LOAD_FLASH_START;
static isp_burn_flash_state_t      isp_burn_flash_state      = ISP_BURN_FLASH_START;
static isp_read_eeprom_state_t     isp_read_eeprom_state     = ISP_READ_EE_START;
static isp_write_eeprom_state_t    isp_write_eeprom_state    = ISP_WRITE_EE_START;
static isp_read_lock_state_t       isp_read_lock_state       = ISP_READ_LOCK_START;
static isp_write_lock_state_t      isp_write_lock_state      = ISP_WRITE_LOCK_START;
static isp_read_signature_state_t  isp_read_signature_state  = ISP_READ_SIG_START;
static isp_read_low_fuse_state_t   isp_read_low_fuse_state   = ISP_READ_LOW_FUSE_START;
static isp_read_high_fuse_state_t  isp_read_high_fuse_state  = ISP_READ_HIGH_FUSE_START;
static isp_read_ext_fuse_state_t   isp_read_ext_fuse_state   = ISP_READ_EXT_FUSE_START;
static isp_write_low_fuse_state_t  isp_write_low_fuse_state  = ISP_WRITE_LOW_FUSE_START;
static isp_write_high_fuse_state_t isp_write_high_fuse_state = ISP_WRITE_HIGH_FUSE_START;
static isp_write_ext_fuse_state_t  isp_write_ext_fuse_state  = ISP_WRITE_EXT_FUSE_START;
//--------------------------------------------------
// go the non-blocking way
// just repeat the same function till it returns
// non-zero result
//--------------------------------------------------
uint8_t isp_enter_nb(void)/*{{{*/
{
switch(isp_enter_state)
    {
    case ISP_ENTER_START:
	if(isp_check_lock() != 0)
	    { return 1; }

	LPA_ISP_RESET;
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_WAIT_RESET;
	break;

    case ISP_ENTER_WAIT_RESET:
	if(HAL_GetTick() - clock < 100)
	    { break; }

	isp_enter_state = ISP_ENTER_DETECT;
	break;

    case ISP_ENTER_DETECT:
	if(LPA_ISP_DETECT == GPIO_PIN_RESET)
	    {
	    isp_enter_state = ISP_ENTER_RESET2;
	    break;
	    }

	relay_set(TM_151_RELAY_LPA_POWER, CH_1, STATE_ON);
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_WAIT_REDETECT;
	break;

    case ISP_ENTER_RESET2:
	LPA_ISP_UNRESET;
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_WAIT_RESET2;
	break;

    case ISP_ENTER_WAIT_RESET2:
	if(HAL_GetTick() - clock < 10)
	    { break; }

	LPA_ISP_RESET;
	LPA_ISP_ENABLE;
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_RESET2_WAIT;
	break;

    case ISP_ENTER_RESET2_WAIT:
	if(HAL_GetTick() - clock < 21)
	    { break; }

	isp_tx_buff[0] = 0xAC;
	isp_tx_buff[1] = 0x53;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_WAIT_XMIT;
	break;

    case ISP_ENTER_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_enter_state = ISP_ENTER_START;
	    return isp_error(2);
	    }
	break; // wait for interrupt

    case ISP_ENTER_XMIT_DONE:
	isp_enter_state = ISP_ENTER_START;

	if(isp_rx_buff[2] != 0x53)
	    {
	    isp_enter_state = ISP_ENTER_RESET3;
	    break;
	    }

	isp_lock = 0;
	return 255; // done!

    case ISP_ENTER_WAIT_REDETECT:
	if(HAL_GetTick() - clock < 100)
	    { break; }

	if(LPA_ISP_DETECT == GPIO_PIN_SET)
	    { 
	    relay_set(TM_151_RELAY_LPA_POWER, CH_1, STATE_OFF);
	    isp_enter_state = ISP_ENTER_START;
	    return isp_error(4);
	    }

	isp_enter_state = ISP_ENTER_RESET2;
	break;

    case ISP_ENTER_RESET3:
	LPA_ISP_UNRESET;
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_WAIT_RESET3;
	break;

    case ISP_ENTER_WAIT_RESET3:
	if(HAL_GetTick() - clock < 10)
	    { break; }

	LPA_ISP_RESET;
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_GO_LOW;
	break;

    case ISP_ENTER_GO_LOW:
	isp_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	if(HAL_SPI_Init(&isp_spi) != HAL_OK)
	    {
	    isp_enter_state = ISP_ENTER_START;
	    return isp_error(5);
	    }

	isp_enter_state = ISP_ENTER_RESET3_WAIT;
	break;

    case ISP_ENTER_RESET3_WAIT:
	if(HAL_GetTick() - clock < 21)
	    { break; }

	isp_tx_buff[0] = 0xAC;
	isp_tx_buff[1] = 0x53;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_enter_state = ISP_ENTER_WAIT_XMIT_LOW;
	break;

    case ISP_ENTER_WAIT_XMIT_LOW:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	    if(HAL_SPI_Init(&isp_spi) != HAL_OK)
		{
		isp_enter_state = ISP_ENTER_START;
		return isp_error(6);
		}

	    isp_enter_state = ISP_ENTER_START;
	    return isp_error(7);
	    }
	break; // wait for interrupt

    case ISP_ENTER_XMIT_DONE_LOW:
	isp_enter_state = ISP_ENTER_START;

	if(isp_rx_buff[2] != 0x53)
	    {
	    isp_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	    if(HAL_SPI_Init(&isp_spi) != HAL_OK)
		{ return isp_error(6); }

	    return isp_error(8);
	    }

	isp_lock = 0;
	return 255; // done!
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_chip_erase_nb(void)/*{{{*/
{
switch(isp_chiperase_state)
    {
    case ISP_CHIPERASE_START:
	isp_tx_buff[0] = 0xAC;
	isp_tx_buff[1] = 0x80;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_chiperase_state = ISP_CHIPERASE_WAIT_XMIT;
	break;

    case ISP_CHIPERASE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_chiperase_state = ISP_CHIPERASE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_CHIPERASE_XMIT_DONE:
	clock = HAL_GetTick();
	isp_chiperase_state = ISP_CHIPERASE_WAIT_ERASE;
	break;

    case ISP_CHIPERASE_WAIT_ERASE:
	if(HAL_GetTick() - clock < 10) // at least 9.0ms as of datasheet
	    { break; }

	isp_chiperase_state = ISP_CHIPERASE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_load_ext_addr_nb(uint8_t ext_addr)/*{{{*/
{
switch(isp_lea_state)
    {
    case ISP_LEA_START:
	isp_tx_buff[0] = 0x4D;
	isp_tx_buff[1] = 0x00;
	isp_tx_buff[2] = ext_addr;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_lea_state = ISP_LEA_WAIT_XMIT;
	break;

    case ISP_LEA_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_lea_state = ISP_LEA_START;
	    return isp_error(2);
	    }
	break;

    case ISP_LEA_XMIT_DONE:
	isp_lea_state = ISP_LEA_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_flash_nb(uint16_t addr, uint8_t h, uint8_t *out)/*{{{*/
{
switch(isp_read_flash_state)
    {
    case ISP_READ_FLASH_START:
	isp_tx_buff[0] = 0x20 | (h == 0 ? 0x00 : 0x08);
	isp_tx_buff[1] = (addr >> 8) & 0x00FF;
	isp_tx_buff[2] = (addr     ) & 0x00FF;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_flash_state = ISP_READ_FLASH_WAIT_XMIT;
	break;

    case ISP_READ_FLASH_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_flash_state = ISP_READ_FLASH_START;
	    return isp_error(2);
	    }
	break;

    case ISP_READ_FLASH_XMIT_DONE:
	*out = isp_rx_buff[3];
	isp_read_flash_state = ISP_READ_FLASH_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_load_flash_nb(uint8_t val, uint8_t addr, uint8_t h)/*{{{*/
{
switch(isp_load_flash_state)
    {
    case ISP_LOAD_FLASH_START:
	isp_tx_buff[0] = 0x40 | (h == 0 ? 0x00 : 0x08);
	isp_tx_buff[1] = 0x00;
	isp_tx_buff[2] = addr & 0x3F;
	isp_tx_buff[3] = val;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_load_flash_state = ISP_LOAD_FLASH_WAIT_XMIT;
	break;

    case ISP_LOAD_FLASH_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_load_flash_state = ISP_LOAD_FLASH_START;
	    return isp_error(2);
	    }
	break;

    case ISP_LOAD_FLASH_XMIT_DONE:
	isp_load_flash_state = ISP_LOAD_FLASH_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_burn_flash_nb(uint16_t addr)/*{{{*/
{
switch(isp_burn_flash_state)
    {
    case ISP_BURN_FLASH_START:
	isp_tx_buff[0] = 0x4C;
	isp_tx_buff[1] = (addr >> 8) & 0x00FF;
	isp_tx_buff[2] = (addr     ) & 0x00C0;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_burn_flash_state = ISP_BURN_FLASH_WAIT_XMIT;
	break;

    case ISP_BURN_FLASH_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_burn_flash_state = ISP_BURN_FLASH_START;
	    return isp_error(2);
	    }
	break;

    case ISP_BURN_FLASH_XMIT_DONE:
	clock = HAL_GetTick();
	isp_burn_flash_state = ISP_BURN_FLASH_WAIT_BURN;
	break;

    case ISP_BURN_FLASH_WAIT_BURN:
	if(HAL_GetTick() - clock < 6) // at least 4.5ms as of datasheet
	    { break; }

	isp_burn_flash_state = ISP_BURN_FLASH_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_eeprom_nb(uint16_t addr, uint8_t *out)/*{{{*/
{
switch(isp_read_eeprom_state)
    {
    case ISP_READ_EE_START:
	isp_tx_buff[0] = 0xA0;
	isp_tx_buff[1] = (addr >> 8) & 0x000F;
	isp_tx_buff[2] = (addr     ) & 0x00FF;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_eeprom_state = ISP_READ_EE_WAIT_XMIT;
	break;

    case ISP_READ_EE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_eeprom_state = ISP_READ_EE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_READ_EE_XMIT_DONE:
	*out = isp_rx_buff[3];
	isp_read_eeprom_state = ISP_READ_EE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_eeprom_nb(uint8_t val, uint16_t addr)/*{{{*/
{
switch(isp_write_eeprom_state)
    {
    case ISP_WRITE_EE_START:
	isp_tx_buff[0] = 0xC0;
	isp_tx_buff[1] = (addr >> 8) & 0x000F;
	isp_tx_buff[2] = (addr     ) & 0x00FF;
	isp_tx_buff[3] = val;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_write_eeprom_state = ISP_WRITE_EE_WAIT_XMIT;
	break;

    case ISP_WRITE_EE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_write_eeprom_state = ISP_WRITE_EE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_WRITE_EE_XMIT_DONE:
	clock = HAL_GetTick();
	isp_write_eeprom_state = ISP_WRITE_EE_WAIT_WRITE;
	break;

    case ISP_WRITE_EE_WAIT_WRITE:
	if(HAL_GetTick() - clock < 10) // at least 9ms as of datasheet
	    { break; }

	isp_write_eeprom_state = ISP_WRITE_EE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_lock_nb(uint8_t *out)/*{{{*/
{
switch(isp_read_lock_state)
    {
    case ISP_READ_LOCK_START:
	isp_tx_buff[0] = 0x58;
	isp_tx_buff[1] = 0x00;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_lock_state = ISP_READ_LOCK_WAIT_XMIT;
	break;

    case ISP_READ_LOCK_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_lock_state = ISP_READ_LOCK_START;
	    return isp_error(2);
	    }
	break;

    case ISP_READ_LOCK_XMIT_DONE:
	*out = isp_rx_buff[3];
	isp_read_lock_state = ISP_READ_LOCK_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_lock_nb(uint8_t lock)/*{{{*/
{
switch(isp_write_lock_state)
    {
    case ISP_WRITE_LOCK_START:
	isp_tx_buff[0] = 0xAC;
	isp_tx_buff[1] = 0xE0;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0xC0 | lock;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_write_lock_state = ISP_WRITE_LOCK_WAIT_XMIT;
	break;

    case ISP_WRITE_LOCK_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_write_lock_state = ISP_WRITE_LOCK_START;
	    return isp_error(2);
	    }
	break;

    case ISP_WRITE_LOCK_XMIT_DONE:
	isp_write_lock_state = ISP_WRITE_LOCK_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_signature_nb(uint32_t *out)/*{{{*/
{
static uint32_t sig;

switch(isp_read_signature_state)
    {
    case ISP_READ_SIG_START:
	sig = 0;

	isp_tx_buff[0] = 0x30;
	isp_tx_buff[1] = 0x00;
	isp_tx_buff[2] = 0x02;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_signature_state = ISP_READ_SIG_WAIT_XMIT_1;
	break;

    case ISP_READ_SIG_WAIT_XMIT_1:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_signature_state = ISP_READ_SIG_START;
	    return isp_error(2);
	    }
	break;

    case ISP_READ_SIG_XMIT_1_DONE:
	sig = isp_rx_buff[3];
	isp_read_signature_state = ISP_READ_SIG_START_2;
	break;

    case ISP_READ_SIG_START_2:
	isp_tx_buff[0] = 0x30;
	isp_tx_buff[1] = 0x00;
	isp_tx_buff[2] = 0x01;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_signature_state = ISP_READ_SIG_WAIT_XMIT_2;
	break;

    case ISP_READ_SIG_WAIT_XMIT_2:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_signature_state = ISP_READ_SIG_START;
	    return isp_error(3);
	    }
	break;

    case ISP_READ_SIG_XMIT_2_DONE:
	sig = (sig << 8) | isp_rx_buff[3];
	isp_read_signature_state = ISP_READ_SIG_START_3;
	break;

    case ISP_READ_SIG_START_3:
	isp_tx_buff[0] = 0x30;
	isp_tx_buff[1] = 0x00;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_signature_state = ISP_READ_SIG_WAIT_XMIT_3;
	break;

    case ISP_READ_SIG_WAIT_XMIT_3:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_signature_state = ISP_READ_SIG_START;
	    return isp_error(4);
	    }
	break;

    case ISP_READ_SIG_XMIT_3_DONE:
	*out = (sig << 8) | isp_rx_buff[3];
	isp_read_signature_state = ISP_READ_SIG_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_low_fuse_nb(uint8_t *out)/*{{{*/
{
switch(isp_read_low_fuse_state)
    {
    case ISP_READ_LOW_FUSE_START:
	isp_tx_buff[0] = 0x50;
	isp_tx_buff[1] = 0x00;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_low_fuse_state = ISP_READ_LOW_FUSE_WAIT_XMIT;
	break;

    case ISP_READ_LOW_FUSE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_low_fuse_state = ISP_READ_LOW_FUSE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_READ_LOW_FUSE_XMIT_DONE:
	*out = isp_rx_buff[3];
	isp_read_low_fuse_state = ISP_READ_LOW_FUSE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_high_fuse_nb(uint8_t *out)/*{{{*/
{
switch(isp_read_high_fuse_state)
    {
    case ISP_READ_HIGH_FUSE_START:
	isp_tx_buff[0] = 0x58;
	isp_tx_buff[1] = 0x08;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_high_fuse_state = ISP_READ_HIGH_FUSE_WAIT_XMIT;
	break;

    case ISP_READ_HIGH_FUSE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_high_fuse_state = ISP_READ_HIGH_FUSE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_READ_HIGH_FUSE_XMIT_DONE:
	*out = isp_rx_buff[3];
	isp_read_high_fuse_state = ISP_READ_HIGH_FUSE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_ext_fuse_nb(uint8_t *out)/*{{{*/
{
switch(isp_read_ext_fuse_state)
    {
    case ISP_READ_EXT_FUSE_START:
	isp_tx_buff[0] = 0x50;
	isp_tx_buff[1] = 0x08;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = 0x00;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_read_ext_fuse_state = ISP_READ_EXT_FUSE_WAIT_XMIT;
	break;

    case ISP_READ_EXT_FUSE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_read_ext_fuse_state = ISP_READ_EXT_FUSE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_READ_EXT_FUSE_XMIT_DONE:
	*out = isp_rx_buff[3];
	isp_read_ext_fuse_state = ISP_READ_EXT_FUSE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_low_fuse_nb(uint8_t lf)/*{{{*/
{
switch(isp_write_low_fuse_state)
    {
    case ISP_WRITE_LOW_FUSE_START:
	isp_tx_buff[0] = 0xAC;
	isp_tx_buff[1] = 0xA0;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = lf;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_write_low_fuse_state = ISP_WRITE_LOW_FUSE_WAIT_XMIT;
	break;

    case ISP_WRITE_LOW_FUSE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_write_low_fuse_state = ISP_WRITE_LOW_FUSE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_WRITE_LOW_FUSE_XMIT_DONE:
	isp_write_low_fuse_state = ISP_WRITE_LOW_FUSE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_high_fuse_nb(uint8_t hf)/*{{{*/
{
switch(isp_write_high_fuse_state)
    {
    case ISP_WRITE_HIGH_FUSE_START:
	isp_tx_buff[0] = 0xAC;
	isp_tx_buff[1] = 0xA8;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = hf;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_write_high_fuse_state = ISP_WRITE_HIGH_FUSE_WAIT_XMIT;
	break;

    case ISP_WRITE_HIGH_FUSE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_write_high_fuse_state = ISP_WRITE_HIGH_FUSE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_WRITE_HIGH_FUSE_XMIT_DONE:
	isp_write_high_fuse_state = ISP_WRITE_HIGH_FUSE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_ext_fuse_nb(uint8_t ef)/*{{{*/
{
switch(isp_write_ext_fuse_state)
    {
    case ISP_WRITE_EXT_FUSE_START:
	isp_tx_buff[0] = 0xAC;
	isp_tx_buff[1] = 0xA4;
	isp_tx_buff[2] = 0x00;
	isp_tx_buff[3] = ef;
	HAL_SPI_TransmitReceive_IT(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE);
	clock = HAL_GetTick();
	isp_write_ext_fuse_state = ISP_WRITE_EXT_FUSE_WAIT_XMIT;
	break;

    case ISP_WRITE_EXT_FUSE_WAIT_XMIT:
	if(HAL_GetTick() - clock >= 100)
	    {
	    isp_write_ext_fuse_state = ISP_WRITE_EXT_FUSE_START;
	    return isp_error(2);
	    }
	break;

    case ISP_WRITE_EXT_FUSE_XMIT_DONE:
	isp_write_ext_fuse_state = ISP_WRITE_EXT_FUSE_START;
	isp_lock = 0;
	return 255;
    };

return 0;
}/*}}}*/
//--------------------------------------------------
// isp_exit is uniform
// for blocking and non-blocking ways
//--------------------------------------------------
// interrupts here
//--------------------------------------------------
void isp_spi_isr(void)/*{{{*/
{
if(isp_enter_state == ISP_ENTER_WAIT_XMIT)
    { isp_enter_state = ISP_ENTER_XMIT_DONE; }
if(isp_enter_state == ISP_ENTER_WAIT_XMIT_LOW)
    { isp_enter_state = ISP_ENTER_XMIT_DONE_LOW; }

if(isp_chiperase_state == ISP_CHIPERASE_WAIT_XMIT)
    { isp_chiperase_state = ISP_CHIPERASE_XMIT_DONE; }

if(isp_lea_state == ISP_LEA_WAIT_XMIT)
    { isp_lea_state = ISP_LEA_XMIT_DONE; }
if(isp_read_flash_state == ISP_READ_FLASH_WAIT_XMIT)
    { isp_read_flash_state = ISP_READ_FLASH_XMIT_DONE; }
if(isp_load_flash_state == ISP_LOAD_FLASH_WAIT_XMIT)
    { isp_load_flash_state = ISP_LOAD_FLASH_XMIT_DONE; }
if(isp_burn_flash_state == ISP_BURN_FLASH_WAIT_XMIT)
    { isp_burn_flash_state = ISP_BURN_FLASH_XMIT_DONE; }

if(isp_read_eeprom_state == ISP_READ_EE_WAIT_XMIT)
    { isp_read_eeprom_state = ISP_READ_EE_XMIT_DONE; }
if(isp_write_eeprom_state == ISP_WRITE_EE_WAIT_XMIT)
    { isp_write_eeprom_state = ISP_WRITE_EE_XMIT_DONE; }

if(isp_read_lock_state == ISP_READ_LOCK_WAIT_XMIT)
    { isp_read_lock_state = ISP_READ_LOCK_XMIT_DONE; }
if(isp_write_lock_state == ISP_WRITE_LOCK_WAIT_XMIT)
    { isp_write_lock_state = ISP_WRITE_LOCK_XMIT_DONE; }

if(isp_read_signature_state == ISP_READ_SIG_WAIT_XMIT_1)
    { isp_read_signature_state = ISP_READ_SIG_XMIT_1_DONE; }
if(isp_read_signature_state == ISP_READ_SIG_WAIT_XMIT_2)
    { isp_read_signature_state = ISP_READ_SIG_XMIT_2_DONE; }
if(isp_read_signature_state == ISP_READ_SIG_WAIT_XMIT_3)
    { isp_read_signature_state = ISP_READ_SIG_XMIT_3_DONE; }

if(isp_write_low_fuse_state == ISP_WRITE_LOW_FUSE_WAIT_XMIT)
    { isp_write_low_fuse_state = ISP_WRITE_LOW_FUSE_XMIT_DONE; }
if(isp_write_high_fuse_state == ISP_WRITE_HIGH_FUSE_WAIT_XMIT)
    { isp_write_high_fuse_state = ISP_WRITE_HIGH_FUSE_XMIT_DONE; }
if(isp_write_ext_fuse_state == ISP_WRITE_EXT_FUSE_WAIT_XMIT)
    { isp_write_ext_fuse_state = ISP_WRITE_EXT_FUSE_XMIT_DONE; }

if(isp_read_low_fuse_state == ISP_READ_LOW_FUSE_WAIT_XMIT)
    { isp_read_low_fuse_state = ISP_READ_LOW_FUSE_XMIT_DONE; }
if(isp_read_high_fuse_state == ISP_READ_HIGH_FUSE_WAIT_XMIT)
    { isp_read_high_fuse_state = ISP_READ_HIGH_FUSE_XMIT_DONE; }
if(isp_read_ext_fuse_state == ISP_READ_EXT_FUSE_WAIT_XMIT)
    { isp_read_ext_fuse_state = ISP_READ_EXT_FUSE_XMIT_DONE; }
}/*}}}*/
//--------------------------------------------------
