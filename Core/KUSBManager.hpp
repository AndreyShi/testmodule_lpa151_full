//--------------------------------------------------
// Менеджер общения через USB с отладочным терминалом
//--------------------------------------------------
#ifndef TM_151_USB_MANAGER_HPP
#define TM_151_USB_MANAGER_HPP

//--------------------------------------------------
#include <stdint.h>

//--------------------------------------------------
class KModuleCommand;

//--------------------------------------------------
class KUSBManager
{
public:
    explicit KUSBManager(const uint32_t buff_size = 0);
    virtual ~KUSBManager();

    void step();

protected:
    enum state_t {
	STATE_IDLE,
	STATE_SYMBOL,
	STATE_BYTE,
	STATE_CMD
    };

    void shift_buffer(uint32_t shift = 1);

private:
    char *m_buff;
    char sym1, sym2;
    uint32_t m_buff_size;

    uint32_t m_cur;

    state_t m_state;
    KModuleCommand *m_cmd;
};
//--------------------------------------------------
extern KUSBManager *U;

//--------------------------------------------------
#endif
