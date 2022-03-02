#include "display.h"
#include "stdio.h"
#include "string.h"
#include "status.h"
#include "oled.h"
#include "source.h"
#include "wave.h"
#include "sample.h"
#include "spectrum.h"

uint8_t graph[128];
static void Display_Title(void);
static void Display_Ticks(void);
static void Display_Coordinate(void);
static void Display_Vdc(float Vdc);
static void Display_Vpp(float Vp);
static void Display_YLim(void);
static void Display_Amplitude(uint16_t amp);
static void Display_Frequency(float freq);
static void Display_YaxisType(void);

/**
  * @brief  Page scope status initialization
  * @param  None
  * @note   None
  * @retval None
*/
void Oscilloscope_Init(void)
{
    set_displaying_wave();
    set_auto_scale();
    set_setting_source_freq();
}

/**
  * @brief  Page spectrum status initialization
  * @param  None
  * @note   None
  * @retval None
*/
void Distortion_Init(void)
{
    set_spectrum_yaxis_linear();
}

/**
  * @brief  Get wave data from ADC values
  * @param  ADC values; pointer of wave values
  * @note   None
  * @retval None
*/
void Get_Wave(uint16_t *ADCValue, uint8_t *wave)
{
	uint16_t trigger_index;
	if(is_auto_scale())
		auto_scale(ADCValue);
    
    trigger_index = trigger(ADCValue, SAMPLE_POINTS);
    Generate_Wave(ADCValue + trigger_index, wave);
}

/**
  * @brief  Display all the wave info
  * @param  None
  * @note   None
  * @retval None
*/
void Wave_View(uint16_t *ADCValue, uint32_t sample_rate, uint8_t *wave)
{
    Get_Wave(ADCValue, wave);
	OLED_Clear();
    Display_Title();
	Display_Coordinate();
	Display_Status();
	Display_Wave(wave);

    Display_SampleRate(sample_rate);
    Display_Vpp(get_pp_voltage(ADCValue));
    Display_Vdc(get_dc_voltage(ADCValue));
    Display_Frequency(Get_SourceFreq());
    Display_Amplitude(Get_SourceAmp());
    Display_YLim();
    
	OLED_Refresh();
}

/**
  * @brief  Display the title of OLED, scope or spectrum
  * @param  None
  * @note   None
  * @retval None
*/
static void Display_Title(void)
{
    if(is_displaying_wave())
    {
        OLED_ShowString(GRAPH_WIDTH - 1 - 5*TXT_WIDTH, 0, "Scope", 8, 1);
    }
    else
    {
        OLED_ShowString(GRAPH_WIDTH - 1 - 8*TXT_WIDTH, 0, "Spectrum", 8, 1);
    }
}

/**
  * @brief  Display ticks of scope and spectrum
  * @param  None
  * @note   None
  * @retval None
*/
static void Display_Ticks(void)
{
    uint8_t i = 0, j = 0;
    
    //中间实线
    if(is_displaying_wave())
        OLED_DrawLine(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT/2, GRAPH_START_X + GRAPH_WIDTH - 1, GRAPH_START_Y + GRAPH_HEIGHT/2, 1);
    
    for(i = 0; i < GRAPH_WIDTH / XSCALE; i++)
    {
        for(j = 0; j < GRAPH_HEIGHT / YSCALE; j++)
        {
            OLED_DrawPoint(GRAPH_START_X + i * XSCALE, GRAPH_START_Y + j * YSCALE, 1);
        }
    }
}

