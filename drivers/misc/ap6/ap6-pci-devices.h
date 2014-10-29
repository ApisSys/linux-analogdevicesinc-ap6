/**
 * @file ap6-pci-devices.h
 *
 *  @date	26 févr. 2013
 *  @author mdelacour
 */

#ifndef AP6_PCI_DEVICES_H_
#define AP6_PCI_DEVICES_H_

#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/list.h>
#include "ap6-pci-general.h"
#include "ap6-pci-device-interface.h"


/**
 * @brief Number of PCI regions handled
 */
#define AP6_PCI_N_REGIONS	1


/**
 * @brief PCI Region descriptor
 */
typedef struct{
	u32 io_addr;				/** device io address */
	size_t io_len;				/** device io length */
	u32 io_flags;				/** device io flags */
	void * io_remap;			/** device io remap pointer */
}ap6_pci_region;

/**
 * @brief Apissys PCI Device Instance Block (DIB)
 * Structure instantiated for each Apissys PCI Board
 */
typedef struct{
	s32 active;										/**	active flag */

	u32 id;											/** id */

	struct pci_dev *pci_dev;						/** PCI device */
	struct pci_device_id *pci_id;					/** PCI device id */

	dev_t	cdevnum;								/** char device number */
	struct cdev cdev;								/** char device */

	char name[128];									/** device name */

	ap6_pci_region region[AP6_PCI_N_REGIONS];		/** PCI device regions */

	struct device * chardev;						/** character device  (/dev/ap6pci0-N)*/

	struct list_head lnode;							/** list node */

	ap6pciDeviceDescription description;
	ap6pciDeviceLocation location;

}ap6_pci_dib;


/**
 * @brief Init PCI devices
 */
void initPciDevices(void);


/**
 * @brief Get the number of PCI devices
 * @return number of PCI devices
 */
u32 getPciNbCharDevices(void);


/**
 * @brief Enumerate PCI devices
 * @return PCI device name
 */
char * enumPciDevices(void);


/**
 * @brief Register PCI class
 * @return
 */
s32 register_pci_class(void);

/**
 * @brief Unregister PCI class
 */
void unregister_pci_class(void);


/**
 * @brief Register PCI device
 * @param dev
 * @param id
 * @return 0->success
 */
s32 registerPciDevice(struct pci_dev *dev, const struct pci_device_id *id);

/**
 * @brief Unregister device
 * @param dev : device to unregister
 */
void unregisterPciDevice(struct pci_dev *dev);



#endif /* AP6_PCI_DEVICES_H_ */
