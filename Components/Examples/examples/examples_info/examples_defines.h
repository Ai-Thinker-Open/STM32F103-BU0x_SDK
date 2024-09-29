#ifndef TESTS_DEFINES_
#define TESTS_DEFINES_

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_DEMO		0

#if(EXAMPLE_DEMO)
void build_examples(void);

int read_dev_id(void);
int simple_tx(void);
int tx_timed_sleep(void);
int simple_tx_pdoa(void);
int simple_rx(void);
int simple_rx_pdoa(void);
int tx_wait_resp(void);
int rx_send_resp(void);
int tx_wait_resp_int(void);
int ds_twr_sts_sdc_init(void);
int ds_twr_sts_sdc_resp(void);

#endif
#ifdef __cplusplus
}
#endif


#endif
