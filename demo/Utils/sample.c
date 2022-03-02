#include "sample.h"
#include "tim.h"
#include "adc.h"
#include "status.h"

#define SAMPLE_RATES_NUM 9
uint16_t ADC_Value[SAMPLE_POINTS];
static const uint32_t sample_rate_list[SAMPLE_RATES_NUM] = {2560, 5120, 10240, 20480, 40960, 81920, 102400, 204800, 409600};
static int8_t sample_rate_index = 5;

void Sample_Start(uint16_t *ADCValue)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADCValue, SAMPLE_POINTS);
}

void Sample_Stop(void)
{
    HAL_TIM_Base_Stop(&htim1);
	HAL_TIM_Base_Stop_IT(&htim1);
    HAL_ADC_Stop_DMA(&hadc1);
}

uint32_t Get_SampleRate(void)
{
    return sample_rate_list[sample_rate_index];
}

void Set_SampleRate(uint32_t sample_rate)
{
    __HAL_TIM_SET_AUTORELOAD(&htim1, 64000000 / sample_rate - 1);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 32000000 / sample_rate);
}

void Inc_SampleRate(void)
{
	if (sample_rate_index < SAMPLE_RATES_NUM - 1)
		sample_rate_index++;
	else
		sample_rate_index = SAMPLE_RATES_NUM - 1;
}

void Dec_SampleRate(void)
{
	if (sample_rate_index > 0 && sample_rate_index < SAMPLE_RATES_NUM)
		sample_rate_index--;
	else
		sample_rate_index = 0;
}

void Sample_Init(void)
{
    Set_SampleRate(Get_SampleRate());
    Sample_Start(ADC_Value);
    while(!is_sample_finished());  // Wait until sample finished.
    clr_sample_finished();
}
