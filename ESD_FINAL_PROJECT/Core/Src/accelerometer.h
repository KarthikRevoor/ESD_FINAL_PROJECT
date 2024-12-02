/*
 * accelerometer.h
 *
 *  Created on: Dec 2, 2024
 *      Author: karth
 */

#ifndef SRC_ACCELEROMETER_H_
#define SRC_ACCELEROMETER_H_
#include "uart_init.h"
#define DEV_ADD 0x68

#define FS_GYRO_250 0
#define FS_GYRO_500 8
#define FS_GYRO_1000 9
#define FS_GYRO_2000 10

#define FS_ACC_2G 0
#define FS_ACC_4G 8
#define FS_ACC_8G 9
#define FS_ACC_16G 10

#define REG_CONFIG_GYRO 27
#define REG_CONFIG_ACC 28
#define REG_USR_CTRL 107
#define REG_DATA 59

void MPU_init(void);
int32_t MPU_read();
uint32_t detect_shakes(uint32_t duration, int threshold);

#endif /* SRC_ACCELEROMETER_H_ */
