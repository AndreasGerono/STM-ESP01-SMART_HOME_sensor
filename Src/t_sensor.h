//
// t_sensor.h
// Created by Andreas Gerono on 06/03/2020.

#ifndef _t_sensor_h_
#define _t_sensor_h_

#include "stm32l0xx_hal.h"

void sensor_init();
uint16_t sensor_GetRawTemp();
char* sensor_getTemp();

#endif
