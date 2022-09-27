#include "contiki.h"
#include "cfs/cfs.h"
#include "cfs/cfs-coffee.h"

#include "sys/log.h"
#define LOG_MODULE "DCT"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dct.h"

static void write_dt_of_dct_and_idct_to_file(const char *filename, clock_time_t dt_compression, clock_time_t dt_decompression) {
	// int fd = cfs_open(filename,CFS_APPEND);
	// cfs_write(fd, sprintf("%ld %ld\n", dt_compression, dt_decompression));
	// cfs_close(fd);
	LOG_INFO("timestamp: %ld %ld", dt_compression, dt_decompression);
}
static void write_decompressed_signal_to_file(const char *filename, signal_t signal) {
	// int fd = cfs_open(filename,CFS_WRITE);
	// cfs_write(fd, (const void *) signal, sizeof(signal_t));
	// cfs_close(fd);
	LOG_INFO("decompressed_signal: ");
	for (int i = 0; i < N; ++i) {
		LOG_INFO_("%f ", signal[i]);
	}
	LOG_INFO_("\n");
}

static dct_comprssed_signal_t y;

dct_comprssed_signal_t dct(signal_t signal) {
	dct_block_t xi;
	dct_block_t yi;
	compressed_dct_block_t yM;

	const uint8_t chunk_size_in_bytes = L * sizeof(float);
	const uint8_t compressed_chunk_size_in_bytes = M * sizeof(float);

	// iterate over each chunck xi of size L of the signal
	// and transform it.
	for (int i = 0; i < N; i += L) {
		// xi = signal[i:(i + L)]
		memcpy(xi, signal + i, chunk_size_in_bytes);
		// yi = H * xi
		matmul(H, xi, yi);
		// yM = yi[:M]
		memcpy(yM, yi, compressed_chunk_size_in_bytes);
		// y[M * (i / L):(M * (i / L) + M)] = yM
		// TODO: is the address offset correct
		memcpy(y + M * (i / L), yM, compressed_chunk_size_in_bytes);
	}

	return y;
}

PROCESS(dct, "Discreet Cosine Transform Process");
AUTOSTART_PROCESSES(&dct);

PROCESS_THREAD(dct, ev, data) {
	static struct etimer periodic_timer;

	PROCESS_BEGIN();

	clock_init(); // init timer lib

	etimer_set(&periodic_timer, CLOCK_SECOND * 10);

	uint16_t count = 0;
	clock_time_t t_start, t_end, dt_compression, dt_decompression; 

	while (1) {
		count++;
		LOG_INFO("Waiting for timer to timeout\n");
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
		LOG_INFO("Timer expired\n");

		LOG_INFO("Applying DCT to signal\n");
		t_start = clock_time();
		dct_comprssed_signal_t decompressed_signal = dct(test_signal);
		t_end = clock_time();
		dt_compression = t_end - t_start;
		LOG_INFO("DCT complete\n");

		LOG_INFO("Applying IDCT to compressed signal\n");
		t_start = clock_time();
		signal_t signal = idct(decompressed_signal);
		t_end = clock_time();
		dt_decompression = t_end - t_start;
		LOG_INFO("IDCT complete\n");

		write_decompressed_signal_to_file(sprintf("decompressed_signal_N_%d_L_%d_M_%d_%d.dat", N, L, M, count), signal);
		write_dt_of_dct_and_idct_to_file(sprintf("dt_of_dct_and_icdt_N_%d_L_%d_M_%d_%d.dat", N, L, M, count), dt_compression, dt_decompression);

		etimer_reset(&periodic_timer);
	}

	PROCESS_END();
}
