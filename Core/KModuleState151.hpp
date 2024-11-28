//--------------------------------------------------
// Module LPA-151 state
//--------------------------------------------------
#ifndef K_MODULE_STATE_151_HPP
#define K_MODULE_STATE_151_HPP

//--------------------------------------------------
#include "common.h"
#include "KModuleState151_parameters.h"

//--------------------------------------------------
template<>
struct module_info<MODULE_LPA_151> {
    enum {
	count = TM_151_PARAMETER_COUNT,
	max_channels = 2
    };
};

//--------------------------------------------------
extern KModuleStateIf *module151state;

//--------------------------------------------------
ADD_STATE_PARAMETER(MODULE_LPA_151, TM_151_RELAYS, uint32_t)

ADD_STATE_PARAMETER(MODULE_LPA_151, TM_151_LPA_CONNECTION, state_t)

ADD_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_OFFSET, uint8_t)
ADD_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_FINE,   uint8_t)
ADD_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_MIDDLE, uint8_t)
ADD_STATE_PARAMETER(MODULE_LPA_151, TM_151_EMU_U_ROUGH,  uint8_t)
//--------------------------------------------------
#endif