/**
  * @brief  Display the basic coordinate
  * @param  None
  * @note   None
  * @retval None
*/
static void Display_Coordinate(void)
{
	OLED_DrawLine(GRAPH_START_X, GRAPH_START_Y, GRAPH_START_X + GRAPH_WIDTH - 1, GRAPH_START_Y, 1);
	OLED_DrawLine(GRAPH_START_X + GRAPH_WIDTH - 1, GRAPH_START_Y, GRAPH_START_X + GRAPH_WIDTH - 1, GRAPH_START_Y + GRAPH_HEIGHT, 1);
    OLED_DrawLine(GRAPH_START_X + GRAPH_WIDTH - 1, GRAPH_START_Y + GRAPH_HEIGHT, GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT, 1);
	OLED_DrawLine(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT, GRAPH_START_X, GRAPH_START_Y, 1);
    
    Display_Ticks();
}

/**
  * @brief  Display sample rate on OLED
  * @param  Sample rate
  * @note   None
  * @retval None
*/
void Display_SampleRate(uint32_t sample_rate)
{
	char str[10];

	if (sample_rate <= 128000)
		sprintf(str, "%.1fms", (float)(128000 * 1.0 / sample_rate));
	else
		sprintf(str, "%.1fus", (float)(128000000 * 1.0 / sample_rate));
    
    OLED_ShowString(GRAPH_START_X, 0, str, 8, 0);
}

/**
  * @brief  Display wave on LCD
  * @param  Waves lines
  * @note   None
  * @retval None
*/
void Display_Wave(uint8_t *y)
{
	uint8_t x;
	for(x = GRAPH_START_X; x < GRAPH_WIDTH - 2; x++)
		OLED_DrawLine(x, y[x-GRAPH_START_X], x + 1, y[x-GRAPH_START_X+1], 1);
    
	OLED_DrawPoint(x, y[x-GRAPH_START_X], 1);
}

/**
  * @brief  Display status info about AUTO scale or manual y offset, source frequency or amplitude setting
  * @param  None
  * @note   None
  * @retval None
*/
void Display_Status(void)
{
	if(is_auto_scale())
        OLED_ShowString(GRAPH_START_X + GRAPH_WIDTH - 1 - 4*TXT_WIDTH, GRAPH_START_Y + GRAPH_HEIGHT + 2*TXT_HEIGHT + 2, "AUTO", 8, 1);
    else
        OLED_ShowString(GRAPH_START_X + GRAPH_WIDTH - 1 - 4*TXT_WIDTH, GRAPH_START_Y + GRAPH_HEIGHT + 2*TXT_HEIGHT + 2, "MANU", 8, 1);
    
    if(is_setting_source_freq())
        OLED_ShowString(64 - 2*TXT_WIDTH, 0, "FREQ", 8, 1);
    else
        OLED_ShowString(64 - 2*TXT_WIDTH, 0, "AMP", 8, 1);
}

/**
  * @brief  Display dc voltage
  * @param  Voltage of Vdc
  * @note   None
  * @retval None
*/
static void Display_Vdc(float Vdc)
{
	char str[12];
	sprintf(str, "Vdc=%.2fV", Vdc);
	OLED_ShowString(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT + TXT_HEIGHT + 2, str, 8, 1);
}

/**
  * @brief  Display peak of voltage
  * @param  Peak of voltage
  * @note   None
  * @retval None
*/
static void Display_Vpp(float Vp)
{
	char str[12];
	sprintf(str, "Vp=%.2fV", Vp);
    OLED_ShowString(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT + 2, str, 8, 1);
}

/**
  * @brief  Display the limit of Y-axis
  * @param  None
  * @note   None
  * @retval None
*/
static void Display_YLim(void)
{
    char str[18];
	sprintf(str, "YLim:%.1fV-%.1fV", y_axis.min_voltage, y_axis.max_voltage);
	OLED_ShowString(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT + 2*TXT_HEIGHT + 2, str, 8, 1);
}

/**
  * @brief  Display amplitude of source signal
  * @param  None
  * @note   None
  * @retval None
*/
static void Display_Amplitude(uint16_t amp)
{
    char str[10];
	sprintf(str, "A=%umV", amp);
    if(amp >= 100)
        OLED_ShowString(GRAPH_START_X + GRAPH_WIDTH - 1 - 7*TXT_WIDTH, GRAPH_START_Y + GRAPH_HEIGHT + TXT_HEIGHT + 2, str, 8, 1);
    else
        OLED_ShowString(GRAPH_START_X + GRAPH_WIDTH - 1 - 6*TXT_WIDTH, GRAPH_START_Y + GRAPH_HEIGHT + TXT_HEIGHT + 2, str, 8, 1);
}

