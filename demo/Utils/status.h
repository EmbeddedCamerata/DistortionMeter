#ifndef __STATUS_H
#define __STATUS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"

/*
    Status of Mode Flag
    #0: source on/off
    #1: AUTO/manual scale
    #2：ADC sample finished/not finished
    #3: Current setting for source frequency/amplitude
*/
#define SOURCE_ON           (uint8_t)0x01
#define AUTO_SCALE          (uint8_t)0x02
#define SAMPLE_FINISHED     (uint8_t)0x04
#define SETTING_SOURCE_FREQ (uint8_t)0x08
#define SPECTRUM_YAXIS_LOG  (uint8_t)0x10

/*
    Status of Display Flag
    #0: displaying wave/distortion
    #1: displaying scope menu
*/
#define DISPLAY_WAVE        (uint8_t)0x01
#define DISPLAY_SCOPE_MENU  (uint8_t)0x02

/*
    Status of Trigger Flag
    #0: Trigger on/off
    #1: Trigger edge rising/falling
    #2: Trigger success/fail
*/
#define TRIGGER_ON          (uint8_t)0x01
#define TRIGGER_RISING_EDGE (uint8_t)0x02
#define TRIGGER_OK          (uint8_t)0x04

/* Display Flag */
uint8_t is_displaying_wave(void);
void set_displaying_wave(void);
void set_displaying_distortion(void);
void toggle_display(void);

/* Mode Flag */
uint8_t is_source_on(void);
void set_source_on(void);
void set_source_off(void);

uint8_t is_auto_scale(void);
void set_auto_scale(void);
void set_manual_scale(void);
void toggle_scale(void);

uint8_t is_sample_finished(void);
void set_sample_finished(void);
void clr_sample_finished(void);

uint8_t is_setting_source_freq(void);
void set_setting_source_freq(void);
void set_setting_source_amp(void);
void toggle_source_setting(void);

uint8_t is_spectrum_yaxis_log(void);
void set_spectrum_yaxis_log(void);
void set_spectrum_yaxis_linear(void);
void toggle_spectrum_yaxis(void);

/* Trigger Flag */
void toggle_trigger(void);
uint8_t is_trigger_on(void);
void trigger_ok(void);
void trigger_fail(void);
uint8_t is_trigger_ok(void);


#ifdef __cplusplus
}
#endif    
#endif /* __STATUS_H */
