#ifndef _ADC_H_
#define _ADC_H_

#include "datafile.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

struct ADC_CHANNEL
{
    uint8_t channel;
    adc_cali_handle_t cali_handle;
    uint8_t cali_result;
    int value;
    int value_mv;
};

struct ADC_ONESHOT
{
    adc_oneshot_unit_handle_t handle;   
    uint8_t unit_id;
};

void adc_oneshot_init(struct ADC_ONESHOT *adc);
void adc_oneshot_cfg_channel(struct ADC_ONESHOT *adc, struct ADC_CHANNEL *chan);
bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);
#endif