/**
  * @brief  Display setting/actual frequency
  * @param  Signal frequency
  * @note   None
  * @retval None
*/
static void Display_Frequency(float freq)
{
	char str[11];
    
	sprintf(str, "F=%.1fKHz", freq / 1000);
    if(freq >= 10000)
        OLED_ShowString(GRAPH_START_X + GRAPH_WIDTH - 1 - 9*TXT_WIDTH, GRAPH_START_Y + GRAPH_HEIGHT + 2, str, 8, 1);
    else
        OLED_ShowString(GRAPH_START_X + GRAPH_WIDTH - 1 - 8*TXT_WIDTH, GRAPH_START_Y + GRAPH_HEIGHT + 2, str, 8, 1);
}

/**
  * @brief  Display specturm
  * @param  spectrum data calculated by Generate_Spectrum()
  * @note   None
  * @retval None
*/
void Display_Spectrum(uint8_t *y)
{
	uint8_t x;
    if(is_spectrum_yaxis_log())
    {
        for(x = GRAPH_START_X; x < GRAPH_WIDTH - 1; x++)
            OLED_DrawLine(x, GRAPH_START_Y + y[x-GRAPH_START_X], x + 1, GRAPH_START_Y + y[x-GRAPH_START_X+1], 1);
        OLED_DrawPoint(x, y[x-GRAPH_START_X], 1); 
    }
    else
    {
        for(x = GRAPH_START_X; x < GRAPH_WIDTH - 1; x++)
            OLED_DrawLine(x, GRAPH_START_Y + y[x-GRAPH_START_X], x, GRAPH_START_Y + GRAPH_HEIGHT, 1);
    
        OLED_DrawPoint(x, y[x-GRAPH_START_X], 1); 
    }
    
}

/**
  * @brief  Display sample frequency
  * @param  Sample frequency
  * @note   None
  * @retval None
*/
void Display_SampleFreq(uint32_t sample_rate)
{
	char str[14];
	sprintf(str, "%uHz/div", sample_rate/(2*32));
    
	OLED_ShowString(GRAPH_START_X, 0, str, 8, 0);
}

/**
  * @brief  Display type of Y-axis on spectrum, linear or log(dB10)
  * @param  None
  * @note   None
  * @retval None
*/
static void Display_YaxisType(void)
{
	if(is_spectrum_yaxis_log())
        OLED_ShowString(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT + 2, "dB10", 8, 1);
    else
        OLED_ShowString(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT + 2, "Linear", 8, 1);
}

/**
  * @brief  Display THD
  * @param  THDx calculated by Get_THDx()
  * @note   None
  * @retval None
*/
void Display_THDx(float THDx)
{
    char str[12];
	sprintf(str, "THD=%.1f%%", THDx*100);
	OLED_ShowString(GRAPH_START_X, GRAPH_START_Y + GRAPH_HEIGHT + TXT_HEIGHT + 2, str, 8, 1);
}

/**
  * @brief  Display all spectrum info
  * @param  FFT values; spectrum data; sample rate
  * @note   None
  * @retval None
*/
void Spectrum_View(uint16_t* FFTValue, uint8_t *spectrum, uint32_t sample_rate)
{
	OLED_Clear();
    Display_Title();
    Display_Coordinate();
    Display_YaxisType();
	Display_Spectrum(spectrum);
    Display_Frequency(Get_ActualFreq(FFT_Value, sample_rate));
	Display_SampleFreq(sample_rate);
    Display_THDx(Get_THDx(FFT_Value, 1, sample_rate));
    
	OLED_Refresh();
}
