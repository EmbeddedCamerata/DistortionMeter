#include "wave.h"
#include "display.h"
#include "status.h"
#include "sample.h"

#define adc2volt(x)     ((float)(x*3.3/4096))
#define volt2adc(x)     (x/3.3*4096)
#define V_SCALE_NUM 9

static const float v_scale_list[V_SCALE_NUM] = {0.2, 0.5, 0.8, 1, 1.2, 1.5, 1.8, 2, 2.5};
static int8_t v_scale_index;
Y_axis_params y_axis;

static void get_max_min_pp_value(uint16_t *ADCValue, uint16_t *a_max_value, uint16_t *a_min_value, uint16_t *a_pp_value);
static uint16_t get_dc_value(uint16_t *ADCValue);
static void voltage_range_auto_select(float v_amplitude);

/**
  * @brief  Calculate the max/min and the delta value of result after ADC conversion
  * @param  Result of ADC; pointers of ADC max/min/max-min value
  * @note   None
  * @retval None
*/
static void get_max_min_pp_value(uint16_t *ADCValue, uint16_t *a_max_value, uint16_t *a_min_value, uint16_t *a_pp_value)
{
	uint16_t i;
	uint16_t a_temp_max_value = ADCValue[0], a_temp_min_value = ADCValue[0];

	for (i = 1; i < SAMPLE_POINTS; i++)
	{
		if (ADCValue[i] > a_temp_max_value)
			a_temp_max_value = ADCValue[i];
		if (ADCValue[i] < a_temp_min_value)
			a_temp_min_value = ADCValue[i];
	}

	if (a_max_value != NULL)
		*a_max_value = a_temp_max_value;
	if (a_min_value != NULL)
		*a_min_value = a_temp_min_value;
	if (a_pp_value != NULL)
		*a_pp_value = a_temp_max_value - a_temp_min_value;
}

/**
  * @brief  Calculate the average value(dc) of ADC
  * @param  Result of ADC
  * @note   None
  * @retval uint16_t dc value
*/
static uint16_t get_dc_value(uint16_t *ADCValue)
{
	uint16_t i;
	uint32_t a_sum = 0;
	for(i = 0; i < SAMPLE_POINTS; i++)
		a_sum += ADCValue[i];

	return (uint16_t)(a_sum / SAMPLE_POINTS);
}

/**
  * @brief  Calculate the voltage of peak-peak after adc2volt
  * @param  Result of ADC
  * @note   None
  * @retval float voltage of peak-peak
*/
float get_pp_voltage(uint16_t *ADCValue)
{
	uint16_t a_pp_value;
	get_max_min_pp_value(ADCValue, NULL, NULL, &a_pp_value);
	return adc2volt(a_pp_value);
}

/**
  * @brief  Calculate the dc voltage after adc2volt
  * @param  Result of ADC
  * @note   None
  * @retval float dc voltage
*/
float get_dc_voltage(uint16_t *ADCValue)
{
	uint16_t a_dc_value;
	a_dc_value = get_dc_value(ADCValue);
	return adc2volt(a_dc_value);
}

/**
  * @brief  Auto scale Y-axis
  * @param  ADC values
  * @note   None
  * @retval None
*/
void auto_scale(uint16_t *ADCValue)
{
	uint16_t a_max_value, a_min_value, a_pp_value;
	float exact_voltage, floor_voltage, ceil_voltage;
	get_max_min_pp_value(ADCValue, &a_max_value, &a_min_value, &a_pp_value);
    
    exact_voltage = adc2volt(a_max_value + a_min_value) / 2;
    floor_voltage = (adc2volt((a_max_value + a_min_value)*5)) / 10.0;  // keep one decimal
	ceil_voltage = floor_voltage + 0.1;
    
	voltage_range_auto_select(adc2volt(a_pp_value/2));

	y_axis.mid_voltage = (ceil_voltage - exact_voltage < exact_voltage - floor_voltage) ? ceil_voltage : floor_voltage;
	y_axis.max_voltage = y_axis.mid_voltage + v_scale_list[v_scale_index];
	y_axis.min_voltage = y_axis.mid_voltage - v_scale_list[v_scale_index];
}

/**
  * @brief  Auto select voltage range
  * @param  Amplitude of wave
  * @note   None
  * @retval None
*/
static void voltage_range_auto_select(float v_amplitude)
{
	uint8_t i = 0;
	for (i = 0; i < V_SCALE_NUM; i++)
    {
        if (v_amplitude < v_scale_list[i])
		{
			v_scale_index = i+1;
			break;
		}
    }
}

/**
  * @brief  Increase Y-axis scale
  * @param  None
  * @note   None
  * @retval None
*/
void Inc_YScale(void)
{
	if (v_scale_index + 1 < V_SCALE_NUM)
		v_scale_index++;
	y_axis.max_voltage = y_axis.mid_voltage + v_scale_list[v_scale_index];
	y_axis.min_voltage = y_axis.mid_voltage - v_scale_list[v_scale_index];
}

/**
  * @brief  Decrease Y-axis scale
  * @param  None
  * @note   None
  * @retval None
*/
void Dec_YScale(void)
{
	if (v_scale_index - 1 >= 0)
		v_scale_index--;
	y_axis.max_voltage = y_axis.mid_voltage + v_scale_list[v_scale_index];
	y_axis.min_voltage = y_axis.mid_voltage - v_scale_list[v_scale_index];
}

/**
  * @brief  Calculate the values of Y-axis displayed on screen
  * @param  Result of ADC, pointers of values of Y-axis displayed on screen
  * @note   None
  * @retval None
*/
void Generate_Wave(uint16_t *ADCValue, uint8_t *y_onscreen)
{
	int16_t a_max_value = volt2adc(y_axis.max_voltage);
	int16_t a_min_value = volt2adc(y_axis.min_voltage);
	uint8_t i;

	for (i = 0; i < GRAPH_WIDTH - 1; i++)
	{
		if (ADCValue[i] <= a_max_value && ADCValue[i] >= a_min_value)
			y_onscreen[i] = (GRAPH_HEIGHT - 1) * (a_max_value - ADCValue[i]) / (a_max_value - a_min_value) + GRAPH_START_Y;
		else if (ADCValue[i] > a_max_value)
			y_onscreen[i] = GRAPH_START_Y;
		else if (ADCValue[i] < a_min_value)
			y_onscreen[i] = GRAPH_HEIGHT + GRAPH_START_Y - 1;
	}
}

/**
  * @brief  Wave trigger
  * @param  Result of ADC, pointers of values of Y-axis displayed on screen
  * @note   None
  * @retval None
*/
uint16_t trigger(uint16_t *ADCValue, uint16_t total_points)
{
	uint16_t i;
	uint16_t trigger_value = volt2adc(1.65);

	for (i = 1; i < total_points - GRAPH_WIDTH + 2; i++)
	{ 
        if (ADCValue[i-1] > trigger_value && ADCValue[i] <= trigger_value)
        {
            trigger_ok();
            return i;
        }
	}

	trigger_fail();
	return 0;
}
