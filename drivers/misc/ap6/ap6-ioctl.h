/*
 * ap6-ioctl.h
 *
 *  Created on: 20 févr. 2013
 *      Author: mdelacour
 */

#ifndef AP6_IOCTL_H_
#define AP6_IOCTL_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>


#define AP6_NB_DEVS	1	/** Default number of device files */

#define DATA_SIZE 200	/** Default data size */


#ifdef __INDEXER__
#include <module_indexer_compat.h>
#endif


/**
 * @brief main IOCTL init
 * @return 0->success
 */
int ap6_ioctl_init(void);

/**
 * @brief main IOCTL exit
 */
void ap6_ioctl_exit(void);




#endif /* AP6_IOCTL_H_ */
