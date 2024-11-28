//--------------------------------------------------
// Module LPA-151 parameters
//--------------------------------------------------
#ifndef LPA_151_STATE_PARAMETERS_H
#define LPA_151_STATE_PARAMETERS_H

//--------------------------------------------------
typedef enum tm_151_parameters {
    TM_151_PARAMETER_BASE = 0,

    TM_151_RELAYS = 0,
    TM_151_LPA_CONNECTION,
    TM_151_EMU_U_OFFSET,
    TM_151_EMU_U_FINE,
    TM_151_EMU_U_MIDDLE,
    TM_151_EMU_U_ROUGH,

    TM_151_PARAMETER_COUNT
} tm_151_parameters_t;

//--------------------------------------------------
#endif
