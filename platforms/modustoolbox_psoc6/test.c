#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "xpt2046.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define SYSTICK_MAX_CNT			(1 << 24) - 1		// max value for 24-bit reload register
// #define ENABLE_TOUCH_CALIBRATION
/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static void isr_systick(void);
uint32_t get_micros(void);
uint32_t get_millis(void);
static inline __attribute__((always_inline)) uint32_t get_ticks(void);
/*******************************************************************************
* Global Variables
*******************************************************************************/
volatile uint32_t systick_wrap = 0;	// SysTick wraps every 1s (NOTE: uint64_t not supported!!)
cyhal_spi_t mSPI;
/*******************************************************************************
* Function Name: main
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    /* Initialize the User LED */
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* Initialize CS pins */
	cyhal_gpio_init(TOUCH_CS, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);
	cyhal_gpio_init(DISP_CS, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("****************** "
           "XPT2046 Touch Test! "
           "****************** \r\n\n");


    // CLK_TIMER set to 1MHz using device configurator
    // Interrupt fires after systick counter reached the end
    // Note: Device Configurator shows wrong freq for CLK_TIMER.
    // Actual freq of CLK_TIMER = Source Clk / Divider
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_TIMER, SYSTICK_MAX_CNT);
    Cy_SysTick_SetCallback(0, isr_systick);

    uint16_t cal_params[5] = {212, 3653, 349, 3578, 4};

#ifdef ENABLE_TOUCH_CALIBRATION
    xpt2046_calibrate_touch(cal_params);
    printf("uint16_t cal_params[5] = {%d", cal_params[0]);
    for (uint8_t i = 1; i < 5; i++)
    {
    	printf(", %d", cal_params[i]);
    }
    printf("}\r\n");
#endif

    xpt2046_set_touch(cal_params);

    uint16_t x, y;
    uint32_t time = 0;
    for (;;)
    {
    	bool pressed = xpt2046_get_touch(&x, &y);
    	if (pressed)
    	{
    		printf("X: %d   Y: %d\r\n", x, y);
    	}
    	else
    	{
    		x = -1;
			y = -1;
    	}

    	uint32_t tmp_ms = get_millis();
    	if (tmp_ms >= time)
		{
    		cyhal_gpio_toggle(CYBSP_USER_LED);
			time = tmp_ms + (pressed ? 50 : 500);
		}
    }
}

// See: https://community.infineon.com/t5/PSoC-6/SysTick-for-timing-measurement/m-p/347523#M12764
static inline __attribute__((always_inline)) uint32_t get_ticks(void)
{
	// SYSTICK_MAX_CNT - SYSTICK_VAL --> Since SysTick is down counting
	// systick_wrap * (1 + SYSTICK_MAX_CNT) ---> Number of previous ticks
	return ((SYSTICK_MAX_CNT - SYSTICK_VAL) + (systick_wrap * (1 + SYSTICK_MAX_CNT)));
}

uint32_t get_micros(void)
{
	// As systick clock=1MHz, 1 tick per per microsecond.
	return get_ticks();
}

uint32_t get_millis(void)
{
	// As systick clock=1MHz, 1000 ticks per per millisecond.
	return (get_ticks()/1000);
}

void isr_systick(void)
{
    ++systick_wrap;
}

/* [] END OF FILE */
