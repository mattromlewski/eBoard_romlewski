/* Simple LED task demo:
 *
 * The LED on PC13 is toggled in task1.
 */
#include <string.h>
#include <ctype.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "meter.h"
#include "oled.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/cm3/nvic.h>

#define GPIO_PORT_LED		GPIOC		// Builtin LED port
#define GPIO_LED		GPIO13		// Builtin LED

#define mainECHO_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )

// Hewwo! I love you please don't delete this

static QueueHandle_t uart_txq;		// TX queue for UART

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
uart_setup(void) {

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);

	// UART TX on PA9 (GPIO_USART1_TX)
	gpio_set_mode(GPIOA,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		GPIO_USART1_TX);

	usart_set_baudrate(USART1,38400);
	usart_set_databits(USART1,8);
	usart_set_stopbits(USART1,USART_STOPBITS_1);
	usart_set_mode(USART1,USART_MODE_TX);
	usart_set_parity(USART1,USART_PARITY_NONE);
	usart_set_flow_control(USART1,USART_FLOWCONTROL_NONE);
	usart_enable(USART1);

	// Create a queue for data to transmit from UART
	uart_txq = xQueueCreate(512,sizeof(char));


}

//Queue a string of characters to be transmitted
static void
uart_puts(const char *s) {

	for ( ; *s; ++s ) {
		// blocks when queue is full
		xQueueSend(uart_txq,s,portMAX_DELAY);
	}
}

static void
adc_task(void *args __attribute__((unused))) {

	int adc0;
	char tx_string[16];

	for (;;) {
		adc0 = read_adc(0) * 330 / 4095;
		sprintf(tx_string,"%d\n\r",adc0);
		uart_puts(tx_string);
		vTaskDelay(pdMS_TO_TICKS(10));

	}
}

static void
uart_task(void *args __attribute__((unused))) {
	char ch;

	for (;;) {
		// Receive char to be TX
		if ( xQueueReceive(uart_txq,&ch,500) == pdPASS ) {
			while ( !usart_get_flag(USART1,USART_SR_TXE) )
				taskYIELD();	// Yield until ready
			usart_send(USART1,ch);
		}
		// Toggle LED to show signs of life
		gpio_toggle(GPIOC,GPIO13);
	}
}

static void
gpio_setup(void) {

	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(
		GPIOC,
		GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		GPIO13);
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

	//gpio_setup();

	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(
		GPIOC,
		GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL,
		GPIO13);

	uart_setup();
	adc_setup();

	xTaskCreate(adc_task, "ADC",100,NULL,configMAX_PRIORITIES-2,NULL);
	xTaskCreate(uart_task,"UART",100,NULL,configMAX_PRIORITIES-1,NULL);

	vTaskStartScheduler();
	for (;;);
	return 0;
}

// End
