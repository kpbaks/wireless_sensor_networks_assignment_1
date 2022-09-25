#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "sys/log.h"
#define LOG_MODULE "DCT"
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dct.h"

PROCESS(dct, "Discreet Cosine Transform Process");
AUTOSTART_PROCESSES(&dct);

static struct simple_udp_connection udp_conn;

static float y[COMPRESED_DATA_LENGTH];
static uint8_t tx_buffer[MSGLEN] = {0};

float *dct(float signal[N]) {
  float xi[L];
  float yi[L];
  float yM[M];
  // iterate over each chunck xi of size L of the signal
  // and transform it.
  for (int i = 0; i < N; i += L) {
    // xi = signal[i:(i + L)]
    memcpy(xi, signal + i, L * sizeof(float));
    // yi = H * xi
    matmul(H, xi, yi);
    // yM = yi[:M]
    memcpy(yM, yi, M * sizeof(float));
    // y[M * (i / L):(M * (i / L) + M)] = yM
    memcpy(y + M * (i / L), yM, M * sizeof(float));
  }

  return y;
}

PROCESS_THREAD(dct, ev, data) {
  static struct etimer periodic_timer;
  static uint32_t tx_count = 0;
  static uint32_t missed_tx_count = 0;
  uip_ipaddr_t dest_ipaddr;

  PROCESS_BEGIN();

  etimer_set(&periodic_timer, CLOCK_SECOND * 10);

  while (1) {

    LOG_INFO("Waiting for timer to timeout\n");

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    LOG_INFO("Timer expired\n");

    if (NETSTACK_ROUTING.node_is_reachable() &&
        NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

      // do dct
      LOG_INFO("Applying DCT to signal\n");

      dct(test_signal);

      { // use a loop to send using UDP
        uint8_t packets_to_send = COMPRESED_DATA_LENGTH_AS_BYTES / MSGLEN;
        for (int i = 0; i < packets_to_send; ++i) {
          int offset = i * MSGLEN / sizeof(float);

          for (int j = 0; j < MSGLEN / sizeof(float); ++j) {
            memcpy(&tx_buffer[sizeof(float) * j], &y[j + offset],
                   sizeof(float));
          }
          LOG_INFO("Sending udp packet: %d\n", i);

          simple_udp_sendto(&udp_conn, &tx_buffer, MSGLEN, &dest_ipaddr);
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
