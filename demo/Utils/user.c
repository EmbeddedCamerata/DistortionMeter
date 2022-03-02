#include "user.h"
#include "string.h"

System_State System = Oscilloscope;
uint8_t Page_Init = 1;
uint8_t zoom_in = 0x00;
uint8_t zoom_out = 0x00;

void System_Change_State(System_State State)
{
    System = State;
    Page_Init = 1;
}

void OLED_Handle(void)
{
    switch(System)
    {
        case Oscilloscope:
        {
            if(Page_Init)
			{
				Page_Init = 0;
                zoom_in = 0x00;
                zoom_out = 0x00;
				Oscilloscope_Init();
			}
            memset(FFT_Value, 0x0000, sizeof(FFT_Value));
            memset(ADC_Value, 0x0000, sizeof(ADC_Value));
            Source_Init();
            Sample_Init();
            Wave_View(ADC_Value, Get_SampleRate(), graph);
            break;
        }
        
        case Distortion:
        {
            if(Page_Init)
			{
				Page_Init = 0;
                zoom_in = 0x00;
                zoom_out = 0x00;
				Distortion_Init();
			}
            Sample_Init();
            memcpy(FFT_Value, ADC_Value, sizeof(FFT_Value));
            if(ZeroFFT((int16_t*)FFT_Value, FFT_POINT) != -1)
            {   
                Generate_Spectrum((uint16_t*)FFT_Value, graph, is_spectrum_yaxis_log());
            }
            else
            {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
            }
            Spectrum_View(FFT_Value, graph, Get_SampleRate());
            break;
        }
        
        default:
            break;
    }
}

void Key_Handle(void)
{
    static Key_Type Key[3] = {0};
    Get_Key(Key);
    
    switch(System)
    {
        case Oscilloscope:
        {
            if(Get_Rise(Key1) || Get_Long_Tri(Key1))
            {
                if(is_setting_source_freq())
                    Inc_SourceFreq();
                else
                    Inc_SourceAmp();
            }
            if(Get_Rise(Key2) || Get_Long_Tri(Key2))
            {
                if(is_setting_source_freq())
                    Dec_SourceFreq();
                else
                    Dec_SourceAmp();
            }
            if(Get_Long_Press(KeyP))    // 长按旋钮
            {
                toggle_display();
                System_Change_State(Distortion);
            }
            if(Get_Cont_Click(KeyP))    // 短按旋钮
            {
                toggle_scale();
                toggle_source_setting();
            }
            if(zoom_in == 0x01)
            {
                if(is_auto_scale())
                {
                    Dec_SampleRate();
                }
                else
                {
                    Inc_YScale();
                }
                zoom_in = 0x00;
            }
            else if(zoom_out == 0x01)
            {
                if(is_auto_scale())
                {
                    Inc_SampleRate();
                }
                else
                {
                    Dec_YScale();
                }
                zoom_out = 0x00;
            }
            break;
        }
        
        case Distortion:
        {
            if(Get_Long_Press(KeyP))    // 长按旋钮
            {
                toggle_display();
                System_Change_State(Oscilloscope);
            }
            if(Get_Rise(KeyP))    // 短按旋钮
            {
                toggle_spectrum_yaxis();
            }
            if(zoom_in == 0x01)
            {
                Inc_SampleRate();
                zoom_in = 0x00;
            }
            else if(zoom_out == 0x01)
            {
                Dec_SampleRate();
                zoom_out = 0x00;
            }
            break;
        }
        
        default:
            break;
    }
}

void User_Init(void)
{
    OLED_Init();
}

void User_Loop(void)
{
    OLED_Handle();
    HAL_Delay(1000);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{        
    if(htim->Instance == TIM14)
    {
        Key_Handle();
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        if(HAL_GPIO_ReadPin(KeyB_GPIO_Port, KeyB_Pin))
        {
            zoom_out = 0x00;
            zoom_in = 0x01;
        }
        else
        {   
            zoom_out = 0x01;
            zoom_in = 0x00;
        }
    }
}
