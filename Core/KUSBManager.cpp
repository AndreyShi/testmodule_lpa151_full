//--------------------------------------------------
// Менеджер общения через USB с отладочным терминалом
//--------------------------------------------------
#include <stdio.h>

#include "KUSBManager.hpp"
#include "module_commands.hpp"

//--------------------------------------------------
KUSBManager *U = 0;

//--------------------------------------------------
KUSBManager::KUSBManager(const uint32_t buff_size) :
	m_buff_size(buff_size),
	m_cur(0),
	m_state(STATE_IDLE),
	m_cmd(0)
{/*{{{*/
if(buff_size == 0)
    { m_buff_size = 2*KModuleCommand::max_size(); }

m_buff = new char[m_buff_size];
}/*}}}*/
//--------------------------------------------------
KUSBManager::~KUSBManager()
{ delete m_buff; }
//--------------------------------------------------
void KUSBManager::shift_buffer(uint32_t shift)/*{{{*/
{
if(shift == 0)
    { shift = 1; }

for(uint32_t i=0; i<m_cur; i++)
    { m_buff[i] = m_buff[shift + i]; }

m_cur -= shift;
}/*}}}*/
//--------------------------------------------------
void KUSBManager::step()/*{{{*/
{
char sym;
char *cur;
int s;
uint32_t sz;

switch(m_state)
    {
    case STATE_IDLE:/*{{{*/
	sym = s = getc(stdin);
	if(s == EOF)
	    { break; }

	if(sym >= '0' && sym <= '9')
	    { sym1 = sym - '0'; }
	else if(sym >= 'a' && sym <= 'f')
	    { sym1 = sym - 'a' + 10; }
	else if(sym >= 'A' && sym <= 'F')
	    { sym1 = sym - 'A' + 10; }
	else if(sym == '\n' || sym == '\r')
	    {
	    m_cur = 0;
	    break;
	    }
	else
	    { break; }

	m_state = STATE_SYMBOL;
	break;
/*}}}*/
    case STATE_SYMBOL:/*{{{*/
	sym = s = getc(stdin);
	if(s == EOF)
	    { break; }

	if(sym >= '0' && sym <= '9')
	    { sym2 = sym - '0'; }
	else if(sym >= 'a' && sym <= 'f')
	    { sym2 = sym - 'a' + 10; }
	else if(sym >= 'A' && sym <= 'F')
	    { sym2 = sym - 'A' + 10; }
	else if(sym == '\n' || sym == '\r')
	    {
	    m_cur = 0;
	    m_state = STATE_IDLE;
	    break;
	    }
	else
	    {
	    m_state = STATE_IDLE;
	    break;
	    }

	m_state = STATE_BYTE;
	break;
/*}}}*/
    case STATE_BYTE:/*{{{*/
	sym1 = (sym1 & 0x0F) << 4;
	sym2 = (sym2 & 0x0F);

	sym = sym1 | sym2;

	m_buff[ m_cur ] = sym;
	m_cur++;

	m_state = STATE_CMD;
	break;
/*}}}*/
    case STATE_CMD:/*{{{*/
	if(m_cur >= m_buff_size)
	    {
	    printf("failed.input buffer overflow.\n");
	    m_cur = 0;
	    m_state = STATE_IDLE;
	    break;
	    }

	cur = m_buff;
	sz = m_cur;

	m_cmd = KModuleCommand::deserialize(&cur, sz);
	if(m_cmd != 0)
	    {
	    shift_buffer(sz);
	    printf("%d.accepted.\n", m_cmd->id());
	    (*E)(m_cmd);
            m_cur = 0;
	    }

	m_state = STATE_IDLE;
	break;
/*}}}*/
    };
}/*}}}*/
//--------------------------------------------------
