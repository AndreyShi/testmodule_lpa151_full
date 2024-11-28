//--------------------------------------------------
// Модуль управления дискретными сигналами
//--------------------------------------------------
#include "app_config.h"

#include "gpio.h"
#include "gpio_if.h"

// светодиоды
//--------------------------------------------------
#define RED_LED_SET (GPIOA->BSRR = GPIO_BSRR_BS_3)
#define RED_LED_RST (GPIOA->BSRR = GPIO_BSRR_BR_3)
#define RED_LED_GET HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)

#define BLU_LED_SET (GPIOA->BSRR = GPIO_BSRR_BS_0)
#define BLU_LED_RST (GPIOA->BSRR = GPIO_BSRR_BR_0)
#define BLU_LED_GET HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)

#define GRN_LED_SET (GPIOA->BSRR = GPIO_BSRR_BS_1)
#define GRN_LED_RST (GPIOA->BSRR = GPIO_BSRR_BR_1)
#define GRN_LED_GET HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)

#define YLW_LED_SET (GPIOA->BSRR = GPIO_BSRR_BS_2)
#define YLW_LED_RST (GPIOA->BSRR = GPIO_BSRR_BR_2)
#define YLW_LED_GET HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)

//--------------------------------------------------
uint8_t led_set(const led_id_t led, const state_t state)/*{{{*/
{
switch(led)
    {
    case TM_151_LED_RED:
	state == STATE_ON ? RED_LED_SET : RED_LED_RST;
	break;
    case TM_151_LED_BLUE:
	state == STATE_ON ? BLU_LED_SET : BLU_LED_RST;
	break;
    case TM_151_LED_GREEN:
	state == STATE_ON ? GRN_LED_SET : GRN_LED_RST;
	break;
    case TM_151_LED_YELLOW:
	state == STATE_ON ? YLW_LED_SET : YLW_LED_RST;
	break;

    default:
	return 0x01;
    };

return 0x00;
}/*}}}*/
//--------------------------------------------------
uint8_t led_tgl(const led_id_t led)/*{{{*/
{
switch(led)
    {
    case TM_151_LED_RED:
	RED_LED_GET == GPIO_PIN_RESET ? RED_LED_SET : RED_LED_RST;
	break;
    case TM_151_LED_BLUE:
	BLU_LED_GET == GPIO_PIN_RESET ? BLU_LED_SET : BLU_LED_RST;
	break;
    case TM_151_LED_GREEN:
	GRN_LED_GET == GPIO_PIN_RESET ? GRN_LED_SET : GRN_LED_RST;
	break;
    case TM_151_LED_YELLOW:
	YLW_LED_GET == GPIO_PIN_RESET ? YLW_LED_SET : YLW_LED_RST;
	break;

    default:
	return 0x01;
    };

return 0x00;
}/*}}}*/
//--------------------------------------------------
