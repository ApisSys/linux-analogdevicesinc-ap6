/*
 * ap6-pci-kmod.c
 *
 *  Created on: 18 févr. 2013
 *      Author: mdelacour
 */

#include "ap6-pci-general.h"
#include "ap6-pci-kmod.h"
#include "ap6-pci-devices.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>

/**
 * @brief Probe affected device (Callback)
 */
static s32 ap6_pci_probe(struct pci_dev *dev, const struct pci_device_id *id){

	printk( KERN_INFO "AP6 PCI driver probe for device %x:%x\n", dev->vendor, dev->device);
	s32 grc=0;
	s32 rcen = pci_enable_device(dev);
	if(rcen>=0){
		s32 rcrpd = registerPciDevice(dev,id);
		if(rcrpd<0)grc=rcrpd;
	}else{
		printk( KERN_ALERT "pci_enable_device failed with return code: %d \n",rcen);
		grc=rcen;
	}
	return grc;
}

/**
 * @brief Remove from device (Callback)
 */
static void ap6_pci_remove(struct pci_dev *dev){
	printk( KERN_INFO "AP6 PCI driver unregistering device %x:%x...\n", dev->vendor, dev->device);
	unregisterPciDevice(dev);

	printk( KERN_INFO "AP6 PCI driver remove for device %x:%x\n", dev->vendor, dev->device);
}


#define AP6PCIVENDORID_TEST			0x8086 	/** Apissys PCI Vendor Id */ /** FOR TEST PURPOSES */

#define AP6PCIDEVICEID_TEST0		0 		/** Apissys PCI Device Id 0 */

#define AP6PCIDEVICEID_TEST1		0x100e 	/** TEST ONLY : Eth controller */


#define AP6PCIVENDORID0				0x10ee

#define AP6PCIDEVICEID0				0x7014



/**
 * @brief Compatible Device Ids Table
 */
static struct pci_device_id ap6_pci_ids[]={
	{PCI_DEVICE(AP6PCIVENDORID_TEST,AP6PCIDEVICEID_TEST0)},
	{PCI_DEVICE(AP6PCIVENDORID_TEST,AP6PCIDEVICEID_TEST1)},
	{PCI_DEVICE(AP6PCIVENDORID0,AP6PCIDEVICEID0)}
};


/**
 * @brief PCI Driver Configuration Structure
 */
static struct pci_driver pci_driver = {
	.name= AP6_PCI_DRIVER_NAME,
	.id_table = ap6_pci_ids,
	.probe = ap6_pci_probe,
	.remove = ap6_pci_remove
};


/**
 * @brief Terminate PCI driver
 * @return : 1 for success, 0 instead
 */
s32 ap6PciInit(){
	printk( KERN_INFO "AP6 PCI init\n");

	register_pci_class();
	printk( KERN_INFO "AP6 PCI class registered\n");

	s32 rcreg = pci_register_driver(&pci_driver);
	printk( KERN_INFO "AP6 PCI registered\n");

	initPciDevices();

	return (rcreg>=0);
}



/**
 * @brief Init PCI driver
 */
void ap6_pci_exit(){
	printk( KERN_INFO "AP6 PCI exit\n");

	pci_unregister_driver(&pci_driver);
	printk( KERN_INFO "AP6 PCI unregistered\n");

	unregister_pci_class();
	printk( KERN_INFO "AP6 PCI class unregistered\n");

	printk( KERN_INFO "AP6 PCI unregister\n");
}




