#include "contiki.h"

#include "sys/log.h"
#define LOG_MODULE "DCT"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dct.h"

PROCESS(assignment, "assignment");
AUTOSTART_PROCESSES(&assignment);

PROCESS_THREAD(assignment, ev, data) {

	PROCESS_BEGIN();

	clock_init(); // init timer lib

	clock_time_t t_start, t_end, dt_compression ; 

	t_start = clock_time();
	dct(test_signal); // modifies compressed_signal
	t_end = clock_time();
	dt_compression = t_end - t_start;

	// LOG_INFO("COMPRESSED DATA:");
	// afaik contiki-ng does not implement float fmt print,
	// so we have to take each data point and scale it by a 
	// large integer factor (e.g. 100) to "move" the fractional part
	// into the integral part, which we can then print
	for (int i = 0; i < COMPRESED_DATA_LENGTH; ++i) {
		float xi = compressed_signal[i];
		int integral = (int) xi; 
		float fractional = (xi - integral) * 100; 
		LOG_INFO_("%d.%03u ", integral, (unsigned int) fractional);
	}

	LOG_INFO("\n");
	LOG_INFO("DT in clock ticks: %ld\n", dt_compression);
	LOG_INFO("arch platforms CLOCK_SECOND: %ld\n", CLOCK_SECOND);
	LOG_INFO("DT in seconds: %ld\n", dt_compression / CLOCK_SECOND);

	PROCESS_END();
}
