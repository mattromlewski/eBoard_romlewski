/* Simple LED task demo:
 *
 * The LED on PC13 is toggled in task1.
 */
#include <string.h>
#include <ctype.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#define GPIO_PORT_LED		GPIOC		// Builtin LED port
#define GPIO_LED		GPIO13		// Builtin LED

#define mainECHO_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )

static QueueHandle_t adc_txq;		// TX queue for UART

static uint16_t
read_adc(uint8_t channel) {
	adc_set_sample_time(ADC1,channel,ADC_SMPR_SMP_239DOT5CYC);
	adc_set_regular_sequence(ADC1,1,&channel);
	adc_start_conversion_direct(ADC1);
	while ( !adc_eoc(ADC1) )
		taskYIELD();
	return adc_read_regular(ADC1);
}

static void
adc_task(void *args __attribute__((unused))) {

	int adc0;

	for (;;) {
		adc0 = read_adc(0) * 330 / 4095;
		xQueueSend(adc_txq,adc0,portMAX_DELAY);
		vTaskDelay(pdMS_TO_TICKS(10));

	}
}


static void
pwm_task(void *args __attribute__((unused))) {
	int ms = 0;

	gpio_set(GPIOC,GPIO13);				// LED on

	rcc_periph_clock_enable(RCC_TIM2);		// Need TIM2 clock
	rcc_periph_clock_enable(RCC_AFIO);		// Need AFIO clock

	// PA1 == TIM2.CH2
	rcc_periph_clock_enable(RCC_GPIOA);		// Need GPIOA clock
	gpio_primary_remap(
			AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_OFF,	// Optional
			AFIO_MAPR_TIM2_REMAP_NO_REMAP);		// This is default: TIM2.CH2=GPIOA1
	gpio_set_mode(GPIOA,GPIO_MODE_OUTPUT_50_MHZ,	// High speed
				  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,GPIO1);	// GPIOA1=TIM2.CH2

	// TIM2:
	timer_disable_counter(TIM2);
	rcc_periph_reset_pulse(RST_TIM2);

	timer_set_mode(TIM2,
				   TIM_CR1_CKD_CK_INT,
				   TIM_CR1_CMS_EDGE,
				   TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM2,72-1); //see https://github.com/ve3wwg/stm32f103c8t6/pull/12/
	// Only needed for advanced timers:
	// timer_set_repetition_counter(TIM2,0);
	timer_enable_preload(TIM2);
	timer_continuous_mode(TIM2);
	timer_set_period(TIM2,33333-1);

	timer_disable_oc_output(TIM2,TIM_OC2);
	timer_set_oc_mode(TIM2,TIM_OC2,TIM_OCM_PWM1);
	timer_enable_oc_output(TIM2,TIM_OC2);

	timer_set_oc_value(TIM2,TIM_OC2,ms);
	timer_enable_counter(TIM2);

	for (;;) {

		if ( xQueueReceive(adc_txq,&ms,500) == pdPASS ) {
			while ( !usart_get_flag(USART1,USART_SR_TXE) )
				taskYIELD();	// Yield until ready
			timer_set_oc_value(TIM2,TIM_OC2,ms);
		}
	}
}

static void
gpio_setup(void) {

	rcc_periph_clock_enable(RCC_GPIOA);		// Enable GPIOA for ADC
	gpio_set_mode(GPIOA,
				  GPIO_MODE_INPUT,
				  GPIO_CNF_INPUT_ANALOG,
				  GPIO0);
}

static void
adc_setup(void) {
	// Initialize ADC:
	rcc_peripheral_enable_clock(&RCC_APB2ENR,RCC_APB2ENR_ADC1EN);
	adc_power_off(ADC1);
	rcc_peripheral_reset(&RCC_APB2RSTR,RCC_APB2RSTR_ADC1RST);
	rcc_peripheral_clear_reset(&RCC_APB2RSTR,RCC_APB2RSTR_ADC1RST);
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV6);	// Set. 12MHz, Max. 14MHz
	adc_set_dual_mode(ADC_CR1_DUALMOD_IND);		// Independent mode
	adc_disable_scan_mode(ADC1);
	adc_set_right_aligned(ADC1);
	adc_set_single_conversion_mode(ADC1);
	adc_set_sample_time(ADC1,ADC_CHANNEL_TEMP,ADC_SMPR_SMP_71DOT5CYC);
	adc_set_sample_time(ADC1,ADC_CHANNEL_VREF,ADC_SMPR_SMP_71DOT5CYC);
	adc_enable_temperature_sensor();
	adc_power_on(ADC1);
	adc_reset_calibration(ADC1);
	adc_calibrate_async(ADC1);
	while ( adc_is_calibrating(ADC1) );
}

int
main(void) {

	gpio_setup();

	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(
			GPIOC,
			GPIO_MODE_OUTPUT_2_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL,
			GPIO13);

	adc_setup();

	xTaskCreate(adc_task, "ADC",100,NULL,configMAX_PRIORITIES-2,NULL);
	xTaskCreate(pwm_task,"PWM",100,NULL,configMAX_PRIORITIES-1,NULL);

	vTaskStartScheduler();
	for (;;);
	return 0;
}

// End
