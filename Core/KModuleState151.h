//--------------------------------------------------
// Module LPA-151 state
//--------------------------------------------------
#ifndef K_MODULE_STATE_151_H
#define K_MODULE_STATE_151_H

//--------------------------------------------------
#include "common.h"

#include "KModuleState151_parameters.h"

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void update_state(tm_151_parameters_t param, uint8_t value, uint8_t position);
state_t get_lpa_power(void);
state_t get_lpa_connection(void);
void print_state(void);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
