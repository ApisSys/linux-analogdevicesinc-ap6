/**
 * @file ap6-drv.h
 *
 *  @file	1 mars 2013
 *  @author	mdelacour
 */

#include <linux/types.h>
#include "ap6-drv_interface.h"

#ifndef AP6_DRV__H_
#define AP6_DRV__H_


/**
 * @brief Apissys Driver Main Name
 */
#define AP6_DRIVER_NAME 	"ap6driver"

/**
 * @brief Retrieve driver info
 * @return driver info
 */
ap6driverInfo * getDriverInfo(void);

#endif /* AP6_DRV__H_ */
