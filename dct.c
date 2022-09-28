#include "contiki.h"

#include "sys/log.h"
#define LOG_MODULE "DCT"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dct.h"

// static void write_dt_of_dct_and_idct_to_file(const char *filename, clock_time_t dt_compression, clock_time_t dt_decompression) {
static void write_dt_of_dct_and_idct_to_file( clock_time_t dt_compression, clock_time_t dt_decompression) {
	// int fd = cfs_open(filename,CFS_APPEND);
	// cfs_write(fd, sprintf("%ld %ld\n", dt_compression, dt_decompression));
	// cfs_close(fd);
	LOG_INFO("timestamp: %ld %ld", dt_compression, dt_decompression);
}
static void write_decompressed_signal_to_file( signal_t signal) {
	// int fd = cfs_open(filename,CFS_WRITE);
	// cfs_write(fd, (const void *) signal, sizeof(signal_t));
	// cfs_close(fd);
	LOG_INFO("decompressed_signal: ");
	for (int i = 0; i < SIGNAL_LENGTH; ++i) {
		LOG_INFO_("%f ", (double) signal[i]);
	}
	LOG_INFO_("\n");
}

PROCESS(assignment, "Discreet Cosine Transform Process");
AUTOSTART_PROCESSES(&assignment);

PROCESS_THREAD(assignment, ev, data) {
	static struct etimer periodic_timer;

	PROCESS_BEGIN();

	clock_init(); // init timer lib

	etimer_set(&periodic_timer, CLOCK_SECOND * 10);

	clock_time_t t_start, t_end, dt_compression, dt_decompression; 

	LOG_INFO("Waiting for timer to timeout\n");
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
	LOG_INFO("Timer expired\n");

	// FIX: it crashes after this line
	LOG_INFO("Applying DCT to signal\n");
	t_start = clock_time();
	dct(test_signal); // modifies compressed_signal
	t_end = clock_time();
	dt_compression = t_end - t_start;
	LOG_INFO("DCT complete\n");

	LOG_INFO("Applying IDCT to compressed signal\n");
	t_start = clock_time();
	idct(compressed_signal);// modifies decompressed_signal
	t_end = clock_time();
	dt_decompression = t_end - t_start;
	LOG_INFO("IDCT complete\n");

	// write_decompressed_signal_to_file(sprintf("decompressed_signal_N_%d_L_%d_M_%d_%d.dat", SIGNAL_LENGTH, L, M, count), signal);
	LOG_INFO("Writing compressed signal to console:\n");
	write_decompressed_signal_to_file(decompressed_signal);
	LOG_INFO("Writing dt of compression and decompression signal to console:\n");
	write_dt_of_dct_and_idct_to_file( dt_compression, dt_decompression);

	etimer_reset(&periodic_timer);

	PROCESS_END();
}
