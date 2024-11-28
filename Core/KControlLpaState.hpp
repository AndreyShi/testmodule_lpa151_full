//--------------------------------------------------
// Состояния, через которые проходят все процессы
// управления подключенным барьером
//--------------------------------------------------
#ifndef TM_151_CONTROL_LPA_STATE_HPP
#define TM_151_CONTROL_LPA_STATE_HPP

//--------------------------------------------------
#include <string>

#include "commands.h"
#include "common.h"
//--------------------------------------------------
#define DEFINE_CONTROL_CLASS(name) \
    class KControlLpa##name : public KControlLpaState \
    { \
    protected: \
	virtual void do_lpa_ready(); \
	virtual void do_lpa_disconnected(); \
	virtual cmd_status_t functor(const std::string &); \
    };
//--------------------------------------------------
class KControlLpaIdle;
class KControlLpaReady;

class KControlLpaPoll;
class KControlLpaReceived;
//--------------------------------------------------
class KControlLpaState
{
public:
    KControlLpaState();
    virtual ~KControlLpaState();

    static void lpa_ready();
    static void lpa_disconnected();
    cmd_status_t operator()(const std::string &);

protected:
    virtual void do_lpa_ready() = 0;
    virtual void do_lpa_disconnected() = 0;
    virtual cmd_status_t functor(const std::string &) = 0;

    void to_idle();
    void to_ready();
    void to_poll();
    void to_received();

    std::string &u_register() const;

    static uint32_t clock;

private:
    static KControlLpaState *m_current_state;

    KControlLpaIdle  *idle;
    KControlLpaReady *ready;

    KControlLpaPoll     *poll;
    KControlLpaReceived *received;
};

//--------------------------------------------------
DEFINE_CONTROL_CLASS(Idle)
DEFINE_CONTROL_CLASS(Ready)

DEFINE_CONTROL_CLASS(Poll)
DEFINE_CONTROL_CLASS(Received)
//--------------------------------------------------
extern KControlLpaState *ctrl_idle;
//--------------------------------------------------
#endif
