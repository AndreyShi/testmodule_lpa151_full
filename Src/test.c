//--------------------------------------------------
// Файлик с тестами различной периферии
//--------------------------------------------------
#include "test.h"

#include "rng.h"
#include "usart.h"

#include "app_config.h"
#include "emu_u.h"
#include "gpio_if.h"
#include "relay_if.h"

//--------------------------------------------------
static const int rap[][3] = {/*{{{*/
    /* intro */
    { TM_151_RELAY_LPA_IN_PLUS_1,  CH_1,  100 },
    { TM_151_RELAY_LPA_IN_PLUS_2,  CH_1,  100 },
    { TM_151_RELAY_LPA_IN_PLUS_3,  CH_1,  100 },

    /* melody 1 */
    { TM_151_RELAY_LPA_IN_PLUS_4,  CH_1,  100 },
    { TM_151_RELAY_SHORT, CH_ALL, 100 },
    { TM_151_RELAY_R1, CH_ALL, 100 },
    { TM_151_RELAY_R2, CH_ALL, 100 },
    { TM_151_RELAY_LPA_IN_PLUS_1,  CH_2,  100 },
    { TM_151_RELAY_R3, CH_ALL, 100 },
    { TM_151_RELAY_R4, CH_ALL, 100 },
    { TM_151_RELAY_R5, CH_ALL, 100 },
    { TM_151_RELAY_LPA_IN_PLUS_2,  CH_2,  100 },
    { TM_151_RELAY_R6, CH_ALL, 100 },
    { TM_151_RELAY_R7, CH_ALL, 100 },
    { TM_151_RELAY_AG401_1, CH_1, 100 },

    /* repeat */
    { TM_151_RELAY_LPA_IN_PLUS_3,  CH_2,  100 },
    { TM_151_RELAY_EMUU_POSITIVE, CH_ALL, 100 },
    { TM_151_RELAY_EMUU_NEGATIVE, CH_ALL, 100 },
    { TM_151_RELAY_SENSE_AG420_1, CH_1, 100 },
    { TM_151_RELAY_LPA_IN_PLUS_4,  CH_2,  100 },
    { TM_151_RELAY_SENSE_AG420_1, CH_2, 100 },
    { TM_151_RELAY_SENSE_AG420_2, CH_1, 100 },
    { TM_151_RELAY_SENSE_AG420_2, CH_2, 100 },
    { TM_151_RELAY_LPA_IN_MINUS_1, CH_1,  100 },
    { TM_151_RELAY_THERMO_1, CH_1, 100 },
    { TM_151_RELAY_THERMO_1, CH_2, 100 },
    { TM_151_RELAY_THERMO_2, CH_1, 100 },
    { TM_151_RELAY_THERMO_2, CH_2, 100 },

    /* melody 2 */
    { TM_151_RELAY_LPA_IN_MINUS_2, CH_1,  100 },
    { TM_151_RELAY_AG401_1, CH_1, 100 },
    { TM_151_RELAY_AG420_1, CH_1, 100 },

    { TM_151_RELAY_LPA_IN_MINUS_3, CH_1,  100 },
    { TM_151_RELAY_AG401_1, CH_2, 100 },
    { TM_151_RELAY_AG420_1, CH_2, 100 },

    { TM_151_RELAY_LPA_IN_MINUS_4, CH_1,  100 },
    { TM_151_RELAY_AG401_2, CH_1, 100 },
    { TM_151_RELAY_AG420_2, CH_1, 100 },

    /* repeat */
    { TM_151_RELAY_LPA_IN_MINUS_1, CH_2,  100 },
    { TM_151_RELAY_AG401_2, CH_2, 100 },
    { TM_151_RELAY_AG420_2, CH_2, 100 },

    { TM_151_RELAY_LPA_IN_MINUS_2, CH_2,  100 },
    { TM_151_RELAY_WARMUP, CH_1, 100 },
    { TM_151_RELAY_SENSE, CH_1, 100 },

    { TM_151_RELAY_LPA_IN_MINUS_3, CH_2,  100 },
    { TM_151_RELAY_WARMUP, CH_2, 100 },
    { TM_151_RELAY_SENSE, CH_2, 100 },

    { TM_151_RELAY_LPA_IN_MINUS_4, CH_2,  100 },
    { TM_151_RELAY_LPA_POWER, CH_ALL,     100 },
    { 0xFFFFFFFF, CH_1, 100 }
};
/*}}}*/
//--------------------------------------------------
uint8_t buff_out[16];
uint8_t buff_in[16];

//--------------------------------------------------
void test_bus(void)/*{{{*/
{
uint32_t tmp;

for(uint8_t i=0; i<4; i++)
    {
    HAL_RNG_GenerateRandomNumber(&hrng, &tmp);
    buff_out[0 + 4*i] = (tmp      ) & 0x000000FF;
    buff_out[1 + 4*i] = (tmp >>  8) & 0x000000FF;
    buff_out[2 + 4*i] = (tmp >> 16) & 0x000000FF;
    buff_out[3 + 4*i] = (tmp >> 24) & 0x000000FF;
    }

HAL_UART_Transmit(&bus_uart, buff_out, 16, 50);
}/*}}}*/
//--------------------------------------------------
void test_emuu(void)/*{{{*/
{
uint32_t tmp;

HAL_RNG_GenerateRandomNumber(&hrng, &tmp);
emuu_set(EMUU_OFFSET, (uint8_t)tmp);
}/*}}}*/
//--------------------------------------------------
void test_relay(void)/*{{{*/
{
// ГТС-рэп на нашем модуле!
for(int i=0; rap[i][0] < 0xFF; i++)
    { 
    relay_set((relay_id_t)rap[i][0], (channels_t)rap[i][1], STATE_ON);
    HAL_Delay(rap[i][2]);
    }

for(int i=0; rap[i][0] < 0xFF; i++)
    { 
    relay_set((relay_id_t)rap[i][0], (channels_t)rap[i][1], STATE_OFF);
    HAL_Delay(rap[i][2]);
    }
}/*}}}*/
//--------------------------------------------------
