// NOTE: Only to be included by xpt2046.h. User should NOT include it

// ---------------------------------------------------------
// Uncomment the target platform and comment out the rest
// ---------------------------------------------------------
#define PLATFORM_GENERIC_TEMPLATE
// #define PLATFORM_MODUSTOOLBOX_PSoC6
// #define PLATFORM_NRFCONNECTSDK_ZEPHYR_nRF52
// #define PLATFORM_PICOSDK_RP2040 // TBD


// ---------------------------------------------------------
// Include header file based on selected platform.
// Change header name/path as neccessary
// ---------------------------------------------------------
#if defined (PLATFORM_MODUSTOOLBOX_PSoC6)
    #include "platform_mtb_psoc6.h"

#elif defined (PLATFORM_NRFCONNECTSDK_ZEPHYR_nRF52)
    #include "platform_ncs_zephyr_nrf52.h"

#elif defined (PLATFORM_PICOSDK_RP2040) // TBD
    #include "platform_picosdk_rp2040.h"

#else
    #include "platform_generic.h"

#endif
// -----------------------------------------------------------//
