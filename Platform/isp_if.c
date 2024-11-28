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
const uint32_t isp_atmega16u4_signature = 0x0087951E;
const uint32_t isp_atmega32u4_signature = 0x0088941E;

//--------------------------------------------------
uint8_t isp_tx_buff[ISP_TRANSFER_SIZE];
uint8_t isp_rx_buff[ISP_TRANSFER_SIZE];

//--------------------------------------------------
// lock makes reentering functions a failure
uint8_t isp_lock = 0;

//--------------------------------------------------
// utility functions
//--------------------------------------------------
uint8_t isp_go_slow(void)/*{{{*/
{
isp_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
if(HAL_SPI_Init(&isp_spi) != HAL_OK)
    { return 1; }

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_go_fast(void)/*{{{*/
{
isp_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
if(HAL_SPI_Init(&isp_spi) != HAL_OK)
    { return 1; }

return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_is_fast(void)
{ return isp_spi.Init.BaudRatePrescaler == SPI_BAUDRATEPRESCALER_32 ? 1 : 0; }
//--------------------------------------------------
uint8_t isp_exit(void)/*{{{*/
{
LPA_ISP_DISABLE;
LPA_ISP_UNRESET;

return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_check_lock(void)/*{{{*/
{
if(isp_lock)
    { return 1; }

isp_lock = 1;
return 0;
}/*}}}*/
//--------------------------------------------------
void isp_parse_error(const char *cmd, uint8_t err)/*{{{*/
{
switch(err)
    {
    case 1: printf("%s.failed.resource locked.\n",                cmd); break;
    case 2: printf("%s.failed.isp high speed receive timeout.\n", cmd); break;
    case 3: printf("%s.failed.fail to enter programming mode.\n", cmd); break;
    case 4: printf("%s.failed.isp detection failed.\n",           cmd); break;
    case 5: printf("%s.failed.error setting low isp speed.\n",    cmd); break;
    case 6: printf("%s.failed.error setting high isp speed.\n",   cmd); break;
    case 7: printf("%s.failed.isp low speed receive timeout.\n",  cmd); break;
    case 8: printf("%s.failed.invalid isp receive.\n",            cmd); break;
    default: printf("%s.failed.unknown error %d\n", cmd, err); break;
    };
}/*}}}*/
//--------------------------------------------------
uint8_t isp_error(uint8_t err)/*{{{*/
{
isp_exit();
isp_lock = 0;
return err;
}/*}}}*/
//--------------------------------------------------
// go the blocking way first
//--------------------------------------------------
uint8_t isp_enter(void)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

LPA_ISP_RESET;
HAL_Delay(100);

if(LPA_ISP_DETECT == GPIO_PIN_SET)
    {
    relay_set(TM_151_RELAY_LPA_POWER, CH_1, STATE_ON);
    HAL_Delay(100);
    }

if(LPA_ISP_DETECT == GPIO_PIN_SET)
    { return isp_error(2); }

LPA_ISP_UNRESET;
HAL_Delay(10);
LPA_ISP_RESET;
LPA_ISP_ENABLE;

/* no less than 20ms */
HAL_Delay(21);

/* enable serial programming */
isp_tx_buff[0] = 0xAC;
isp_tx_buff[1] = 0x53;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

/* check programming enter */
if(isp_rx_buff[2] != 0x53)
    { return isp_error(3); }

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_chip_erase(void)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xAC;
isp_tx_buff[1] = 0x80;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_load_ext_addr(uint8_t ext_addr)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x4D;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = ext_addr;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_flash(uint16_t addr, uint8_t h, uint8_t *out)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x20 | (h == 0 ? 0x00 : 0x08);
isp_tx_buff[1] = (addr >> 8) & 0x00FF;
isp_tx_buff[2] = (addr     ) & 0x00FF;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_load_flash(uint8_t val, uint8_t addr, uint8_t h)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x40 | (h == 0 ? 0x00 : 0x08);
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = addr & 0x3F;
isp_tx_buff[3] = val;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_burn_flash(uint16_t addr)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x4C;
isp_tx_buff[1] = (addr >> 8) & 0x00FF;
isp_tx_buff[2] = (addr     ) & 0x00C0;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

HAL_Delay(6);
isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_eeprom(uint16_t addr, uint8_t *out)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xA0;
isp_tx_buff[1] = (addr >> 8) & 0x000F;
isp_tx_buff[2] = (addr     ) & 0x00FF;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_eeprom(uint8_t val, uint16_t addr)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xC0;
isp_tx_buff[1] = (addr >> 8) & 0x000F;
isp_tx_buff[2] = (addr     ) & 0x00FF;
isp_tx_buff[3] = val;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

HAL_Delay(10);
isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_load_eeprom(uint8_t val, uint8_t addr)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xC1;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = addr & 0x03;
isp_tx_buff[3] = val;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_burn_eeprom(uint16_t addr)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xC2;
isp_tx_buff[1] = (addr >> 8) & 0x0F;
isp_tx_buff[2] = (addr     ) & 0xFC;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

HAL_Delay(10);
isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_lock(uint8_t *out)/*{{{*/
{
isp_tx_buff[0] = 0x58;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_lock(uint8_t lock)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xAC;
isp_tx_buff[1] = 0xE0;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0xC0 | lock;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_signature(uint32_t *out)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x30;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = 0x02;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_tx_buff[0] = 0x30;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = 0x01;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = (*out << 8) | isp_rx_buff[3];

isp_tx_buff[0] = 0x30;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = (*out << 8) | isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_low_fuse(uint8_t lf)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xAC;
isp_tx_buff[1] = 0xA0;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = lf;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_high_fuse(uint8_t hf)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xAC;
isp_tx_buff[1] = 0xA8;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = hf;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_write_ext_fuse(uint8_t ef)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0xAC;
isp_tx_buff[1] = 0xA4;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = ef;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_low_fuse(uint8_t *out)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x50;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_high_fuse(uint8_t *out)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x58;
isp_tx_buff[1] = 0x08;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_ext_fuse(uint8_t *out)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x50;
isp_tx_buff[1] = 0x08;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
uint8_t isp_read_calib_byte(uint8_t *out)/*{{{*/
{
if(isp_check_lock() != 0)
    { return 1; }

isp_tx_buff[0] = 0x38;
isp_tx_buff[1] = 0x00;
isp_tx_buff[2] = 0x00;
isp_tx_buff[3] = 0x00;
HAL_SPI_TransmitReceive(&isp_spi, isp_tx_buff, isp_rx_buff, ISP_TRANSFER_SIZE, 100);

*out = isp_rx_buff[3];

isp_lock = 0;
return 255;
}/*}}}*/
//--------------------------------------------------
