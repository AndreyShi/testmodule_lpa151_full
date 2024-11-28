//--------------------------------------------------
// Module control commands
//--------------------------------------------------
#include <stdio.h>

#include "module_commands.hpp"
#include "KUpdateLpaState.hpp"
#include "KEraseLpaState.hpp"
#include "KControlLpaState.hpp"

//--------------------------------------------------
static KUpdateLpaState  *update_state  = upd_idle;
static KEraseLpaState   *erase_state   = ers_idle;
static KControlLpaState *control_state = ctrl_idle;

//--------------------------------------------------
cmd_status_t KControlLpaCmd::operator()()
{ return (*control_state)(data()); }
//--------------------------------------------------
cmd_status_t KUpdateLpaCmd::operator()()
{ return (*update_state)(data_id()); }
//--------------------------------------------------
cmd_status_t KEraseLpaCmd::operator()()
{ return (*erase_state)(); }
//--------------------------------------------------
