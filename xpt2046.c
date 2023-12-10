#include <stdint.h>
#include <stdlib.h>
#include "xpt2046.h"


#ifndef false
  #define false 0U
#endif
#ifndef true
  #define true 1U
#endif

// Global var: Touch threshold
static uint16_t touchThreshold = Z_THRESHOLD;
// Initialise with example calibration values so processor does not crash if xpt2046_xpt2046_setTouch() not called in xpt2046_setup()
static uint16_t touchCalibration_x0 = 300, touchCalibration_x1 = 3600, touchCalibration_y0 = 300, touchCalibration_y1 = 3600;
static uint8_t  touchCalibration_rotate = 1, touchCalibration_invert_x = 2, touchCalibration_invert_y = 0;

static uint64_t _pressTime;        // Press and hold time-out
static uint16_t _pressX, _pressY;  // For future use (last sampled calibrated coordinates)

static uint16_t _width = DISPLAY_WIDTH, _height = DISPLAY_HEIGHT;

static uint8_t spi_needs_init = true;

/***************************************************************************************
** Function name:           _xpt2046_begin_touch_read_write - was xpt2046_spi_begin_touch
** Description:             Start transaction and select touch controller
***************************************************************************************/
// The touch controller has a low SPI clock rate
static inline __attribute__((always_inline)) void _xpt2046_begin_touch_read_write(void)
{
  PLATFORM_DMA_BUSY_WAIT();
  if (spi_needs_init)
  {
	xpt2046_platform_spi_init(SPI_TOUCH_FREQ, SPI_CPOL, SPI_CPHA, IS_LSBFIRST); // Freq: 2MHz (default), CPOL: 0, CPHA: 0, Bit order: MSB First
	spi_needs_init = false;
  }
#ifdef SPI_IS_SHARED
  PLATFORM_DISP_CS_HIGH(); // Just in case display's CS has been left low
#endif
  PLATFORM_TOUCH_CS_LOW();
}

/***************************************************************************************
** Function name:           _xpt2046_end_touch_read_write - was xpt2046_spi_end_touch
** Description:             End transaction and deselect touch controller
***************************************************************************************/
static inline __attribute__((always_inline)) void _xpt2046_end_touch_read_write(void)
{
  PLATFORM_TOUCH_CS_HIGH();
#ifdef SPI_IS_SHARED
  xpt2046_platform_spi_deinit();
  spi_needs_init = true;
#endif
}


/***************************************************************************************
** Function name:           xpt2046_get_touch_raw
** Description:             read raw touch position.  Always returns true.
***************************************************************************************/
uint8_t xpt2046_get_touch_raw(uint16_t *x, uint16_t *y)
{
  uint16_t tmp;

  _xpt2046_begin_touch_read_write();
  
  // Start YP sample request for x position, read 4 times and keep last sample
  xpt2046_platform_spi_transfer(0xd0);                    // Start new YP conversion
  xpt2046_platform_spi_transfer(0);                       // Read first 8 bits
  xpt2046_platform_spi_transfer(0xd0);                    // Read last 8 bits and start new YP conversion
  xpt2046_platform_spi_transfer(0);                       // Read first 8 bits
  xpt2046_platform_spi_transfer(0xd0);                    // Read last 8 bits and start new YP conversion
  xpt2046_platform_spi_transfer(0);                       // Read first 8 bits
  xpt2046_platform_spi_transfer(0xd0);                    // Read last 8 bits and start new YP conversion

  tmp = xpt2046_platform_spi_transfer(0);                   // Read first 8 bits
  tmp = tmp <<5;
  tmp |= 0x1f & (xpt2046_platform_spi_transfer(0x90)>>3);   // Read last 8 bits and start new XP conversion

  *x = tmp;

  // Start XP sample request for y position, read 4 times and keep last sample
  xpt2046_platform_spi_transfer(0);                       // Read first 8 bits
  xpt2046_platform_spi_transfer(0x90);                    // Read last 8 bits and start new XP conversion
  xpt2046_platform_spi_transfer(0);                       // Read first 8 bits
  xpt2046_platform_spi_transfer(0x90);                    // Read last 8 bits and start new XP conversion
  xpt2046_platform_spi_transfer(0);                       // Read first 8 bits
  xpt2046_platform_spi_transfer(0x90);                    // Read last 8 bits and start new XP conversion

  tmp = xpt2046_platform_spi_transfer(0);                 // Read first 8 bits
  tmp = tmp <<5;
  tmp |= 0x1f & (xpt2046_platform_spi_transfer(0)>>3);    // Read last 8 bits

  *y = tmp;

  _xpt2046_end_touch_read_write();

  return true;
}

