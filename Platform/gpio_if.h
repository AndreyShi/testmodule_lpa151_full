//--------------------------------------------------
// Модуль управления дискретными сигналами
//--------------------------------------------------
#ifndef GPIO_IF_H
#define GPIO_IF_H

//--------------------------------------------------
#include "common.h"

// Неуправляемые снаружи модуля сигналы
//--------------------------------------------------
#define FRAME_START	(GPIOA->BSRR = GPIO_BSRR_BR_9)
#define FRAME_STOP	(GPIOA->BSRR = GPIO_BSRR_BS_9)

#define FRAME_IN_PIN	GPIO_PIN_10
#define FRAME_IN_STATE	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)

#define FRAME_ENABLE    (GPIOD->BSRR = GPIO_BSRR_BS_8)
#define FRAME_DISABLE   (GPIOD->BSRR = GPIO_BSRR_BR_8)

#define LPA_ISP_RESET   (GPIOF->BSRR = GPIO_BSRR_BR_6)
#define LPA_ISP_UNRESET (GPIOF->BSRR = GPIO_BSRR_BS_6)

#define LPA_ISP_PIN     GPIO_PIN_5
#define LPA_ISP_DETECT  HAL_GPIO_ReadPin(GPIOF, LPA_ISP_PIN)

#define LPA_ISP_ENABLE  (GPIOF->BSRR = GPIO_BSRR_BS_10)
#define LPA_ISP_DISABLE (GPIOF->BSRR = GPIO_BSRR_BR_10)

#define RELAY_EN        (GPIOD->BSRR = GPIO_BSRR_BS_3)
#define RELAY_DIS       (GPIOD->BSRR = GPIO_BSRR_BR_3)

#define BOOT_EXIT       (GPIOB->BSRR = GPIO_BSRR_BR_7)
#define BOOT_ENTER      (GPIOB->BSRR = GPIO_BSRR_BS_7)

#define BUS_POWER_PIN    GPIO_PIN_9
#define BUS_POWER_DETECT HAL_GPIO_ReadPin(GPIOD, BUS_POWER_PIN)

#define EMUU_RESET      (GPIOH->BSRR = GPIO_BSRR_BR_15)
#define EMUU_UNRESET    (GPIOH->BSRR = GPIO_BSRR_BS_15)

#define EMUU_SYNC_SET   (GPIOI->BSRR = GPIO_BSRR_BS_0)
#define EMUU_SYNC_RST   (GPIOI->BSRR = GPIO_BSRR_BR_0)

#define EMUU_LDAC_SET   (GPIOH->BSRR = GPIO_BSRR_BS_13)
#define EMUU_LDAC_RST   (GPIOH->BSRR = GPIO_BSRR_BR_13)

#define EMUU_CLR_SET   (GPIOH->BSRR = GPIO_BSRR_BS_14)
#define EMUU_CLR_RST   (GPIOH->BSRR = GPIO_BSRR_BR_14)

#define SSD_RESET   (GPIOC->BSRR = GPIO_BSRR_BR_4)
#define SSD_UNRESET (GPIOC->BSRR = GPIO_BSRR_BS_4)

#define SSD_CS_RESET (GPIOA->BSRR = GPIO_BSRR_BR_4)
#define SSD_CS_SET   (GPIOA->BSRR = GPIO_BSRR_BS_4)

#define SSD_COMMAND (GPIOC->BSRR = GPIO_BSRR_BR_5)
#define SSD_DATA    (GPIOC->BSRR = GPIO_BSRR_BS_5)
// Управляемые снаружи модуля сигналы
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t led_set(const led_id_t led, const state_t state);
extern uint8_t led_tgl(const led_id_t led);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
