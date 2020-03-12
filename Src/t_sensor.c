//
// t_sensor.c
// Created by Andreas Gerono on 06/03/2020.

#include "t_sensor.h"
#include <stdlib.h>
#include <stdio.h>
#include "adc.h"
#include "tim.h"

#define	ADCMAX 4095
#define	ADCREF_MV 3300
#define TEMP_LEN 6

static uint32_t adcValue;

void sensor_init() {
	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
	HAL_TIM_Base_Start(&htim6);
	HAL_ADC_Start_DMA(&hadc, &adcValue, 1);

}

uint16_t sensor_GetRawTemp() {
	return ((adcValue * ADCREF_MV) / ADCMAX);
}

char* sensor_getTemp() {
	char* str = malloc( sizeof(char) * TEMP_LEN);
	uint32_t temp = sensor_GetRawTemp();
	sprintf(str, "%02lu.%1lu ", temp / 10, temp % 10);
	return str;
}
