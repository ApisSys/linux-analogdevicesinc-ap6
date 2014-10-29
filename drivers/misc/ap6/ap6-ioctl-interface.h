/**
 * @file ap6-ioctl-interface.h
 * @author mdelacour
 */

#ifndef AP6_IOCTL_INTERFACE_H_
#define AP6_IOCTL_INTERFACE_H_


#ifdef __cplusplus
extern "C" {
#endif



#define MAGIC	'x'
/**	Global driver IOCTL Read Command 		*/
#define AP6_IOCTL_READ				_IOR(MAGIC,0,int)

/**	Global driver IOCTL Write Command 		*/
#define AP6_IOCTL_WRITE				_IOW(MAGIC,1,int)

/**	Global driver IOCTL Signal Command 		*/
#define AP6_IOCTL_SIG				_IOR(MAGIC,2,int)

/**	Global driver IOCTL Info Command 		*/
#define AP6_IOCTL_DRV_INFO			_IOR(MAGIC,3,int)



// global PCI IOCTL
/** Global driver PCI Init IOCTL command 		*/
#define AP6_IOCTL_PCI_INIT			_IOR(MAGIC,100,int)

/** Global driver PCI Exit IOCTL command 		*/
#define AP6_IOCTL_PCI_EXIT			_IOR(MAGIC,101,int)

/** Global driver PCI Enumerate IOCTL command 	*/
#define AP6_IOCTL_PCI_ENUM			_IOR(MAGIC,102,int)

/** Global driver PCI Nb devices IOCTL command 	*/
#define AP6_IOCTL_PCI_NBDEV			_IOR(MAGIC,103,int)


// per PCI device IOCTL
/** Local PCI Device Info IOCTL command */
#define AP6_IOCTL_PCIDEV_INFO		_IOR(MAGIC,120,int)

#ifdef __cplusplus
	}
#endif //__cplusplus

#endif /* AP6_IOCTL_INTERFACE_H_ */
