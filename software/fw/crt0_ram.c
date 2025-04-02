#include <stdint.h>

extern uint8_t _siram; // 10200000
extern uint8_t _eiram; // 10208000

extern uint8_t _frtext; // 10085ECE
extern uint8_t _srtext; // 10200000
extern uint8_t _ertext; // 102003C6

extern uint8_t _fdata; // 1008882C
extern uint8_t _sdata; // 102003C8
extern uint8_t _edata; // 10200EF8

static void copy(uint8_t *from, uint8_t *to, uint8_t *end)
{
	while (to != end)
	{
		*to++ = *from++;
	}
}

void init_ram()
{
	copy(&_frtext, &_srtext, &_ertext);
	copy(&_fdata, &_sdata, &_edata);
}