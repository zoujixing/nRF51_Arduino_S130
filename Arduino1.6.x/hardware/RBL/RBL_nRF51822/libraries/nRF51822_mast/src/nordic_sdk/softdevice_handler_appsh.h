/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef SOFTDEVICE_HANDLER_APPSH_H
#define SOFTDEVICE_HANDLER_APPSH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "softdevice_handler.h"
#include <stdint.h>

#define SOFTDEVICE_HANDLER_APPSH_INIT(CLOCK_SOURCE,USE_SCHEDULER) \
    SOFTDEVICE_HANDLER_INIT(CLOCK_SOURCE,(USE_SCHEDULER) ? softdevice_evt_schedule : NULL)

uint32_t softdevice_evt_schedule(void);

#ifdef __cplusplus
}
#endif

#endif //SOFTDEVICE_HANDLER_APPSH_H
