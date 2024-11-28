//--------------------------------------------------
// Уточнение поведения движка для модуля калибровки
// ЛПА-151
//--------------------------------------------------
#include <stdio.h>

#include "stm32f7xx_hal.h"

#include "app_export.h"
#include "engine_151_states.hpp"
#include "gpio_if.h"
#include "firmware.h"

// Just helper macro
//--------------------------------------------------
#define LOAD_SCRIPT(s) \
    if(s - SCRIPT_ID_BASE >= TM_151_SCRIPTS_COUNT) \
	{ return CMD_STATUS_FAILED; } \
    set_script((char *)firmware_get_script(s), TM_151_SCRIPT_SIZE); \
    return CMD_STATUS_DONE;

//--------------------------------------------------
static const int slow_blink = 400; // ms
static const int fast_blink = 150; // ms
//--------------------------------------------------
static uint32_t clock; // ms
//--------------------------------------------------
KEngineState<ENGINE_IDLE>               *state_idle      = new K151Idle;
KEngineState<ENGINE_EXECUTING>          *state_executing = new K151Executing;
KEngineState<ENGINE_EXECUTION_DONE>     *state_done      = new K151Done;
KEngineState<ENGINE_EXECUTION_FAILED>   *state_failed    = new K151Failed;
KEngineState<ENGINE_EXECUTION_ERROR>    *state_error     = new K151Error;
KEngineState<ENGINE_EXECUTION_OVERFLOW> *state_overflow  = new K151Overflow;
//--------------------------------------------------
cmd_status_t K151Idle::load_script(script_id_t script) const
{ LOAD_SCRIPT(script); }
//--------------------------------------------------
bool K151Idle::command_done(cmd_id_t) const/*{{{*/
{
led_set(TM_151_LED_RED, STATE_OFF);
led_tgl(TM_151_LED_YELLOW);
return true;
}/*}}}*/
//--------------------------------------------------
bool K151Idle::command_repeated(cmd_id_t) const/*{{{*/
{
uint32_t now;

now = HAL_GetTick();
if(now - clock > slow_blink)
    {
    led_tgl(TM_151_LED_YELLOW);
    clock = now;
    }

return false;
}/*}}}*/
//--------------------------------------------------
bool K151Idle::command_failed(cmd_id_t) const/*{{{*/
{
led_set(TM_151_LED_RED, STATE_ON);
return true;
}/*}}}*/
//--------------------------------------------------
void K151Idle::state_exit() const
{ led_set(TM_151_LED_YELLOW, STATE_OFF); }
//--------------------------------------------------
void K151Executing::indicate() const/*{{{*/
{
uint32_t now;

now = HAL_GetTick();
if(now - clock > slow_blink)
    {
    led_tgl(TM_151_LED_YELLOW);
    clock = now;
    }
}/*}}}*/
//--------------------------------------------------
void K151Executing::state_enter() const/*{{{*/
{
led_set(TM_151_LED_RED, STATE_OFF);
clock = HAL_GetTick();
}/*}}}*/
//--------------------------------------------------
void K151Executing::state_exit() const
{ led_set(TM_151_LED_YELLOW, STATE_OFF); }
//--------------------------------------------------
cmd_status_t K151Done::load_script(script_id_t script) const
{ LOAD_SCRIPT(script); }
//--------------------------------------------------
cmd_status_t K151Failed::load_script(script_id_t script) const
{ LOAD_SCRIPT(script); }
//--------------------------------------------------
void K151Failed::state_enter() const
{ led_set(TM_151_LED_RED, STATE_ON); }
//--------------------------------------------------
void K151Failed::state_exit() const
{ led_set(TM_151_LED_RED, STATE_OFF); }
//--------------------------------------------------
cmd_status_t K151Error::load_script(script_id_t script) const
{ LOAD_SCRIPT(script); }
//--------------------------------------------------
void K151Error::indicate() const/*{{{*/
{
uint32_t now;

now = HAL_GetTick();
if(now - clock > fast_blink)
    {
    led_tgl(TM_151_LED_RED);
    clock = now;
    }
}/*}}}*/
//--------------------------------------------------
void K151Error::state_enter() const
{ clock = HAL_GetTick(); }
//--------------------------------------------------
cmd_status_t K151Overflow::load_script(script_id_t script) const
{ LOAD_SCRIPT(script); }
//--------------------------------------------------
// cleanup
#undef LOAD_SCRIPT
//--------------------------------------------------
