#ifndef __TIMER_H__
#define __TIMER_H__

void timer_init();
void timer_deinit();

uint64_t timer_timestamp();

void timer_timeout(uint64_t usec, void (* cb)());

#endif