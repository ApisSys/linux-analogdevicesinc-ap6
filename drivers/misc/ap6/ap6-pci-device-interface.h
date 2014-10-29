/*
 * ap6-pci-device-interface.h
 *
 *  Created on: 5 mars 2013
 *      Author: mdelacour
 */

#ifndef AP6_PCI_DEVICE_INTERFACE_H_
#define AP6_PCI_DEVICE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief PCI device Location
 */
typedef struct
{
	u16 domain;			/** PCI domain (host bridge) 	*/
	u8  bus;			/** PCI bus						*/
	u8  dev;			/** PCI device					*/
	u8  func;			/** PCI function				*/
} ap6pciDeviceLocation;


/**
 * @brief PCI device description
 */
typedef struct
{
	u16 VendorId;				/** PCI Vendor Id 	*/
	u16 DeviceId;				/** PCI Device Id 	*/
	u16 ClassId;				/**	PCI Class Id 	*/
	u16 FunctionId;				/** PCI Function Id	*/
} ap6pciDeviceDescription;


/**
 * @brief PCI device interface
 */
typedef struct{
	ap6pciDeviceLocation location;			/** PCI location 			*/
	ap6pciDeviceDescription description;	/** PCI description			*/
	u32 	id;								/** device interface id		*/
	u32 	address;						/** mapped address			*/
	u32 	size;							/** mapped size				*/
	char 			devicePath[256];		/** character device path	*/
}ap6pciDeviceInterface;

#ifdef __cplusplus
	}
#endif //__cplusplus

#endif /* AP6_PCI_DEVICE_INTERFACE_H_ */
