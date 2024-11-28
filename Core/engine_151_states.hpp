//--------------------------------------------------
// Уточнение поведения движка для модуля калибровки
// ЛПА-151
//--------------------------------------------------
#ifndef TESTMODULE_151_ENGINE_STATES_HPP
#define TESTMODULE_151_ENGINE_STATES_HPP

//--------------------------------------------------
#include <stdint.h>

#include "engine_states.cpp"

//--------------------------------------------------
class K151Idle : public KEngineState<ENGINE_IDLE>
{
public:
    virtual cmd_status_t load_script(script_id_t) const;

protected:
    virtual bool command_done(cmd_id_t cmd) const;
    virtual bool command_repeated(cmd_id_t cmd) const;
    virtual bool command_failed(cmd_id_t cmd) const;

    virtual void state_exit() const;
};
//--------------------------------------------------
class K151Executing : public KEngineState<ENGINE_EXECUTING>
{
public:
protected:
    virtual void indicate() const;

    virtual void state_enter() const;
    virtual void state_exit() const;
};
//--------------------------------------------------
class K151Done : public KEngineState<ENGINE_EXECUTION_DONE>
{
public:
    virtual cmd_status_t load_script(script_id_t) const;
};
//--------------------------------------------------
class K151Failed : public KEngineState<ENGINE_EXECUTION_FAILED>
{
public:
    virtual cmd_status_t load_script(script_id_t) const;

protected:
    virtual void state_enter() const;
    virtual void state_exit() const;
};
//--------------------------------------------------
class K151Error : public KEngineState<ENGINE_EXECUTION_ERROR>
{
public:
    virtual cmd_status_t load_script(script_id_t) const;

protected:
    virtual void indicate() const;

    virtual void state_enter() const;
};
//--------------------------------------------------
class K151Overflow : public KEngineState<ENGINE_EXECUTION_OVERFLOW>
{
public:
    virtual cmd_status_t load_script(script_id_t) const;
};
//--------------------------------------------------
#endif