/***************************************************************************************
** Function name:           xpt2046_get_touch_raw_z
** Description:             read raw pressure on touchpad and return Z value. 
***************************************************************************************/
uint16_t xpt2046_get_touch_raw_z(void)
{

  _xpt2046_begin_touch_read_write();

  // Z sample request
  int16_t tz = 0xFFF;
  xpt2046_platform_spi_transfer(0xb0);
  // Start new Z1 conversion
  // TODO: remove transfer16 and use transfer instead
  tz += xpt2046_platform_spi_transfer16(0xc0) >> 3;  // Read Z1 and start Z2 conversion
  tz -= xpt2046_platform_spi_transfer16(0x00) >> 3;  // Read Z2

  _xpt2046_end_touch_read_write();

  if (tz == 4095) tz = 0;

  return (uint16_t)tz;
}

/***************************************************************************************
** Function name:           _xpt2046_valid_touch
** Description:             read validated position. Return false if not pressed. 
***************************************************************************************/
#define _RAWERR 20 // Deadband error allowed in successive position samples
uint8_t _xpt2046_valid_touch(uint16_t *x, uint16_t *y, uint16_t threshold)
{
  uint16_t x_tmp, y_tmp, x_tmp2, y_tmp2;

  // Wait until pressure stops increasing to debounce pressure
  uint16_t z1 = 1;
  uint16_t z2 = 0;
  while (z1 > z2)
  {
    z2 = z1;
    z1 = xpt2046_get_touch_raw_z();
    xpt2046_platform_delay(1);
  }

  if (z1 <= threshold) return false;
    
  xpt2046_get_touch_raw(&x_tmp,&y_tmp);

  xpt2046_platform_delay(1); // Small xpt2046_platform_delay to the next sample
  if (xpt2046_get_touch_raw_z() <= threshold) return false;

  xpt2046_platform_delay(2); // Small xpt2046_platform_delay to the next sample
  xpt2046_get_touch_raw(&x_tmp2,&y_tmp2);

  if (abs(x_tmp - x_tmp2) > _RAWERR) return false;
  if (abs(y_tmp - y_tmp2) > _RAWERR) return false;
  
  *x = x_tmp;
  *y = y_tmp;
  
  return true;
}
  
/***************************************************************************************
** Function name:           xpt2046_get_touch
** Description:             read callibrated position. Return false if not pressed. 
***************************************************************************************/
uint8_t xpt2046_get_touch(uint16_t *x, uint16_t *y)
{
  uint16_t x_tmp, y_tmp;
  
  uint16_t tmp_threshold = touchThreshold;
  if (tmp_threshold<20)
	  tmp_threshold = 20;

  // To make touch-and-drag easier
  if (_pressTime > xpt2046_platform_get_millis())
	  tmp_threshold=20;

  uint8_t n = 5;
  uint8_t valid = 0;
  while (n--)
  {
    if (_xpt2046_valid_touch(&x_tmp, &y_tmp, tmp_threshold))
    	valid++;
  }
  if (valid<1)
  {
	  _pressTime = 0;
	  return false;
  }
  _pressTime = xpt2046_platform_get_millis() + 50;

  xpt2046_convert_raw_xy(&x_tmp, &y_tmp);

  if (x_tmp >= _width || y_tmp >= _height)
	  return false;

  _pressX = x_tmp;
  _pressY = y_tmp;
  *x = _pressX;
  *y = _pressY;
  return valid;
}

/***************************************************************************************
** Function name:           xpt2046_convert_raw_xy
** Description:             convert raw touch x,y values to screen coordinates 
***************************************************************************************/
void xpt2046_convert_raw_xy(uint16_t *x, uint16_t *y)
{
  uint16_t x_tmp = *x, y_tmp = *y, xx, yy;

  if(!touchCalibration_rotate){
    xx=(x_tmp-touchCalibration_x0)*_width/touchCalibration_x1;
    yy=(y_tmp-touchCalibration_y0)*_height/touchCalibration_y1;
    if(touchCalibration_invert_x)
      xx = _width - xx;
    if(touchCalibration_invert_y)
      yy = _height - yy;
  } else {
    xx=(y_tmp-touchCalibration_x0)*_width/touchCalibration_x1;
    yy=(x_tmp-touchCalibration_y0)*_height/touchCalibration_y1;
    if(touchCalibration_invert_x)
      xx = _width - xx;
    if(touchCalibration_invert_y)
      yy = _height - yy;
  }
  *x = xx;
  *y = yy;
}

