/*
ESP32的ADC资源

GPIO36      ADC1_CH0            GPIO4       ADC2_CH0
GPIO37      ADC1_CH1            GPIO0       ADC2_CH1
GPIO38      ADC1_CH2            GPIO2       ADC2_CH2
GPIO39      ADC1_CH3            GPIO15      ADC2_CH3
GPIO32      ADC1_CH4            GPIO13      ADC2_CH4
GPIO33      ADC1_CH5            GPIO12      ADC2_CH5
GPIO34      ADC1_CH6            GPIO14      ADC2_CH6
GPIO35      ADC1_CH7            GPIO27      ADC2_CH7
                                GPIO25      ADC2_CH8
                                GPIO26      ADC2_CH9
注
    在使用WIFI时不要使用ADC2
*/
#include "adc.h"

const static char *TAG = "adc";
/* 初始化ADC */
void adc_oneshot_init(struct ADC_ONESHOT *adc)
{

    adc_oneshot_unit_init_cfg_t onehost_unit_cfg = {};
    onehost_unit_cfg.unit_id = adc->unit_id;
    // onehost_unit_cfg.ulp_mode =
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&onehost_unit_cfg, &adc->handle));
}
/* 配置ADC */
void adc_oneshot_cfg_channel(struct ADC_ONESHOT *adc, struct ADC_CHANNEL *chan)
{
    /* 配置ADC */
    adc_oneshot_chan_cfg_t oneshot_chan_cfg = {};
    oneshot_chan_cfg.bitwidth = ADC_BITWIDTH_DEFAULT; /* ADC衰弱 */
    oneshot_chan_cfg.atten = ADC_ATTEN_DB_12;         /* ADC位宽 */
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc->handle, chan->channel, &oneshot_chan_cfg));
    /* 配置校准 */
    chan->cali_result = example_adc_calibration_init(adc->unit_id, chan->channel, ADC_ATTEN_DB_12, &chan->cali_handle);
}

/**************************************************************************************************************************
 * ADC校准
 *************************************************************************************************************************/
bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI("adc_cali", "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated)
    {
        ESP_LOGI("adc_cali", "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK)
        {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Calibration Success");
    }
    else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated)
    {
        ESP_LOGW("adc_cali", "eFuse not burnt, skip software calibration");
    }
    else
    {
        ESP_LOGE("adc_cali", "Invalid arg or no memory");
    }

    return calibrated;
}
