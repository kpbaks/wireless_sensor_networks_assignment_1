#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
// #include "cfs/cfs.h"
// #include "cfs/cfs-coffee.h"
#include <stdio.h>

#include "sys/log.h"
#define LOG_MODULE "IDCT"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dct.h"

PROCESS(idct, "Inverse Discreet Cosine Transform Process");
AUTOSTART_PROCESSES(&idct);

static struct simple_udp_connection udp_conn;

static uint8_t rx_buffer[MSGLEN] = {0};

// static float y[L * M / N];

// float *idct(dct_comprssed_signal_t signal) {
// 	float xi[L];
// 	float yi[L];
// 	float yM[M];
// 	// iterate over each chunck xi of size L of the signal
// 	// and transform it.
// 	for (int i = 0; i < N; i += L) {
// 		// xi = signal[i:(i + L)]
// 		memcpy(xi, signal + i, L);
// 		// yi = H * xi
// 		matmul(H_inv, xi, yi);
// 		// yM = yi[:M]
// 		memcpy(yM, yi, M);
// 		// y[M * (i / L):(M * (i / L) + M)] = yM
// 		memcpy(y + M * (i / L), yM, M);
// 	}

// 	return y;
// }

static dct_block_t decompressed_block;

static const dct_block_t* idct_a_block(dct_block_t xi) {
	matmul(H_inv, xi, decompressed_block);
	return decompressed_block;
}

static void append_block_to_output_file(const char *filename, dct_block_t x) {
	FILE *f = fopen(filename, 'a');
	if (f != NULL) {
		for (int i = 0; i < L; ++i) {
			fprintf(f, "%.3f\n", x[i]);
		}
	}

	fclose(f);
}

static void udp_rx_callback(struct simple_udp_connection *c,
							const uip_ipaddr_t *sender_addr,
							uint16_t sender_port,
							const uip_ipaddr_t *receiver_addr,
							uint16_t receiver_port, const uint8_t *data,
							uint16_t datalen) {
	unsigned count = *(unsigned *)data;
	LOG_INFO("Received request %u (Max MAC Tx: %d) from ", count,
		  uipbuf_get_attr(UIPBUF_ATTR_MAX_MAC_TRANSMISSIONS));
	LOG_INFO_6ADDR(sender_addr);
	LOG_INFO_("\n");
	LOG_INFO("datalen: %d\n", datalen);

	dct_block_t xi;


	const uint16_t compressed_block_size = M * sizeof(float);


	for(int i =0; i < datalen / compressed_block_size; ++i) {
		int offset = i * sizeof(float);
		// read data in chunks of M into xi
		uint8_t* start = data + compressed_block_size * i;
		memcpy(&xi, start, compressed_block_size);
		// right zero pad xi with L - M zeros
		memset(xi + compressed_block_size, L - M, 0x00);
		// apply idct on xi to get yi
		dct_block_t yi = idct_a_block(xi);
		// when the signal has been decompressed, write it to a file
		append_block_to_output_file("decompressed_signal.dat", yi);
	}
	LOG_INFO("Finished writing decompressed_signal from udp packet to file.\n");

}

PROCESS_THREAD(idct, ev, data) {

	PROCESS_BEGIN();
	// Initialize DAG root
	NETSTACK_ROUTING.root_start();

	// Initialize UDP connection
	simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT,
					 udp_rx_callback);

	PROCESS_END();
}
