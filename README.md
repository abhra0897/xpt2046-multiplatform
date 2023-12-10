## XPT2046 Touch Library for any platform!

This is a touch library for XPT2046 IC used for resistive touch screens. XPT2046 is a very popular touch controller IC and can be found in regular LCD displays that are used in embedded systems projects.

- This library is (mostly) platform-independent.
- Hardware specific calls are seperated from main library.
- Very easy to add any hardware platform support by providing a few function definitions and macro definitions that are called/used by this library.

### How to Add Platform Supports:

Adding platform support is super easy! Go to the [generic_template](platforms/generic_template) directory and read `platform_generic.h` and `platform_generic.c` files there. You need to implement the functions and macros given there. That's all! Those functions and macros are called by this library.

So, create `platform_<name_of_platform>.c` and `platform_<name_of_platform>.h` files with all the mandatory functions and macros. After creating your own platform files based on the template, add an entry to the `platform_select.h` file. When using your newly created platform, simply uncomment that entry in the `platform_select.h` and comment out the rest.

**Note: Your main application only needs to include `xpt2046.h` file and not the platform files.**

### How to Use:

1. Select your platform in `platform_select.h` file by uncommenting the target platform and commenting out the rest.
2. Include `xpt2046.h` in the main application.
3. Define extern global variables that are used by your platform files, in your main application.
4. (optional) Call `xpt2046_calibrate_touch()` if you are running it for the first time. Store the parameters in an array and use it for the consecutive builds/runs.
5. Call `xpt2046_get_touch()` to get calibrated and filtered touch position data.
6. Enjoy!

### API:

```C
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
```

### Tests:

This library is tested on following platforms so far:

| SDK/Platform | RTOS   | Devkit/EVK/Board    | MCU Family | Platform Dir                                                                 |
|--------------|--------|---------------------|------------|------------------------------------------------------------------------------|
| ModusToolbox | None   | CY8CKIT-062S2-43012 | PSoC6      | [platforms/modustoolbox_psoc6](platforms/modustoolbox_psoc6)                 |
| nRF Connect  | Zephyr | nRF52840-DK         | nRF52      | [platforms/nrfconnectsdk_zephyr_nrf52](platforms/nrfconnectsdk_zephyr_nrf52) |

**See [generic_template](platforms/generic_template/platform_generic.h) to see how to add your platform.**
