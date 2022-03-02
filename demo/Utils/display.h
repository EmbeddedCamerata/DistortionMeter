#ifndef __DISPLAY_H
#define __DISPLAY_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

#define TXT_WIDTH       6
#define TXT_HEIGHT      8
#define GRAPH_START_X   0
#define GRAPH_START_Y   9
#define GRAPH_WIDTH     128
#define GRAPH_HEIGHT    90

#define XSCALE          4
#define YSCALE          5

extern uint8_t graph[128];

void Oscilloscope_Init(void);
void Distortion_Init(void);

void Get_Wave(uint16_t *ADCValue, uint8_t *wave);
void Wave_View(uint16_t *ADCValue, uint32_t sample_rate, uint8_t *wave);
void Display_SampleRate(uint32_t sample_rate);
void Display_Wave(uint8_t *y);
void Display_Status(void);

void Display_Spectrum(uint8_t *y);
void Display_SampleFreq(uint32_t sample_rate);
void Display_THDx(float THDx);
void Spectrum_View(uint16_t* FFTValue, uint8_t *spectrum, uint32_t sample_rate);

#ifdef __cplusplus
}
#endif    
#endif /* __DISPLAY_H */
