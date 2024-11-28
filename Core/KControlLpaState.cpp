//--------------------------------------------------
// Состояния, через которые проходят все процессы
// управления подключенным барьером
//--------------------------------------------------
#include "KControlLpaState.h"
#include "KControlLpaState.hpp"

#include "app_export.h"
#include "hid_lpa.h"
#include "gpio_if.h"

#include "KModuleEngine.hpp"

//--------------------------------------------------
// Для начала, связки с USB хостом
//--------------------------------------------------
void lpa_ready(void)
{ KControlLpaState::lpa_ready(); }
//--------------------------------------------------
void lpa_disconnected(void)
{ KControlLpaState::lpa_disconnected(); }
//--------------------------------------------------
// Собственно автомат общения с барьером
//--------------------------------------------------
static KControlLpaIdle  control_idle;
static KControlLpaReady control_ready;

static KControlLpaPoll     control_poll;
static KControlLpaReceived control_received;
//--------------------------------------------------
KControlLpaState *ctrl_idle = &control_idle;
//--------------------------------------------------
KControlLpaState *KControlLpaState::m_current_state = &control_idle;
uint32_t KControlLpaState::clock = 0;
//--------------------------------------------------
KControlLpaState::KControlLpaState() :/*{{{*/
	idle(&control_idle),
	ready(&control_ready),
	poll(&control_poll),
	received(&control_received)
{ }/*}}}*/
//--------------------------------------------------
KControlLpaState::~KControlLpaState()
{ }
//--------------------------------------------------
void KControlLpaState::lpa_ready()/*{{{*/
{
if(m_current_state)
    { m_current_state->do_lpa_ready(); }
}/*}}}*/
//--------------------------------------------------
void KControlLpaState::lpa_disconnected()/*{{{*/
{
if(m_current_state)
    { m_current_state->do_lpa_disconnected(); }
}/*}}}*/
//--------------------------------------------------
cmd_status_t KControlLpaState::operator()(const std::string &lpa_cmd)/*{{{*/
{
if(m_current_state)
    { return m_current_state->functor(lpa_cmd); }

printf("ControlLpa.failed.current state null.\n");
return CMD_STATUS_FAILED;
}/*}}}*/
//--------------------------------------------------
/* transitions here {{{ */
void KControlLpaState::to_idle()
{ m_current_state = idle; }
//--------------------------------------------------
void KControlLpaState::to_ready()
{ m_current_state = ready; }
//--------------------------------------------------
void KControlLpaState::to_poll()
{
clock = HAL_GetTick();
m_current_state = poll;
}
//--------------------------------------------------
void KControlLpaState::to_received()
{ m_current_state = received; }/*}}}*/
//--------------------------------------------------
std::string &KControlLpaState::u_register() const
{ return E->m_u_register; }
//--------------------------------------------------
// Idle - барьер не подключен, работа невозможна {{{
//--------------------------------------------------
void KControlLpaIdle::do_lpa_ready()
{ to_ready(); }
//--------------------------------------------------
void KControlLpaIdle::do_lpa_disconnected()
{ }
//--------------------------------------------------
cmd_status_t KControlLpaIdle::functor(const std::string &)
{
printf("ControlLpa.failed.not connected.\n");
return CMD_STATUS_FAILED;
}/* }}} */
//--------------------------------------------------
// Ready - барьер подключен и готов принимать команды {{{
//--------------------------------------------------
void KControlLpaReady::do_lpa_ready()
{ }
//--------------------------------------------------
void KControlLpaReady::do_lpa_disconnected()
{ to_idle(); }
//--------------------------------------------------
cmd_status_t KControlLpaReady::functor(const std::string &lpa_cmd)
{
USBH_HID_LPASendData((uint8_t *)lpa_cmd.data(), lpa_cmd.size());

to_poll();
return CMD_STATUS_REPEAT;
}/* }}} */
//--------------------------------------------------
// Poll - постоянная проверка готовности данных от барьера {{{
//--------------------------------------------------
void KControlLpaPoll::do_lpa_ready()
{ }
//--------------------------------------------------
void KControlLpaPoll::do_lpa_disconnected()
{ to_idle(); }
//--------------------------------------------------
cmd_status_t KControlLpaPoll::functor(const std::string &)
{
const uint32_t to = 1000;

if(HAL_GetTick() - clock > to)
    {
    printf("ControlLpa.failed.receive timeout.\n");
    to_ready();
    return CMD_STATUS_FAILED;
    }

if(USBH_HID_LPAGetFlag() == 1)
    { to_received(); }

return CMD_STATUS_REPEAT;
}/*}}}*/
//--------------------------------------------------
// Received - данные получены и готовы к обработке {{{
//--------------------------------------------------
void KControlLpaReceived::do_lpa_ready()
{ }
//--------------------------------------------------
void KControlLpaReceived::do_lpa_disconnected()
{ to_idle(); }
//--------------------------------------------------
cmd_status_t KControlLpaReceived::functor(const std::string &)
{
uint8_t *data;
uint8_t sz;

data = (uint8_t *)USBH_HID_LPAGetData();

sz = data[0];
u_register().clear();
u_register().append((char *)data, sz);

printf("ControlLpa.ok.");
if(sz == 0)
    { printf("''"); }
else if(sz == 1)
    { printf("%02X", data[0]); }
else
    {
    printf("%02X", data[0]);
    for(uint8_t i=1; i<sz; i++)
	{ printf(" %02X", data[i]); }
    }
printf(".\n");

to_ready();
return CMD_STATUS_DONE;
}/* }}} */
//--------------------------------------------------
