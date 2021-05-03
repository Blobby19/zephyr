/*
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_PM_DEVICE_H_
#define ZEPHYR_INCLUDE_PM_DEVICE_H_

#include <kernel.h>
#include <sys/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Device Power Management API
 *
 * @defgroup device_power_management_api Device Power Management API
 * @ingroup power_management_api
 * @{
 */

struct device;

/** @def PM_DEVICE_ACTIVE_STATE
 *
 * @brief device is in ACTIVE power state
 *
 * @details Normal operation of the device. All device context is retained.
 */
#define PM_DEVICE_ACTIVE_STATE          1

/** @def PM_DEVICE_LOW_POWER_STATE
 *
 * @brief device is in LOW power state
 *
 * @details Device context is preserved by the HW and need not be
 * restored by the driver.
 */
#define PM_DEVICE_LOW_POWER_STATE       2

/** @def PM_DEVICE_SUSPEND_STATE
 *
 * @brief device is in SUSPEND power state
 *
 * @details Most device context is lost by the hardware.
 * Device drivers must save and restore or reinitialize any context
 * lost by the hardware
 */
#define PM_DEVICE_SUSPEND_STATE         3

/** @def PM_DEVICE_FORCE_SUSPEND_STATE
 *
 * @brief device is in force SUSPEND power state
 *
 * @details Driver puts the device in suspended state after
 * completing the ongoing transactions and will not process any
 * queued work or will not take any new requests for processing.
 * Most device context is lost by the hardware. Device drivers must
 * save and restore or reinitialize any context lost by the hardware.
 */
#define PM_DEVICE_FORCE_SUSPEND_STATE	4

/** @def PM_DEVICE_OFF_STATE
 *
 * @brief device is in OFF power state
 *
 * @details - Power has been fully removed from the device.
 * The device context is lost when this state is entered, so the OS
 * software will reinitialize the device when powering it back on
 */
#define PM_DEVICE_OFF_STATE             5

/* Constants defining support device power commands */
#define PM_DEVICE_SET_POWER_STATE       1
#define PM_DEVICE_GET_POWER_STATE       2

typedef void (*pm_device_cb)(const struct device *dev,
			     int status, void *context, void *arg);

/**
 * @brief Device PM info
 */
struct pm_device {
	/** Pointer to the device */
	const struct device *dev;
	/** Lock to synchronize the get/put operations */
	struct k_sem lock;
	/* Following are packed fields protected by #lock. */
	/** Device pm enable flag */
	bool enable : 1;
	/* Following are packed fields accessed with atomic bit operations. */
	atomic_t atomic_flags;
	/** Device usage count */
	atomic_t usage;
	/** Device idle internal power state */
	atomic_t fsm_state;
	/** Work object for asynchronous calls */
	struct k_work work;
	/** Event object to listen to the sync request events */
	struct k_poll_event event;
	/** Signal to notify the Async API callers */
	struct k_poll_signal signal;
};

/** Bit position in device_pm::atomic_flags that records whether the
 * device is busy.
 */
#define PM_DEVICE_ATOMIC_FLAGS_BUSY_BIT 0

/**
 * @brief Get name of device PM state
 *
 * @param state State id which name should be returned
 */
const char *pm_device_state_str(uint32_t state);

/** Alias for legacy use of device_pm_control_nop */
#define device_pm_control_nop __DEPRECATED_MACRO NULL

/** @} */

#ifdef __cplusplus
}
#endif

#endif