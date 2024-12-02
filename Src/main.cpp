//--------------------------------------------------
#include "stm32f7xx_hal.h"

#include "dma.h"
#include "gpio.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "usb_host.h"

#include "bus_uart_if.h"
#include "firmware.h"
#include "gpio_if.h"
#include "isp_if.h"
#include "qspi_flash.h"
#include "relay_if.h"
#include "render.h"
#include "retarget.h"
#include "ssd1306.h"

#include "app_config.h"
#include "animation.h"
#include "boot.h"
#include "emu_u.h"
#include "resources.h"
#include "menu.h"

#include "module_commands.hpp"

#include "KBusManager.hpp"
#include "KUSBManager.hpp"
#include "KModuleEngine.hpp"

#include <stdio.h>

// debug suit
//--------------------------------------------------
static size_t allocs_count = 0;
static size_t frees_count = 0;

static uint8_t diffs_cur = 0;
static size_t diffs[32];
//--------------------------------------------------
void *operator new(size_t s)
{
allocs_count++;
return malloc(s);
}
//--------------------------------------------------
void operator delete(void *p)
{
frees_count++;
free(p);
}
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
int main(void)
{
SCB_EnableICache();
HAL_Init();
SystemClock_Config();

/* L0 initialization */
// NB: dma and gpio must go first! {{{
MX_GPIO_Init();
MX_DMA_Init();
MX_QUADSPI_Init();
MX_SPI1_Init();
MX_SPI2_Init();
MX_SPI5_Init();
MX_TIM3_Init();
MX_TIM6_Init();
MX_USART3_UART_Init();
MX_USB_DEVICE_Init();
//MX_IWDG_Init();
MX_USB_HOST_Init();
/*}}}*/
/* L1 initialization */
QSPI_Flash_Init();
/* L2 initialization {{{ */
boot_init();
bus_init();
emuu_init();
relay_init();
firmware_init();
menu_init();
ssd1306_init();

display_logo();
/* }}} */
/* L3 initialization */
//HAL_IWDG_Refresh(&hiwdg);
M = new KBusManager();
U = new KUSBManager(260);
E = new KModuleEngine();

finish_logo();
render_cls();
led_set(TM_151_LED_GREEN, STATE_ON);
while(1)
    {
    //HAL_IWDG_Refresh(&hiwdg);
    //menu_task();
    MX_USB_HOST_Process();
    menu_task();
    retarget_task();
    ssd1306_task();

    M->step();
    U->step();
    E->step();

    /* test site */
    diffs[diffs_cur] = allocs_count - frees_count;

    diffs_cur++;
    if(diffs_cur >= 32)
	{ diffs_cur = 0; }
    /* end test site */
    }
}
//--------------------------------------------------
void SystemClock_Config(void)/*{{{*/
{
RCC_OscInitTypeDef RCC_OscInitStruct;
RCC_ClkInitTypeDef RCC_ClkInitStruct;
RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

__HAL_RCC_PWR_CLK_ENABLE();
__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
RCC_OscInitStruct.HSEState = RCC_HSE_ON;
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
RCC_OscInitStruct.PLL.PLLM = 4;
RCC_OscInitStruct.PLL.PLLN = 216;
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
RCC_OscInitStruct.PLL.PLLQ = 9;
HAL_RCC_OscConfig(&RCC_OscInitStruct);

HAL_PWREx_EnableOverDrive();

RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);

PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3| RCC_PERIPHCLK_CLK48;
PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/8000);
HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);

/* SysTick_IRQn interrupt configuration */
HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}/*}}}*/
//--------------------------------------------------
void _Error_Handler(char *, int)/*{{{*/
{
while(1)
    {
    led_tgl(TM_151_LED_RED);
    HAL_Delay(300);
    }
}/*}}}*/
void _Error_Handler_new(int led)/*{{{*/
{
while(1)
    {
    led_tgl((_led_id)led);
    HAL_Delay(300);
    }
}/*}}}*/
//--------------------------------------------------
void HardFault_Handler(void)/*{{{*/
{
while(1)
    {
    led_tgl(TM_151_LED_RED);
    HAL_Delay(150);
    };
}/*}}}*/
//--------------------------------------------------
void HAL_SYSTICK_Callback()/*{{{*/
{
relay_systick_isr();
animation_systick_isr();
}/*}}}*/
//--------------------------------------------------
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)/*{{{*/
{
if(GPIO_Pin == FRAME_IN_PIN)
    { bus_gpio_isr(); }
}/*}}}*/
//--------------------------------------------------
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)/*{{{*/
{
if(huart->Instance == bus_uart.Instance)
    { bus_uart_isr(); }
}/*}}}*/
//--------------------------------------------------
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)/*{{{*/
{
if(hspi->Instance == disp_spi.Instance)
    { ssd_spi_isr(); }
}/*}}}*/
//--------------------------------------------------
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)/*{{{*/
{
if(hspi->Instance == isp_spi.Instance)
    { isp_spi_isr(); }
}/*}}}*/
//--------------------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)/*{{{*/
{
if(htim->Instance == bus_tim.Instance)
    { bus_tim_isr(); }
}/*}}}*/
//--------------------------------------------------
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ }
#endif
//--------------------------------------------------
