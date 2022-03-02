#include "status.h"

static uint8_t mode_flag = 0x00;
static uint8_t display_flag = 0x00;
static uint8_t trigger_flag = 0x00;

/*
    Get status and check operation for display_flag 
*/
uint8_t is_displaying_wave(void)
{
    return display_flag & DISPLAY_WAVE;
}

void set_displaying_wave(void)
{
    display_flag |= DISPLAY_WAVE;
}

void set_displaying_distortion(void)
{
    display_flag &= ~DISPLAY_WAVE;
}

void toggle_display(void)
{
    display_flag ^= DISPLAY_WAVE;
}

/*
    Get status and check operation for mode_flag 
*/
uint8_t is_source_on(void)
{
	return mode_flag & SOURCE_ON;
}

void set_source_on(void)
{
    mode_flag |= SOURCE_ON;
}

void set_source_off(void)
{
    mode_flag &= ~SOURCE_ON;
}

uint8_t is_auto_scale(void)
{
	return mode_flag & AUTO_SCALE;
}

void set_auto_scale(void)
{
    mode_flag |= AUTO_SCALE; 
}

void set_manual_scale(void)
{
    mode_flag &= ~AUTO_SCALE;
}

void toggle_scale(void)
{
    mode_flag ^= AUTO_SCALE;
}

uint8_t is_sample_finished(void)
{
	return mode_flag & SAMPLE_FINISHED;
}

void clr_sample_finished(void)
{
	mode_flag &= ~SAMPLE_FINISHED;
}

void set_sample_finished(void)
{
    mode_flag |= SAMPLE_FINISHED;
}

uint8_t is_setting_source_freq(void)
{
    return mode_flag & SETTING_SOURCE_FREQ;
}

void set_setting_source_freq(void)
{
    mode_flag |= SETTING_SOURCE_FREQ;
}

void set_setting_source_amp(void)
{
    mode_flag &= ~SETTING_SOURCE_FREQ;
}

void toggle_source_setting(void)
{
    mode_flag ^= SETTING_SOURCE_FREQ;
}

uint8_t is_spectrum_yaxis_log(void)
{
    return mode_flag & SPECTRUM_YAXIS_LOG;
}

void set_spectrum_yaxis_log(void)
{
    mode_flag |= SPECTRUM_YAXIS_LOG;
}

void set_spectrum_yaxis_linear(void)
{
    mode_flag &= ~SPECTRUM_YAXIS_LOG;
}

void toggle_spectrum_yaxis(void)
{
    mode_flag ^= SPECTRUM_YAXIS_LOG;
}

/*
    Get status and check operation for tigger_flag 
*/
void trigger_ok(void)
{
	trigger_flag |= TRIGGER_OK;
}

void trigger_fail(void)
{
	trigger_flag &= ~TRIGGER_OK;
}

uint8_t is_trigger_ok(void)
{
    return trigger_flag & TRIGGER_OK;
}
