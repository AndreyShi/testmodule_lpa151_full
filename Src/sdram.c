//--------------------------------------------------
// Подготовка памяти к работе
//--------------------------------------------------
#include <stdint.h>

#include "sdram.h"

//--------------------------------------------------
#define SDRAM_REFRESH ((uint32_t)0x0603)
#define SDRAM_TIMEOUT ((uint32_t)0xFFFF)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 

//--------------------------------------------------
void SDRAM_Init(SDRAM_HandleTypeDef *hsdram)
{
uint32_t bank;
FMC_SDRAM_CommandTypeDef cmd;

if(hsdram == &hsdram1)
    { bank = FMC_SDRAM_CMD_TARGET_BANK1; }
else
    { bank = FMC_SDRAM_CMD_TARGET_BANK2; }

/* Step 1. Enable clock */
cmd.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
cmd.CommandTarget = bank;
cmd.AutoRefreshNumber = 1;
cmd.ModeRegisterDefinition = 0;
HAL_SDRAM_SendCommand(hsdram, &cmd, SDRAM_TIMEOUT);

/* Step 2. Wait for at least 100us */
HAL_Delay(2);

/* Step 3. Precharge all bits */
cmd.CommandMode = FMC_SDRAM_CMD_PALL;
cmd.CommandTarget = bank;
cmd.AutoRefreshNumber = 1;
cmd.ModeRegisterDefinition = 0;
HAL_SDRAM_SendCommand(hsdram, &cmd, SDRAM_TIMEOUT);

/* Step 4. Set auto refresh */
cmd.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
cmd.CommandTarget = bank;
cmd.AutoRefreshNumber = 8;
cmd.ModeRegisterDefinition = 0;
HAL_SDRAM_SendCommand(hsdram, &cmd, SDRAM_TIMEOUT);

/* Step 5. Set mode register */
cmd.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
cmd.CommandTarget = bank;
cmd.AutoRefreshNumber = 1;
cmd.ModeRegisterDefinition = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 |\
			     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |\
			     SDRAM_MODEREG_CAS_LATENCY_2 |\
			     SDRAM_MODEREG_OPERATING_MODE_STANDARD |\
			     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
HAL_SDRAM_SendCommand(hsdram, &cmd, SDRAM_TIMEOUT);

/* Step 6. Program refresh counter */
HAL_SDRAM_ProgramRefreshRate(hsdram, SDRAM_REFRESH);
}
//--------------------------------------------------
