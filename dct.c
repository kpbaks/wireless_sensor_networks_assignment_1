#include "contiki.h"

#include "sys/log.h"
#define LOG_MODULE "DCT"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dct.h"

// static void write_dt_of_dct_and_idct_to_file(const char *filename, clock_time_t dt_compression, clock_time_t dt_decompression) {
// static void write_dt_of_dct_and_idct_to_file( clock_time_t dt_compression, clock_time_t dt_decompression) {
// 	// int fd = cfs_open(filename,CFS_APPEND);
// 	// cfs_write(fd, sprintf("%ld %ld\n", dt_compression, dt_decompression));
// 	// cfs_close(fd);
// 	LOG_INFO("timestamp: %ld %ld", dt_compression, dt_decompression);
// }
// static void write_decompressed_signal_to_file( signal_t signal) {
// 	// int fd = cfs_open(filename,CFS_WRITE);
// 	// cfs_write(fd, (const void *) signal, sizeof(signal_t));
// 	// cfs_close(fd);
// 	LOG_INFO("decompressed_signal: ");
// 	for (int i = 0; i < SIGNAL_LENGTH; ++i) {
// 		LOG_INFO_("%f ", (double) signal[i]);
// 	}
// 	LOG_INFO_("\n");
// }

// void log_float(float x) {
// 	int integral = (int) x;
// 	float fractional =  (x - integral) * 100;
// 	LOG_INFO("%d.%03u ", integral, (unsigned int) fractional);
// }


PROCESS(assignment, "Discreet Cosine Transform Process");
AUTOSTART_PROCESSES(&assignment);

PROCESS_THREAD(assignment, ev, data) {
	// static struct etimer periodic_timer;

	PROCESS_BEGIN();

	clock_init(); // init timer lib

	// etimer_set(&periodic_timer, CLOCK_SECOND * 10);

	clock_time_t t_start, t_end, dt_compression, dt_decompression; 

	// LOG_INFO("Waiting for timer to timeout\n");
	// PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
	// LOG_INFO("Timer expired\n");

	// FIX: it crashes after this line
	// LOG_INFO("Applying DCT to signal\n");
	t_start = clock_time();
	dct(test_signal); // modifies compressed_signal
	t_end = clock_time();
	dt_compression = t_end - t_start;

	for (int i = 0; i < COMPRESED_DATA_LENGTH; ++i) {
		float xi = compressed_signal[i];
		int integral = (int) xi; 
		float fractional =  (xi - integral) * 100; 
		LOG_INFO_("%d.%03u ", integral, (unsigned int) fractional);
	}
	// LOG_INFO("DCT complete\n");

	// LOG_INFO("Applying IDCT to compressed signal\n");
	t_start = clock_time();

	// LOG_INFO("DECOMPRESSED SIGNAL:\t");
	// dct_block_t xi;
	// dct_block_t yi;

	// const uint8_t chunk_size_in_bytes = L * sizeof(float);

	// // iterate over each chunck xi of size M of the compressed_signal
	// // and transform it.
	// for (int i = 0; i < COMPRESED_DATA_LENGTH; i += M) {
	// 	// yi[:M] = compressed_signal[i:(i + M)]
	// 	memcpy(yi, compressed_signal + i, chunk_size_in_bytes);
	// 	// pad yi[M:] with zeros
	// 	for (int j = M; j < L; ++j) {
	// 		yi[j] = 0.0f;
	// 	}
	// 	// LOG_INFO("%d", i);
	// 	// xi = H_inv * yi
	// 	matmul(H_inv, yi, xi);
	// 	// print out each chunk in one line to then capture in the console ...
	// 	for (int j = 0; j < L; ++j) {

	// 		// int integral = (int) xi[j]; 
	// 		// float fractional =  (xi[j] - integral) * 100; 
	// 		// LOG_INFO("%d.%03u ", integral, (unsigned int) fractional);
	// 		// LOG_INFO_("%f ", (double) xi[j]);
	// 		// LOG_INFO_("%f ",  xi[j]);
	// 		// printf("%f ", (double) xi[j]);
	// 	}
	// }

	t_end = clock_time();
	dt_decompression = t_end - t_start;
	// LOG_INFO("IDCT complete\n");

	LOG_INFO("TIMESTAMP: %ld %ld", dt_compression, dt_decompression);
	// LOG_INFO("Writing dt of compression and decompression signal to console:\n");

	// etimer_reset(&periodic_timer);

	PROCESS_END();
}
