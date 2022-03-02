#ifndef __WAVE_H
#define __WAVE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

typedef struct
{
	float mid_voltage;
	float max_voltage;
	float min_voltage;
} Y_axis_params;

extern Y_axis_params y_axis;

float get_pp_voltage(uint16_t *ADCValue);
float get_dc_voltage(uint16_t *ADCValue);
void auto_scale(uint16_t *ADCValue);
void Inc_YScale(void);
void Dec_YScale(void);
void Generate_Wave(uint16_t *ADCValue, uint8_t *y_onscreen);
uint16_t trigger(uint16_t *ADCValue, uint16_t total_points);

void Wave_Interrupt(void);

#ifdef __cplusplus
}
#endif    
#endif /* __WAVE_H */
