// Copyright 2021 Matías Levy Alfie.
//
// Author: Matías Levy Alfie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Driver for PWM DAC.


// We will try using the 4 channels from TIM4 for this, separating 2 for each analog output forming a Dual Pwm.
// TIM4 is mapped to PB6, PB7, PB8 and PB9
// Vamos a probar usar los 4 canales del TIM4, separando 2 por canal para hacer Dual PWM
// El TIM4 esta mapeado a PB6, PB7, PB8 y PB9

#ifndef PEAKS_DRIVERS_PWM_DAC_H_
#define PEAKS_DRIVERS_PWM_DAC_H_

#include <stm32f10x_conf.h>

#include "stmlib/stmlib.h"

namespace peaks {

const uint32_t kPwmFrequency = 125000;
const uint16_t kPwmResolution = 7;  // bits

class PWMDac {
 public:
  PWMDac() { }
  ~PWMDac() { }

  void Init() {
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // Initialize pins.
    GPIO_InitTypeDef gpio_init;

    gpio_init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio_init);

      
    TIM_TimeBaseInitTypeDef timer_init;
    timer_init.TIM_Period = (1 << kPwmResolution) - 1;
    timer_init.TIM_Prescaler = 0;
    timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
    timer_init.TIM_CounterMode = TIM_CounterMode_Up;
    timer_init.TIM_RepetitionCounter = 0;
    TIM_InternalClockConfig(TIM4);
    TIM_TimeBaseInit(TIM4, &timer_init);


    TIM_OCInitTypeDef output_compare_init;
    output_compare_init.TIM_OCMode = TIM_OCMode_PWM1;
    output_compare_init.TIM_OutputState = TIM_OutputState_Enable;
    output_compare_init.TIM_Pulse = 0x00;
    output_compare_init.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM4, &output_compare_init);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2Init(TIM4, &output_compare_init);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);    
    TIM_OC3Init(TIM4, &output_compare_init);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC4Init(TIM4, &output_compare_init);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
        
    TIM_Cmd(TIM4, ENABLE);
    
    wrote_both_channels_ = false;
  }

  void testWrite() {
    TIM_SetCompare1(TIM4, 50 >> (8 - kPwmResolution));
    TIM_SetCompare2(TIM4, 20 >> (8 - kPwmResolution));
    TIM_SetCompare3(TIM4, 40 >> (8 - kPwmResolution));
    TIM_SetCompare4(TIM4, 250 >> (8 - kPwmResolution));

  }

  inline void Write(int index, uint16_t value) {
    data_[index] = value;
  }

  inline bool Update() {
    if (wrote_both_channels_) {
      wrote_both_channels_ = false;
      //Escribimos data[0], 7 bits por PWM
      TIM_SetCompare1(TIM4, data_[0] >> 9);
      uint8_t l_byte = (data_[0] & 0x1FC) >> 2;
      TIM_SetCompare2(TIM4, l_byte);
    } else {
      wrote_both_channels_ = true;
      //Escribimos data[1]
      TIM_SetCompare3(TIM4, data_[1] >> 9);
      uint8_t l_byte = (data_[1] & 0x1FC) >> 2;
      TIM_SetCompare4(TIM4, l_byte);
    }
    return wrote_both_channels_;
  }
  

 private:
  uint16_t data_[2];
  bool wrote_both_channels_;
  
  DISALLOW_COPY_AND_ASSIGN(PWMDac);
};

}  // namespace peaks

#endif  // PEAKS_DRIVERS_PWM_DAC_H_
