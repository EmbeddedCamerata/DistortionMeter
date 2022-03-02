#ifndef __SAMPLE_H
#define __SAMPLE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

#define SAMPLE_POINTS   256
extern uint16_t ADC_Value[SAMPLE_POINTS];

void Sample_Start(uint16_t *ADCValue);
void Sample_Stop(void);
void Set_SampleRate(uint32_t sample_rate);
uint32_t Get_SampleRate(void);
void Inc_SampleRate(void);
void Dec_SampleRate(void);
void Sample_Init(void);

#ifdef __cplusplus
}
#endif    
#endif /* __SAMPLE_H */