/***************************************************************************************
** Function name:           xpt2046_calibrate_touch
** Description:             generates calibration parameters for touchscreen.
*                           Arg must be an array of 5 items (e.g. uint16_t params[5])
***************************************************************************************/
void xpt2046_calibrate_touch(uint16_t parameters[5])
{
  int16_t values[] = {0,0,0,0,0,0,0,0};
  uint16_t x_tmp, y_tmp;

  xpt2046_platform_print("Touch corners of the display as per the prompts...\r\n");

  for(uint8_t i = 0; i<4; i++){
//     fillRect(0, 0, size+1, size+1, color_bg);
//     fillRect(0, _height-size-1, size+1, size+1, color_bg);
//     fillRect(_width-size-1, 0, size+1, size+1, color_bg);
//     fillRect(_width-size-1, _height-size-1, size+1, size+1, color_bg);

    if (i == 5) break; // used to clear the arrows

    switch (i)
    {
      case 0: // up left
        xpt2046_platform_print("1. Touch TOP LEFT corner..");
        break;
      case 1: // bot left
        xpt2046_platform_print("2. Touch BOTTOM LEFT corner..");
        break;
      case 2: // up right
        xpt2046_platform_print("3. Touch TOP RIGHT corner..");
        break;
      case 3: // bot right
        xpt2046_platform_print("4. Touch BOTTOM RIGHT corner..");
        break;
    }

    // user has to get the chance to release
    if(i>0)
    	xpt2046_platform_delay(1000);

    for(uint8_t j=0; j<8; j++)
    {
      // Use a lower detect threshold as corners tend to be less sensitive
      while(!_xpt2046_valid_touch(&x_tmp, &y_tmp, Z_THRESHOLD/2));
      values[i*2  ] += x_tmp;
      values[i*2+1] += y_tmp;
    }
    xpt2046_platform_print(" [DONE] \r\n");
    values[i*2  ] /= 8;
    values[i*2+1] /= 8;
  }


  // from case 0 to case 1, the y value changed. 
  // If the measured delta of the touch x axis is bigger than the delta of the y axis, the touch and TFT axes are switched.
  touchCalibration_rotate = false;
  if(abs(values[0]-values[2]) > abs(values[1]-values[3])){
    touchCalibration_rotate = true;
    touchCalibration_x0 = (values[1] + values[3])/2; // calc min x
    touchCalibration_x1 = (values[5] + values[7])/2; // calc max x
    touchCalibration_y0 = (values[0] + values[4])/2; // calc min y
    touchCalibration_y1 = (values[2] + values[6])/2; // calc max y
  } else {
    touchCalibration_x0 = (values[0] + values[2])/2; // calc min x
    touchCalibration_x1 = (values[4] + values[6])/2; // calc max x
    touchCalibration_y0 = (values[1] + values[5])/2; // calc min y
    touchCalibration_y1 = (values[3] + values[7])/2; // calc max y
  }

  // in addition, the touch screen axis could be in the opposite direction of the TFT axis
  touchCalibration_invert_x = false;
  if(touchCalibration_x0 > touchCalibration_x1){
    values[0]=touchCalibration_x0;
    touchCalibration_x0 = touchCalibration_x1;
    touchCalibration_x1 = values[0];
    touchCalibration_invert_x = true;
  }
  touchCalibration_invert_y = false;
  if(touchCalibration_y0 > touchCalibration_y1){
    values[0]=touchCalibration_y0;
    touchCalibration_y0 = touchCalibration_y1;
    touchCalibration_y1 = values[0];
    touchCalibration_invert_y = true;
  }

  // pre calculate
  touchCalibration_x1 -= touchCalibration_x0;
  touchCalibration_y1 -= touchCalibration_y0;

  if(touchCalibration_x0 == 0) touchCalibration_x0 = 1;
  if(touchCalibration_x1 == 0) touchCalibration_x1 = 1;
  if(touchCalibration_y0 == 0) touchCalibration_y0 = 1;
  if(touchCalibration_y1 == 0) touchCalibration_y1 = 1;

  // export parameters, if pointer valid
  if(parameters != NULL){
    parameters[0] = touchCalibration_x0;
    parameters[1] = touchCalibration_x1;
    parameters[2] = touchCalibration_y0;
    parameters[3] = touchCalibration_y1;
    parameters[4] = touchCalibration_rotate | (touchCalibration_invert_x <<1) | (touchCalibration_invert_y <<2);
  }
}


/***************************************************************************************
** Function name:           xpt2046_set_touch
** Description:             imports calibration parameters for touchscreen. 
*                           Arg must be an array of 5 items (e.g. uint16_t params[5])
***************************************************************************************/
void xpt2046_set_touch(uint16_t parameters[5])
{
  touchCalibration_x0 = parameters[0];
  touchCalibration_x1 = parameters[1];
  touchCalibration_y0 = parameters[2];
  touchCalibration_y1 = parameters[3];

  if(touchCalibration_x0 == 0) touchCalibration_x0 = 1;
  if(touchCalibration_x1 == 0) touchCalibration_x1 = 1;
  if(touchCalibration_y0 == 0) touchCalibration_y0 = 1;
  if(touchCalibration_y1 == 0) touchCalibration_y1 = 1;

  touchCalibration_rotate = parameters[4] & 0x01;
  touchCalibration_invert_x = parameters[4] & 0x02;
  touchCalibration_invert_y = parameters[4] & 0x04;
}


/***************************************************************************************
** Function name:           xpt2046_set_touch_threshold
** Description:             Sets touch threshold to detect touch.
***************************************************************************************/
void xpt2046_set_touch_threshold(uint16_t threshold)
{
  touchThreshold = threshold;
}
