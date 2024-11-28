//--------------------------------------------------
// Менеджер общения с шиной.
// Разбирает входящие пакеты и управляет адресацией.
//--------------------------------------------------
#ifndef TESTMODULE_151_BUS_MANAGER_HPP
#define TESTMODULE_151_BUS_MANAGER_HPP

//--------------------------------------------------
#include "KAbstractBusManager.hpp"

//--------------------------------------------------
class KBusFrame;

//--------------------------------------------------
class KBusManager : public KAbstractBusManager
{
public:
    KBusManager();

protected:
    virtual void request_address();
    virtual void process_incoming();
    virtual void process_outgoing();

private:
    uint32_t m_clock;

    /* all my friends are here */
    friend class KNumeratorFrame;
};

//--------------------------------------------------
#endif
