//--------------------------------------------------
// Конфигурация приложения
//--------------------------------------------------
#ifndef TESTMODULE_151_CONFIG_H
#define TESTMODULE_151_CONFIG_H

//--------------------------------------------------
#include "common.h"
#include "app_export.h"

// Псевдонимы для периферии
//--------------------------------------------------
#define bus_uart  huart3

#define enc_tim   htim3
#define bus_tim   htim6

#define disp_spi  hspi1
#define emuu_spi  hspi2
#define isp_spi   hspi5

// Настройки движка исполнения скриптов
//--------------------------------------------------
#define ENGINE_I_REG_COUNT 2
#define ENGINE_F_REG_COUNT 2

//--------------------------------------------------
#endif
