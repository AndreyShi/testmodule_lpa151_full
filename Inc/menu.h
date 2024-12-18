//--------------------------------------------------
// Функции управления меню
//--------------------------------------------------
#ifndef MENU_H
#define MENU_H

//--------------------------------------------------
/* Меню модуля
 *
 * Данный программный модуль берет на себя работу с
 * кнопками и экраном, отрисовывает различные
 * экранчики с данными о состоянии модуля и
 * осуществляет переходы между ними.
 *
 * Данные для отображения модуль сам получает данные
 * от внешних источников (Engine, BusManager).
 * Так же модуль использует c-api для подачи команд
 * движку.
 */
//--------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "common.h"

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void menu_init(void);
extern void menu_task(void);

// external interface
extern void menu_update(void);

// show stage screens and testing results
extern void menu_set_stage(uint8_t stage);

#ifdef __cplusplus
};
#endif
//--------------------------------------------------
#endif
