#include "contiki.h"
#include "net/ipv6/simple-udp.h"

#include "net/netstack.h"

#include "sys/log.h"
#define LOG_MODULE "DCT"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dct.h"

static struct simple_udp_connection udp_conn;

static float y[COMPRESED_DATA_LENGTH];
static uint8_t tx_buffer[UDP_MSGLEN] = {0};

float *dct(float signal[N]) {
	// float xi[L];
	dct_block_t xi;
	// float yi[L];
	dct_block_t yi;
	// float yM[M];
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
		memcpy(y + M * (i / L), yM, M * sizeof(float));
	}

	return y;
}

static void udp_rx_callback(
	struct simple_udp_connection *c,
	const uip_ipaddr_t *sender_addr,
	uint16_t sender_port,
	const uip_ipaddr_t *receiver_addr,
	uint16_t receiver_port,
	const uint8_t *data,
	uint16_t datalen)
{

}


PROCESS(dct, "Discreet Cosine Transform Process");
AUTOSTART_PROCESSES(&dct);


PROCESS_THREAD(dct, ev, data) {
	static struct etimer periodic_timer;
	static uint32_t tx_count = 0;
	static uint32_t missed_tx_count = 0;
	uip_ipaddr_t dest_ipaddr;

	PROCESS_BEGIN();

	simple_udp_register(&udp_conn, UDP_CLIENT_PORT , NULL, UDP_SERVER_PORT, udp_rx_callback);

	etimer_set(&periodic_timer, CLOCK_SECOND * 10);

	while (1) {

		LOG_INFO("Waiting for timer to timeout\n");

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
		LOG_INFO("Timer expired\n");

		// TODO: is this the right way to do it?
		if (NETSTACK_ROUTING.node_is_reachable() &&
			NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

			// do dct
			LOG_INFO("Applying DCT to signal\n");

			dct(test_signal);

			{ // use a loop to send using UDP
				uint8_t packets_to_send = COMPRESED_DATA_LENGTH_AS_BYTES / UDP_MSGLEN;
				for (int i = 0; i < packets_to_send; ++i) {
					int offset = i * UDP_MSGLEN / sizeof(float);

					for (int j = 0; j < UDP_MSGLEN / sizeof(float); ++j) {
						memcpy(&tx_buffer[sizeof(float) * j], &y[j + offset],
			 sizeof(float));
					}
					LOG_INFO("Sending udp packet: %d\n", i);

					simple_udp_sendto(&udp_conn, &tx_buffer, UDP_MSGLEN, &dest_ipaddr);
				}
			}

			LOG_INFO("Finished sending udp packets\n");

			tx_count++;

		} else {
			LOG_INFO("Not reachable yet\n");
			if (tx_count > 0) {
				missed_tx_count++;
			}
		}

		etimer_reset(&periodic_timer);
	}

	PROCESS_END();
}
