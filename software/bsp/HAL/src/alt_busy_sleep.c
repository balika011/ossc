#include <stdint.h>
#include "priv/alt_busy_sleep.h"
#include "timer.h"

unsigned int __attribute__((noinline, __section__(".rtext"))) alt_busy_sleep(unsigned int us)
{
	uint64_t start_ts = timer_timestamp();

	while (timer_timestamp() < start_ts + us)
		;

	return 0;
}
