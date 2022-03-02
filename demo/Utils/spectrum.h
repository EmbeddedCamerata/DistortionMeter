#ifndef __SPECTRUM_H
#define __SPECTRUM_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"
#include "Adafruit_ZeroFFT.h"
    
#define SAMPLE_POINTS   256
extern uint16_t FFT_Value[SAMPLE_POINTS];

void Generate_Spectrum(uint16_t *FFTValue, uint8_t *y, uint8_t log_or_linear);
uint8_t Get_SpectrumMax(uint16_t *FFTValue, uint8_t ignore_dc);
float Get_ActualFreq(uint16_t *FFTValue, uint32_t sample_rate);
float Get_THDx(uint16_t *FFTValue, uint8_t ignore_dc, uint32_t sample_rate);
    
#ifdef __cplusplus
}
#endif    
#endif /* __SPECTRUM_H */

