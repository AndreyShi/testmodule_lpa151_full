//--------------------------------------------------
#include "module_types.hpp"

#include "KBusManager.hpp"
#include "bus_frames.hpp"

//--------------------------------------------------
bool KNumeratorFrame::set_accepted(uint8_t)
{ return true; }
//--------------------------------------------------
bool KNumeratorFrame::operator()(uint8_t bus_address)/*{{{*/
{
uint8_t new_addr;

new_addr = header()->num_dests() + 1;
if(new_addr > 0 && new_addr <= ADDR_MAX)
    {
    M->m_bus_address = new_addr;
    header()->set_accepted(new_addr);
    header()->set_executed(new_addr);

    if(m_types.value().size() < ADDR_MAX)
	{ m_types.value().push_back(MODULE_LPA_151); }
    }

M->enqueue_frame(this);
return false;
}/*}}}*/
//--------------------------------------------------
bool KCommandFrame::operator()(uint8_t bus_address)/*{{{*/
{
if( !m_cmd )
    {
    header()->set_failed(bus_address);
    M->enqueue_frame(this);
    return false;
    }

switch( (*E)(m_cmd) )
    {
    case CMD_STATUS_FAILED:
	header()->set_failed(bus_address);
	// fall-through
    case CMD_STATUS_DONE:
	header()->set_executed(bus_address);
	break;

    default:
	break;
    };

M->enqueue_frame(this);
return false;
}/*}}}*/
//--------------------------------------------------
bool KResetFrame::operator()(uint8_t bus_address)/*{{{*/
{
header()->set_executed(bus_address);
M->enqueue_frame(this);
return false;
}/*}}}*/
//--------------------------------------------------
bool KInfoFrame::operator()(uint8_t)
{ return true; }
//--------------------------------------------------
bool KTestStateFrame::operator()(uint8_t)
{ return true; }
//--------------------------------------------------
bool KModuleStateFrame::operator()(uint8_t)
{ return true; }
//--------------------------------------------------
bool KStatusFrame::operator()(uint8_t)
{ return true; }
//--------------------------------------------------
bool KNumRequestFrame::operator()(uint8_t)/*{{{*/
{
M->enqueue_frame(this);
return false;
}/*}}}*/
//--------------------------------------------------
bool KMeterDataFrame::operator()(uint8_t)
{ return true; }
//--------------------------------------------------
