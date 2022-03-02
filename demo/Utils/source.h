#ifndef __SOURCE_H
#define __SOURCE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

typedef struct
{
	uint32_t frequency;
	uint16_t amplitude;
} Source_Params;

#define SIGNAL_LENGTH 256

void PWM_Start(void);
void PWM_Stop(void);
void Set_SourceSignal(Source_Params signal);
void Inc_SourceFreq(void);
void Dec_SourceFreq(void);
void Inc_SourceAmp(void);
void Dec_SourceAmp(void);
uint32_t Get_SourceFreq(void);
uint16_t Get_SourceAmp(void);
void Source_Init(void);

#ifdef __cplusplus
}
#endif    
#endif /* __SOURCE_H */
