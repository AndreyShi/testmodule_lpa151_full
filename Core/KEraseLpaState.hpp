//--------------------------------------------------
// Состояния, через которые проходит стирание памяти
// барьера
//--------------------------------------------------
#ifndef TM_151_ERASE_LPA_STATE_HPP
#define TM_151_ERASE_LPA_STATE_HPP

//--------------------------------------------------
#include "commands.h"
#include "common.h"
//--------------------------------------------------
#define DEFINE_ERASE_CLASS(name) \
    class KEraseLpa##name : public KEraseLpaState \
    { protected: virtual cmd_status_t functor(); };
//--------------------------------------------------
class KEraseLpaIdle;
class KEraseLpaEnter;
class KEraseLpaSigCheck;
class KEraseLpaChipErase;
class KEraseLpaExit;
//--------------------------------------------------
class KEraseLpaState
{
public:
    KEraseLpaState();
    virtual ~KEraseLpaState();

    cmd_status_t operator()();

    cmd_status_t result() const;

protected:
    virtual cmd_status_t functor() = 0;
    void set_result(cmd_status_t);

    void to_idle();
    void to_enter();
    void to_sig_check();
    void to_chip_erase();
    void to_exit();

private:
    static cmd_status_t m_result;
    static KEraseLpaState *m_current_state;

    KEraseLpaIdle      *idle;
    KEraseLpaEnter     *enter;
    KEraseLpaSigCheck  *sig_check;
    KEraseLpaChipErase *chip_erase;
    KEraseLpaExit      *exit;
};
//--------------------------------------------------
DEFINE_ERASE_CLASS(Idle)
DEFINE_ERASE_CLASS(Enter)
DEFINE_ERASE_CLASS(SigCheck)
DEFINE_ERASE_CLASS(ChipErase)
DEFINE_ERASE_CLASS(Exit)
//--------------------------------------------------
extern KEraseLpaState *ers_idle;
//--------------------------------------------------
#endif
