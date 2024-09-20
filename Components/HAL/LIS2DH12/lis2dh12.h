#ifndef __LIS2DH12_H__
#define __LIS2DH12_H__

#include <stdint.h>
#define LIS2DH12_FROM_FS_2g_HR_TO_mg(lsb)  (float)((int16_t)lsb>>4)* 1.0f
#define LIS2DH12_FROM_FS_4g_HR_TO_mg(lsb)  (float)((int16_t)lsb>>4)* 2.0f
#define LIS2DH12_FROM_FS_8g_HR_TO_mg(lsb)  (float)((int16_t)lsb>>4)* 4.0f
#define LIS2DH12_FROM_FS_16g_HR_TO_mg(lsb) (float)((int16_t)lsb>>4)*12.0f
#define LIS2DH12_FROM_LSB_TO_degC_HR(lsb)  (float)((int16_t)lsb>>6)/4.0f+25.0f

int32_t HalIis2dh12Init();
int drv_lis2dh12_get_angle (float* get);

#endif /* __LIS2DH12_H__ */