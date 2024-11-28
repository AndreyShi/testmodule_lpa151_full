//--------------------------------------------------
// Состояния, через которые проходят все процессы
// обновления прошивки барьера
//--------------------------------------------------
#ifndef TM_151_UPDATE_LPA_STATE_HPP
#define TM_151_UPDATE_LPA_STATE_HPP

//--------------------------------------------------
#include "commands.h"
#include "common.h"
//--------------------------------------------------
#define DEFINE_UPDATE_CLASS(name) \
    class KUpdateLpa##name : public KUpdateLpaState \
    { protected: virtual cmd_status_t functor(data_id_t); };
//--------------------------------------------------
class KUpdateLpaIdle;
class KUpdateLpaEnter;
class KUpdateLpaSigCheck;
class KUpdateLpaDispatch;

class KUpdateLpaBurnLock;
class KUpdateLpaBurnFuse;

class KUpdateLpaBurnEEFactory;
class KUpdateLpaBurnEEUser;
class KUpdateLpaVerifyEEFactory;
class KUpdateLpaVerifyEEUser;

class KUpdateLpaBurnFlash;
class KUpdateLpaVerifyFlash;

class KUpdateLpaExit;
//--------------------------------------------------
class KUpdateLpaState
{
public:
    KUpdateLpaState();
    virtual ~KUpdateLpaState();

    cmd_status_t operator()(data_id_t);

    cmd_status_t result() const;

protected:
    virtual cmd_status_t functor(data_id_t) = 0;
    void set_result(cmd_status_t);

    void to_idle();
    void to_enter();
    void to_sig_check();
    void to_dispatch();

    void to_burn_lock();
    void to_burn_fuse();

    void to_burn_ee_factory();
    void to_burn_ee_user();
    void to_verify_ee_factory();
    void to_verify_ee_user();

    void to_burn_flash();
    void to_verify_flash();

    void to_exit();

    static uint8_t f[2];

private:
    static cmd_status_t m_result;
    static KUpdateLpaState *m_current_state;

    KUpdateLpaIdle     *idle;
    KUpdateLpaEnter    *enter;
    KUpdateLpaSigCheck *sig_check;
    KUpdateLpaDispatch *dispatch;

    KUpdateLpaBurnLock *burn_lock;
    KUpdateLpaBurnFuse *burn_fuse;

    KUpdateLpaBurnEEFactory   *burn_ee_factory;
    KUpdateLpaBurnEEUser      *burn_ee_user;
    KUpdateLpaVerifyEEFactory *verify_ee_factory;
    KUpdateLpaVerifyEEUser    *verify_ee_user;

    KUpdateLpaBurnFlash   *burn_flash;
    KUpdateLpaVerifyFlash *verify_flash;

    KUpdateLpaExit *exit;
};
//--------------------------------------------------
DEFINE_UPDATE_CLASS(Idle)

DEFINE_UPDATE_CLASS(Enter)
DEFINE_UPDATE_CLASS(SigCheck)
DEFINE_UPDATE_CLASS(Dispatch)

DEFINE_UPDATE_CLASS(BurnLock)
DEFINE_UPDATE_CLASS(BurnFuse)

DEFINE_UPDATE_CLASS(BurnEEFactory)
DEFINE_UPDATE_CLASS(BurnEEUser)
DEFINE_UPDATE_CLASS(VerifyEEFactory)
DEFINE_UPDATE_CLASS(VerifyEEUser)

DEFINE_UPDATE_CLASS(BurnFlash)
DEFINE_UPDATE_CLASS(VerifyFlash)

DEFINE_UPDATE_CLASS(Exit)
//--------------------------------------------------
extern KUpdateLpaState *upd_idle;
//--------------------------------------------------
#endif
