// Coded by Bodmer 10/2/18, see license in root directory.
// This is part of the TFT_eSPI class and is associated with the Touch Screen handlers

#ifndef _XPT2046_H_
#define _XPT2046_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "platform_select.h"

// #define SPI_IS_SHARED

#ifndef DISPLAY_WIDTH
    #define DISPLAY_WIDTH    320
#endif
#ifndef DISPLAY_HEIGHT
    #define DISPLAY_HEIGHT   240
#endif
#ifndef SPI_TOUCH_FREQ
    #define SPI_TOUCH_FREQ 2000000UL    /* 2MHz */
#endif

// Define a default pressure threshold for validating touches
#ifndef Z_THRESHOLD
  #define Z_THRESHOLD 350
#endif

/* Mode: 0 (CPOL 0, CPHA 0 */
#define SPI_CPOL        0
#define SPI_CPHA        0
#define IS_LSBFIRST     0   /* Bit order: MSB First */



// Public

// Get raw x,y ADC values from touch controller
uint8_t  xpt2046_get_touch_raw(uint16_t *x, uint16_t *y);

// Get raw z (i.e. pressure) ADC value from touch controller
uint16_t xpt2046_get_touch_raw_z(void);

// Convert raw x,y values to calibrated and correctly rotated screen coordinates
void     xpt2046_convert_raw_xy(uint16_t *x, uint16_t *y);

// Get the screen touch coordinates, returns true if screen has been touched
// if the touch coordinates are off screen then x and y are not updated
// The returned value can be treated as a bool type, false or 0 means touch not detected
// In future the function may return an 8 "quality" (jitter) value.
uint8_t  xpt2046_get_touch(uint16_t *x, uint16_t *y);

// Run screen calibration and test, writes calibration values to passed array of 5 elements
// Arg must be an array of 5 items (e.g. uint16_t params[5])
void     xpt2046_calibrate_touch(uint16_t data[5]);

// Set the screen calibration values
// Arg must be an array of 5 items (e.g. uint16_t params[5])
void     xpt2046_set_touch(uint16_t data[5]);

// Set touch threshold to detect touch
void     xpt2046_set_touch_threshold(uint16_t threshold);


// Private

// Function to validate a touch, allow settle time and reduce spurious coordinates
uint8_t  _xpt2046_valid_touch(uint16_t *x, uint16_t *y, uint16_t threshold);

#ifdef __cplusplus
}
#endif

#endif /* _XPT2046_H_ */
