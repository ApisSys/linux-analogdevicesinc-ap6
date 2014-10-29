/**
 * @mainpage ap6driver documentation
 *
 * 	@section intro_sec Introduction
 *		This is the Apissys Linux Driver Documentation
 *
 *	@section install_sec Installation
 *
 *	This driver is delivered in DKMS format
 *
 *		./src : 			source files<br>
 *		./dkms.conf :		DKMS configuration file<br>
 *		./kernutils.mk:		Master kernel module makefile<br>
 *		./Makefile : 		ap6driver specific makefile<br>
 *		./install.sh:		DKMS post installation script<br>
 *		./local_install.sh:	Installation script for non-DKMS install<br>
 *		./remove.sh:		Module removal script<br>
 *
 */



/**
 * @file main.c
 *
 *  @date	18 févr. 2013
 *  @author mdelacour
 */

#include "ap6-drv.h"
#include "ap6-pci-kmod.h"
#include "ap6-ioctl.h"
#include "ap6-drv-version.h"

#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h> 	/* class_create */
#include <linux/slab.h> 	/* kmalloc */
#include <linux/errno.h>	/* standard error codes */
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>

#ifdef __INDEXER__
#include <module_indexer_compat.h>
#endif

MODULE_AUTHOR("Apissys SAS");
MODULE_DESCRIPTION("AP6 Linux Driver");
MODULE_VERSION(VERSION_STRING);
MODULE_LICENSE("GPL");


/*
 * Driver Information Initialization
 */
ap6driverInfo driverInfo = {
	.version = {
		.minor=VERSION_MINOR,
		.major=VERSION_MAJOR,
		.str=VERSION_STRING
	},
	.name = "ap6driver",
	.capability={
		{.active=1,.name="PCI"}
	}
};



/**
 * Retrieve Driver Info
 * @return: ap6driverInfo instance
 */
ap6driverInfo * getDriverInfo(void){
	return &driverInfo;
}


/**
 * Module Init callback
 * @return:  0-> success
 */
static int __init ap6_pci_kmod__init(void){
	printk(KERN_ALERT "AP6 Linux Driver installed ! \n");

	int rcioctl = ap6_ioctl_init();
	printk(KERN_ALERT "AP6 Linux Driver ioctl init %s \n",(rcioctl==0)?("ok"):("ko"));

	int rcpci = ap6PciInit();
	printk(KERN_ALERT "AP6 Linux Driver pci init %s \n",(rcpci)?("ok"):("ko"));

//	int rcpcifake = ap6_pci_fake_init();
//	printk(KERN_ALERT "AP6 Linux Driver fake pci init %s \n",(rcpcifake)?("ok"):("ko"));

	return 0;
}


/**
 * Module exit callback
 */
static void __exit ap6_pci_kmod__exit(void){

	printk(KERN_ALERT "AP6 LInux Calling IOCTL exit !\n");
	ap6_ioctl_exit();
	printk(KERN_ALERT "AP6 LInux Calling PCI exit !\n");
	ap6_pci_exit();

//	ap6_pci_fake_exit();

	printk(KERN_ALERT "AP6 LInux Driver removed !\n");
}


module_init(ap6_pci_kmod__init);
module_exit(ap6_pci_kmod__exit);
