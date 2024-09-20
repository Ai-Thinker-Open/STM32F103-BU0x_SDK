#ifndef _SHARE_FUNC_
#define _SHARE_FUNC_

#ifdef __cplusplus
extern "C" {
#endif
#include "default_config.h"
#include "stdbool.h"
#define FILTER_SIZE 30  //NOTE: filter size needs to be > 2

typedef struct
{
    //motion filter
    bool motionFilterReady; //set to true when enough data accumulated in the filter array
    int filterHisIdx;
    double estXHis[FILTER_SIZE];
    double estYHis[FILTER_SIZE];
    double estAngleHis[FILTER_SIZE];
} tag_reports_t;

	
/* */
typedef struct
{
    uint16_t    addr16;         //标签地址
    uint8_t     rangeNum;       //标签测距序列号 number from Tag Poll and Final messages, which indicates the current range number
    uint32_t    resTime_us;     //标签测距时间 reception time of the end of the Final from the Tag wrt node's SuperFrame start, microseconds
    float       pdoa_raw_deg;   //标签相位差 pdoa_raw: phase differences in degrees without any correction [-180 .. 180]
    float       dist_cm;        //标签距离值 distance to the tag in cm
    float       x_cm;           //标签x轴坐标 X of the tag wrt to the node, cm
    float       y_cm;           //标签y轴坐标 Y of the tag wrt to the node, cm
    float       angle;			//标签角度值 angle of the tag 	
    float       clockOffset_pphm;//clock offset in hundredths of ppm (i.e. 1ppm is 100)
    uint16_t    flag;           //service message data from the tag (low byte) and node (high byte), bitmask (defined as "RES_FLAG_")
    int16_t     acc_x;          //Normalized accel data X from the Tag, mg: acc_x
    int16_t     acc_y;          //Normalized accel data Y from the Tag, mg: acc_y
    int16_t     acc_z;          //Normalized accel data Z from the Tag, mg: acc_z
}result_pdoa_t;	

	
/*! ------------------------------------------------------------------------------------------------------------------
 * @fn check_for_status_errors()
 *
 * @brief This function is used to get a value to increase the delay timer by dependent on the current TX preamble length set.
 *
 * @param reg: uint32_t value representing the current status register value.
 * @param errors: pointer to a uint32_t buffer that contains the sum of different errors logged during program operation.
 *
 * @return none
 */
void check_for_status_errors(uint32_t reg, uint32_t * errors);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn get_rx_delay_time_txpreamble()
 *
 * @brief This function is used to get a value to increase the delay timer by dependent on the current TX preamble length set.
 *
 * @param None
 *
 * @return delay_time - a uint32_t value indicating the required increase needed to delay the time by.
 */
uint32_t get_rx_delay_time_txpreamble(void);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn get_rx_delay_time_data_rate()
 *
 * @brief This function is used to get a value to increase the delay timer by dependent on the current data rate set.
 *
 * @param None
 *
 * @return delay_time - a uint32_t value indicating the required increase needed to delay the time by.
 */
uint32_t get_rx_delay_time_data_rate(void);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn set_delayed_rx_time()
 *
 * @brief This function is used to set the delayed RX time before running dwt_rxenable()
 *
 * @param delay - This is a defined delay value (usually POLL_TX_TO_RESP_RX_DLY_UUS)
 * @param config_options - pointer to dwt_config_t configuration structure that is in use at the time this function
 *                         is called.
 *
 * @return None
 */
void set_delayed_rx_time(uint32_t delay, dwt_config_t *config_options);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn set_resp_rx_timeout()
 *
 * @brief This function is used to set the RX timeout value
 *
 * @param delay - This is a defined delay value (usually RESP_RX_TIMEOUT_UUS)
 * @param config_options - pointer to dwt_config_t configuration structure that is in use at the time this function
 *                         is called.
 *
 * @return None
 */
void set_resp_rx_timeout(uint32_t delay, dwt_config_t *config_options);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn resync_sts()
 *
 * @brief Resync the current device's STS value the given value
 *
 * @param newCount - The 32 bit value to set the STS to.
 *
 * @return None
 */
void resync_sts(uint32_t newCount);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn resp_msg_get_ts()
 *
 * @brief Read a given timestamp value from the response message. In the timestamp fields of the response message, the
 *        least significant byte is at the lower address.
 *
 * @param  ts_field  pointer on the first byte of the timestamp field to get
 *         ts  timestamp value
 *
 * @return none
 */
void resp_msg_get_ts(uint8_t *ts_field, uint32_t *ts);
uint64_t get_tx_timestamp_u64(void);
uint64_t get_rx_timestamp_u64(void);
void final_msg_get_ts(const uint8_t *ts_field, uint32_t *ts);
void final_msg_set_ts(uint8_t *ts_field, uint64_t ts);
void resp_msg_set_ts(uint8_t *ts_field, const uint64_t ts);
int ds_twr_sts_sdc_initiator(void);
int ds_twr_sts_sdc_responder(void);



#ifdef __cplusplus
}
#endif


#endif
