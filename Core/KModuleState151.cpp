//--------------------------------------------------
// Module LPA-151 state
//--------------------------------------------------
#include <stdio.h>

#include "app_export.h"
#include "menu.h"

#include "KModuleState.hpp"
#include "KModuleState151.hpp"
#include "KModuleState151.h"

//--------------------------------------------------
#define TM_151_EXTRACT_PARAMETER(type, pos) \
    (dynamic_cast<KParameter<type> *>(module151s[pos]))

//--------------------------------------------------
template<>
const char *KModuleState<MODULE_LPA_151>::m_name = "Module LPA-151";
//--------------------------------------------------
template<>
void KModuleState<MODULE_LPA_151>::set_defaults()/*{{{*/
{
EXTRACT_STATE_PARAMETER(MODULE_LPA_151, TM_151_RELAYS)->set_value(0);

EXTRACT_STATE_PARAMETER(MODULE_LPA_151, TM_151_LPA_CONNECTION)->set_value(STATE_OFF);

EXTRACT_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_OFFSET)->set_value(0);
EXTRACT_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_FINE)->set_value(0);
EXTRACT_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_MIDDLE)->set_value(0);
EXTRACT_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_ROUGH)->set_value(0);
}/*}}}*/
//--------------------------------------------------
template<>
KModuleState<MODULE_LPA_151>::KModuleState()/*{{{*/
{
CREATE_STATE_PARAMETER(MODULE_LPA_151, TM_151_RELAYS)

CREATE_STATE_PARAMETER(MODULE_LPA_151, TM_151_LPA_CONNECTION)

CREATE_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_OFFSET)
CREATE_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_FINE)
CREATE_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_MIDDLE)
CREATE_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_ROUGH)

set_defaults();
}/*}}}*/
//--------------------------------------------------
static KModuleState<MODULE_LPA_151> module151s;
KModuleStateIf *module151state = &module151s;
//--------------------------------------------------
// C-bindings
//--------------------------------------------------
void update_state(tm_151_parameters_t param, uint8_t value, uint8_t pos)/*{{{*/
{
uint32_t relays;

switch(param)
    {
    case TM_151_RELAYS:
	relays = TM_151_EXTRACT_PARAMETER(uint32_t, param)->value();

	if(value == STATE_ON)
	    { relays |=  (1 << (pos - RELAY_BASE)); }
	else
	    { relays &= ~(1 << (pos - RELAY_BASE)); }

	TM_151_EXTRACT_PARAMETER(uint32_t, param)->set_value(relays);

	if(pos == TM_151_RELAY_LPA_POWER)
	    { menu_update(); }
	break;

    case TM_151_LPA_CONNECTION:
	TM_151_EXTRACT_PARAMETER(state_t, param)->set_value((state_t)value);
	if(value == STATE_ON)
	    { printf("ControlLpa.conn.\n"); }
	else
	    { printf("ControlLpa.disc.\n"); }
	menu_update();
	break;

    case TM_151_EMU_U_OFFSET:
    case TM_151_EMU_U_FINE:
    case TM_151_EMU_U_MIDDLE:
    case TM_151_EMU_U_ROUGH:
	TM_151_EXTRACT_PARAMETER(uint8_t, param)->set_value(value);
	break;
    };
}/*}}}*/
//--------------------------------------------------
state_t get_lpa_power()/*{{{*/
{
uint32_t relays;

relays = TM_151_EXTRACT_PARAMETER(uint32_t, TM_151_RELAYS)->value();
if( (relays & (1 << (TM_151_RELAY_LPA_POWER - RELAY_BASE))) == 0)
   { return STATE_OFF; }
else
   { return STATE_ON; }
}/*}}}*/
//--------------------------------------------------
state_t get_lpa_connection()
{ return TM_151_EXTRACT_PARAMETER(state_t, TM_151_LPA_CONNECTION)->value(); }
//--------------------------------------------------
void print_state(void)/*{{{*/
{
uint32_t relays;

for(uint8_t param = TM_151_PARAMETER_BASE; param < TM_151_PARAMETER_COUNT; param++)
    {
    switch((tm_151_parameters_t)param)
	{
	case TM_151_RELAYS:
	    relays = TM_151_EXTRACT_PARAMETER(uint32_t, param)->value();
	    for(uint8_t i=0; i<31; i++)
		{
		if(relays & (1<<i))
		    { printf("on."); }
		else
		    { printf("off."); }
		}
	    break;

	case TM_151_LPA_CONNECTION:
	    if(TM_151_EXTRACT_PARAMETER(state_t, param)->value() == STATE_ON)
		{ printf("conn."); }
	    else
		{ printf("disc."); }
	    break;

	case TM_151_EMU_U_OFFSET:
	    printf("%02X.", TM_151_EXTRACT_PARAMETER(uint8_t, param)->value());
	    break;

	case TM_151_EMU_U_FINE:
	    printf("%02X.", TM_151_EXTRACT_PARAMETER(uint8_t, param)->value());
	    break;

	case TM_151_EMU_U_MIDDLE:
	    printf("%02X.", TM_151_EXTRACT_PARAMETER(uint8_t, param)->value());
	    break;

	case TM_151_EMU_U_ROUGH:
	    printf("%02X.", TM_151_EXTRACT_PARAMETER(uint8_t, param)->value());
	    break;
	};
    }

printf("\n");
}/*}}}*/
//--------------------------------------------------
