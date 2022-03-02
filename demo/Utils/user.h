#ifndef __USER_H
#define __USER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"
#include "status.h"
#include "oled.h"
#include "source.h"
#include "display.h"
#include "keys.h"
#include "wave.h"
#include "sample.h"
#include "spectrum.h"
    
typedef enum
{
	Oscilloscope,
	Distortion
}System_State;

void User_Init(void);
void User_Loop(void);

#ifdef __cplusplus
}
#endif    
#endif /* __USER_H */
