#include "spectrum.h"
#include "display.h"
#include "source.h"
#include "string.h"
#include "math.h"

uint16_t FFT_Value[SAMPLE_POINTS];

/**
  * @brief  Generate the standard specturm(Power)
  * @param  FFT values; pointer of specture wave; Y-axis db20/linear display
  * @note   None
  * @retval None
*/
void Generate_Spectrum(uint16_t *FFTValue, uint8_t *y, uint8_t log_or_linear)
{
	uint8_t max_index = Get_SpectrumMax(FFTValue, 0);
	uint8_t i;
    
    if(log_or_linear)
    {
        for(i = 0; i < FFT_POINT / 2; i++)
        {
            if(FFTValue[i] > 0)
            {
                y[i] = (uint8_t)(-30*log10f((1.0*FFTValue[i]/FFTValue[max_index])));
            }
            else
            {
                y[i] = GRAPH_HEIGHT-1;
            }
        }
    }
    else
    {
        for(i = 0; i < FFT_POINT / 2; i++)
        {
            y[i] = (GRAPH_HEIGHT-1) * (FFTValue[max_index] - FFTValue[i]) / FFTValue[max_index];
        }
    }
}

/**
  * @brief  Find the maximum value of specturm
  * @param  FFT values; 1 for ignoring dc/0 for not ignoring
  * @note   None
  * @retval Index of max value
*/
uint8_t Get_SpectrumMax(uint16_t *FFTValue, uint8_t ignore_dc)
{
	uint8_t i;
	uint8_t temp_max_index = ignore_dc ? 2 : 0;

	for (i = ignore_dc ? 3 : 1; i <= FFT_POINT / 2; i++)
		if (FFTValue[i] > FFTValue[temp_max_index])
			temp_max_index = i;

	return temp_max_index;
}

/**
  * @brief  Calculate the approximate frequency by FFT values
  * @param  FFT values; sample rate
  * @note   None
  * @retval Actual signal frequency
*/
float Get_ActualFreq(uint16_t *FFTValue, uint32_t sample_rate)
{
	return FFT_BIN(Get_SpectrumMax(FFTValue, 1), sample_rate, FFT_POINT);
}

/**
  * @brief  Calculate the THDx.Harmony waves under SAMPLE RATE/2 are included
  * @param  FFT values; ignoring dc; sample rate;
  * @note   None
  * @retval float THD
*/
float Get_THDx(uint16_t *FFTValue, uint8_t ignore_dc, uint32_t sample_rate)
{
    uint16_t maxN_power = 0;
    uint16_t max_power = 0;
    int i = 1;  
    
    max_power = FFT_Value[FFT_INDEX(Get_SourceFreq(), sample_rate, FFT_POINT)];
    // FFTValue[Get_SpectrumMax(FFTValue, ignore_dc)];
    while(Get_SourceFreq()*(i+1) <= sample_rate/2)
    {
        maxN_power += FFT_Value[FFT_INDEX(Get_SourceFreq()*(i+1), sample_rate, FFT_POINT)];
        i++;
    }
    return sqrtf(((float)maxN_power)/max_power);
}
