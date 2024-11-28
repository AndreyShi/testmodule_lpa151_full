//--------------------------------------------------
// Эмулятор напряжения.
// Технически просто очень точный управляемый
// источник напряжения
//--------------------------------------------------
#include "app_config.h"
#include "emu_u.h"
#include "gpio_if.h"
#include "spi.h"

#include "KModuleState151.h"

//--------------------------------------------------
static const uint32_t emu_nop       = 0x00000000;
static const uint32_t emu_dac       = 0x00100000;
static const uint32_t emu_control   = 0x00200000;
static const uint32_t emu_clrcode   = 0x00300000;
static const uint32_t emu_swctrl    = 0x00400000;

static const uint32_t emu_rddac     = 0x00900000;
static const uint32_t emu_rdctrl    = 0x00A00000;
static const uint32_t emu_rdclrcode = 0x00B00000;

static const uint32_t emu_mask      = 0x000FFFFF;
static const uint32_t emu_ctrl_mask = 0x000001FF;

//--------------------------------------------------
static uint8_t emu_ready;
static uint32_t emuu_buff;
//--------------------------------------------------
static uint32_t mask_emu_cmd(const uint32_t cmd, const uint32_t arg);
//--------------------------------------------------
void emuu_init()
{
EMUU_SYNC_SET;
EMUU_CLR_SET;
EMUU_LDAC_SET;
EMUU_UNRESET;
emu_ready = 0;

// single-sided 0 to Vref output
// disregarded because of emuu_positive and emuu_negative pcb error
//const uint32_t dac_ctrl = 0x00000012;
// double-sided -Vref to Vref output
const uint32_t dac_ctrl = 0x00000000;

HAL_Delay(1);
HAL_SPI_Transmit(&emuu_spi, &emu_ready, 1, 10);
HAL_Delay(1);

emuu_buff = mask_emu_cmd(emu_control, dac_ctrl);
EMUU_SYNC_RST;
HAL_Delay(1);
if(HAL_SPI_Transmit(&emuu_spi, (uint8_t *)&emuu_buff, 3, 10) != HAL_OK)
    {
    EMUU_SYNC_SET;
    return;
    }
HAL_Delay(1);
EMUU_SYNC_SET;

HAL_Delay(10);
EMUU_SYNC_RST;
emuu_buff = mask_emu_cmd(emu_rdctrl, 0x00000000);
HAL_Delay(1);
if(HAL_SPI_Transmit(&emuu_spi, (uint8_t *)&emuu_buff, 3, 10) != HAL_OK)
    {
    EMUU_SYNC_SET;
    return;
    }
HAL_Delay(1);
EMUU_SYNC_SET;

uint32_t in;
HAL_Delay(10);
EMUU_SYNC_RST;
emuu_buff = mask_emu_cmd(emu_nop, 0x00000000);
HAL_Delay(1);
if(HAL_SPI_TransmitReceive(&emuu_spi, (uint8_t *)&emuu_buff, (uint8_t *)&in, 3, 10) != HAL_OK)
    {
    EMUU_SYNC_SET;
    return;
    }
HAL_Delay(1);
EMUU_SYNC_SET;

if(in == mask_emu_cmd(emu_control, dac_ctrl))
    {
    emu_ready = 1;
    EMUU_LDAC_RST;
    }
}
//--------------------------------------------------
uint8_t emuu_set(const uint32_t val)
{
if(emu_ready == 0)
    { return 0x01; }

if(val > emu_mask)
    { return 0x02; }

EMUU_SYNC_RST;
emuu_buff = mask_emu_cmd(emu_dac, val);
if(HAL_SPI_Transmit(&emuu_spi, (uint8_t *)&emuu_buff, 3, 10) != HAL_OK)
    {
    EMUU_SYNC_SET;
    return 0x03;
    }
EMUU_SYNC_SET;

return 0x00;
}
//--------------------------------------------------
static uint32_t mask_emu_cmd(uint32_t cmd, uint32_t arg)
{
uint32_t tmp;
uint32_t res;

tmp  = (cmd | (arg & emu_mask)) & 0x00FFFFFF;

res  =  (tmp & 0x000000FF) << 16;
res |=  (tmp & 0x0000FF00);
res |=  (tmp & 0x00FF0000) >> 16;
return res;
}
//--------------------------------------------------
