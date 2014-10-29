/*
 * ap6-pci-devices.c
 *
 *  Created on: 26 févr. 2013
 *      Author: mdelacour
 */

#ifndef AP6_PCI_DEVICES_C_
#define AP6_PCI_DEVICES_C_

#include "ap6-pci-devices.h"
#include "ap6-ioctl-interface.h"

#define N_MAX_PCI_DEVICES	21
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/list.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>

/**
 * @brief Check PCI memory access
 */
#define CHECKACCESS(MEMSIZE, offset,size) ((offset+size)<MEMSIZE)

/**
 * @brief Char device debug print formating macro
 */
#define CDEVFMT(cdev)	"CDEV:\n------------\ncount: %d\ndevt: %d\nowner: %s\n------------\n",(cdev).count, (cdev).dev,(cdev).owner->name

/**
 * @brief current id (used for enumeration
 */
u32 s_currentId=0;

/**
 * @brief Declares device list
 * @param ap6pci_devicelist
 */
LIST_HEAD(ap6pci_devicelist);

/**
 * @brief device enumeration cursor
 */
struct list_head * enumeration_cursor;

/**
 * @brief Current number of PCI character devices
 */
u32 nbCharDevices=0;

/**
 * @brief Get the number of PCI devices
 * @return number of PCI devices
 */
u32 getPciNbCharDevices(void){
	return nbCharDevices;
}


/**
 * @brief Init PCI devices
 */
void initPciDevices(void){
	enumeration_cursor = &ap6pci_devicelist;
}


/**
 * @brief Enumerate PCI devices
 * @return PCI device name
 */
char * enumPciDevices(void){
	char * name=NULL;
	struct list_head * tmp = enumeration_cursor->next;

	if(tmp){
		if(tmp==&ap6pci_devicelist){
			tmp = tmp->next;  // skip list head
		}
		ap6_pci_dib * dib = container_of(tmp,ap6_pci_dib,lnode);
			if(dib){
				name = dib->name;
				enumeration_cursor = &(dib->lnode);
			}
	}

	return name;
}

/**
 * @brief ap6pci[0-N] device read callback
 * 	@param fd: 	device file descriptor
 *
 *  @return : 	read block size
 */
static ssize_t ap6pci_devread (struct file *fd, char __user *userbuffer, size_t size, loff_t *offset){
	ap6_pci_dib *dib = (ap6_pci_dib *) fd->private_data;
	ssize_t ret_size=0;
	if(dib){

		const char * devName = dib->name;

		printk(KERN_INFO "%s : read access trial at 0x%x with size: %d\n", devName, (u32)*offset, size);

		if(dib->region[0].io_addr!=0){
			char *data = (char*)dib->region[0].io_remap;
			size_t dataSize = dib->region[0].io_len;
			if(CHECKACCESS(dataSize,*offset,size)){
				s32 rccpuser=0;
				s32 test = *((s32 *)(dib->region[0].io_remap));
				printk(KERN_INFO "ap6pci: io dereferencing trial:  %d\n",test);
			}else{
				printk(KERN_ERR "%s : read access failed at 0x%x\n", devName, *offset);
			}
		}else{
			printk(KERN_ERR "%s : read access failed at 0x%x: region not allocated\n", devName, *offset);
		}
	}


	return 0;
}


/**
 * @brief ap6pci[0-N] device write callback
 * @param fd:			character device file descriptor
 * @param userbuffer:	buffer from user space
 * @param size:			write size
 * @param offset:		write offset
 * @return written size
 */
static ssize_t ap6pci_devwrite (struct file *fd, const char __user *userbuffer, size_t size, loff_t *offset){

	ap6_pci_dib *dib = (ap6_pci_dib *) fd->private_data;
		if(dib){

			const char * devName = dib->name;

			printk(KERN_INFO "%s : write access trial at 0x%x\n", devName, (u32)*offset);

			if(dib->region[0].io_addr!=0){
				char *data = (char*)dib->region[0].io_remap;
				size_t dataSize = dib->region[0].io_len;
				if(CHECKACCESS(dataSize,*offset,size)){

				}else{
					printk(KERN_ERR "%s : write access failed at 0x%x\n", devName, *offset);
				}
			}else{

			}
		}
	return 0;
}


/**
 * @brief ap6pci[0-N] device open callback
 * @param inod : character device inode
 * @param fd:	 character device file descriptor
 * @return 0-> success
 */
static s32 ap6pci_devopen (struct inode *inod, struct file *fd){

	ap6_pci_dib *dib = container_of(inod->i_cdev, ap6_pci_dib, cdev);

	printk( KERN_INFO CDEVFMT(*(inod->i_cdev)));


	printk(KERN_INFO "cdev at 0x%x", inod->i_cdev);


	if(dib){

		if(dib->active!=1){
			printk(KERN_ERR "dib retrievied but corrupted (not active)\n");
		}else{
			printk(KERN_INFO "dib retrieved active\n");
			char * cdevName = dib->name;
			if(cdevName)printk(KERN_INFO "dib retrieved for cdev: %s\n",cdevName);
			else printk(KERN_ERR "dib retrievied but corrupted\n");
		}

	}else{
		printk(KERN_ERR "dib retrieval failed\n");
	}


	fd->private_data = dib; /* store dib in the file's privatedata for later use */


	return 0; //success !
}

/**
 * @brief ap6pci[0-N] device ioctl callback
 * @param fd: file descriptor
 * @param cmd: IOCTL command
 * @param arg: IOCTL command argument
 * @return 0->success
 */
static long ap6pci_ioctl(struct file *fd, u32 cmd, unsigned long arg){


	ap6_pci_dib *dib = (ap6_pci_dib *) fd->private_data;

	printk(KERN_INFO "private_data: %x\n",fd->private_data);


	if(dib){
		switch(cmd) {
			case AP6_IOCTL_PCIDEV_INFO:
			{
				ap6pciDeviceInterface pdi;
				memcpy(&(pdi.location), &(dib->location), sizeof(ap6pciDeviceLocation));
				memcpy(&(pdi.description), &(dib->description), sizeof(ap6pciDeviceDescription));
				pdi.address = dib->region[0].io_addr;
				pdi.size = dib->region[0].io_len;
				pdi.id = dib->id;
				printk(KERN_INFO "vendor: %d, device: %d \n", pdi.description.VendorId, pdi.description.DeviceId);
				copy_to_user((void *)arg, &pdi, sizeof(ap6pciDeviceInterface));
			}
				break;
			default:
				break;
		}
	}
	return 0;
}


/**
 * @brief ap6pci[0-N] device mmap open callback
 * @param vma: vma area
 */
void ap6pci_mmap_vma_open(struct vm_area_struct * vma){
	printk(KERN_INFO "ap6pci VMA open, virt %lx, phys %lx\n",
	            vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

/**
 * @brief ap6pci[0-N] device mmap close callback
 * @param vma : vma area
 */
void ap6pci_mmap_vma_close(struct vm_area_struct * vma){
	printk(KERN_INFO "ap6pci VMA close.\n");
}



//struct page *ap6pci_mmap_vma_nopage(struct vm_area_struct *vma,
//                           unsigned long address, int *type){
//	printk(KERN_INFO "ap6pci page fault.\n");
//	return VM_FAULT_NOPAGE; /* send a SIGBUS */
//}

/**
 * @brief ap6pci[0-N] device mmap operations
 */
static struct vm_operations_struct ap6pci_remap_vm_ops = {
    .open =  ap6pci_mmap_vma_open,
    .close = ap6pci_mmap_vma_close,
};

/**
 * @brief ap6pci[0-N] device mmap callback
 * @param fd: file descriptor
 * @param vma: vma area
 * @return 0->success
 */
s32 ap6pci_mmap(struct file *fd, struct vm_area_struct * vma){

	s32 rc=0;
	ap6_pci_dib *dib = (ap6_pci_dib *) fd->private_data;

	unsigned long physical = dib->region[0].io_addr >> PAGE_SHIFT;
	unsigned long vsize = vma->vm_end - vma->vm_start;
	unsigned long psize = dib->region[0].io_len;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	if (vsize > psize){
		printk(KERN_ERR "ap6pci mmap. failed : too big , vsize=%d , psize=%d!\n", vsize, psize);
	    rc= -EINVAL; /*  spans too high */
	}else{
		if(remap_pfn_range(vma, vma->vm_start, physical, vsize, vma->vm_page_prot)<0){
			printk(KERN_ERR "ap6pci mmap. remap_pfn failed.\n");
			rc=-EIO;
		}else{
			printk(KERN_INFO "ap6pci mmap succed: phys[0x%lx] -> virt[0x%lx]\n", physical, vma->vm_start);
			vma->vm_ops = &ap6pci_remap_vm_ops;
			ap6pci_mmap_vma_open(vma);
		}
	}
	return rc;
}

//s32 ap6pci_mma_old(struct file *fd, struct vm_area_struct * vma){
//	s32 rc=0;
//	//printk(KERN_INFO "ap6pci mmap.\n");
//	ap6_pci_dib *dib = (ap6_pci_dib *) fd->private_data;
//
//	// !! NEED TO HOLD THE MM SEMAPHORE ...
//	//printk(KERN_INFO "ap6pci mmap.\n");
//	if(remap_pfn_range(vma,vma->vm_start, dib->region[0].io_addr >> PAGE_SHIFT,dib->region[0].io_len,vma->vm_page_prot)<0){
//		printk( KERN_ERR "%s : remap pfn failed\n", dib->name);
//		rc = -EIO;
//	}else{
//		printk( KERN_INFO "%s : mapped %d bytes at 0x%x\n", dib->name, vma->vm_start);
//	}
//
//	vma->vm_ops = &ap6pci_remap_vm_ops;
//	ap6pci_mmap_vma_open(vma);
//	//printk(KERN_INFO "ap6pci mmap.\n");
//	return rc;
//}

/**
 * @brief ap6pci[0-N] device release callback
 * @param inod
 * @param fd
 * @return
 */
static s32 ap6pci_devrelease (struct inode *inod, struct file *fd){
	printk(KERN_NOTICE "ap6pci device release.\n");
	return 0;
}





/**
 * @brief ap6pci[0-N] device file operations
 */
static struct file_operations ap6pci_fops = {
      owner:THIS_MODULE,
      read:ap6pci_devread,
      write:ap6pci_devwrite,
      open:ap6pci_devopen,
      mmap:ap6pci_mmap,
      unlocked_ioctl:ap6pci_ioctl,
      release:ap6pci_devrelease,
};


/**
 * @brief character devices minor version cursor (static)
 */
static u32 minor = 0;

/**
 * @brief PCI character device class
 */
static struct class *ap6pci_class = NULL;










/**
 * @brief Register PCI character device class
 * @return 0->success
 */
s32 register_pci_class(void){
	s32 rc=0;
	ap6pci_class = class_create(THIS_MODULE, AP6_PCI_DRIVER_NAME);

	if (IS_ERR(ap6pci_class)) {
		printk(KERN_ERR "%s class creation failed", AP6_PCI_DRIVER_NAME);
		rc = PTR_ERR(ap6pci_class);
	}

	return rc;
}




/**
 * @brief Unregister PCI character device class
 */
void unregister_pci_class(){
	if(ap6pci_class){
		class_destroy(ap6pci_class);
	}
}





/**
 * @brief Register PCI device
 * @param pcidev : PCI device
 * @param pci_id : PCI device id
 * @return 0-> success
 */
s32 registerPciDevice(struct pci_dev *pcidev, const struct pci_device_id *pci_id){
	s32 rc=0;
	s32 grc=0;
	ap6_pci_dib * dib;

	rc = pci_request_regions(pcidev,AP6_PCI_DRIVER_NAME);
	if(rc){
		printk(KERN_ERR "ap6pci probe: pci_request_regions() ==> regions unavailable\n");
		grc=rc;
	}

	if(grc>=0){
		printk(KERN_INFO "ap6pci, allocating dib...\n");
		dib = kzalloc(sizeof(*dib), GFP_KERNEL);
		if (dib == NULL) {
			printk(KERN_ERR "ap6pci, unable to allocate memory\n");
			grc = -ENOMEM;
		}else{
			memset(dib,0,sizeof(ap6_pci_dib));
			dib->active=1;
			dib->id=s_currentId;
			s_currentId++;

//			printk(KERN_INFO "Device field: %x, %x\n",PCI_SLOT(pcidev->devfn),PCI_FUNC(pcidev->devfn));

			dib->description.ClassId = pcidev->class;
			dib->description.DeviceId = pcidev->device;
			dib->description.FunctionId = 0;
			dib->description.VendorId = pcidev->vendor;


			//struct pci_bus * domain = pcidev->bus->parent;
			//if(domain) dib->location.domain = domain->number;
			dib->location.domain = pci_domain_nr(pcidev->bus); // always returns 0 as far as we know on the 4th of June 2013...
			dib->location.bus = pcidev->bus->number;
			dib->location.dev = PCI_SLOT(pcidev->devfn);
			dib->location.func = PCI_FUNC(pcidev->devfn);

			list_add(&(dib->lnode),&ap6pci_devicelist); // add dib to ap6pci_devicelist
		}

	}

	/* allocate character device number */
	if(grc>=0){
		printk(KERN_INFO "ap6pci, allocating character device num:%d, minor:%d, name:%s...\n",dib->cdevnum,minor,AP6_PCI_DRIVER_NAME);
		rc = alloc_chrdev_region(&dib->cdevnum, minor, 1, AP6_PCI_DRIVER_NAME);
		if (rc) {
			printk(KERN_ERR "ap6pci, unable to allocate character device\n");
			grc=rc;
		}else printk(KERN_INFO "ap6pci, allocated character device num:%d, minor:%d, name:%s\n",dib->cdevnum,minor,AP6_PCI_DRIVER_NAME);
	}

	s32 i=0;
	for(i=0;(grc>=0)&&(i<AP6_PCI_N_REGIONS);i++){
		printk(KERN_INFO "ap6pci, allocating pci region %d...\n",i);
		/* retrieve io address base*/
		if(grc>=0){
			dib->region[i].io_addr = pci_resource_start(pcidev, i);
			printk(KERN_INFO "ap6pci, pci region %d @0x%lx\n",i,dib->region[i].io_addr);
			if (!dib->region[i].io_addr) {
				printk(KERN_ERR "ap6pci : io address retrieval failed, PCI region %d unavailable\n",i);
				rc = -EIO;
				grc=rc;
			}
		}

		/* retrieve io memory length */
		if(grc>=0){
			dib->region[i].io_len = pci_resource_len(pcidev, i);
			printk(KERN_INFO "ap6pci, pci region %d len %d\n",i,dib->region[i].io_len);
			if (!dib->region[i].io_len) {
				printk(KERN_ERR "ap6pci : io length retrieval failed : PCI region %d has 0 length\n",i);
				rc = -EIO;
				grc = rc;
			}
		}


		/* remap io memory */
		if(grc>=0){
			dib->region[i].io_remap = ioremap(dib->region[i].io_addr, dib->region[i].io_len);
			if (!dib->region[i].io_remap) {
				printk(KERN_ERR "ap6pci : PCI region %d mapping failed\n",i);
				rc = -ENOMEM;
				grc=rc;
			}else{
				s32 test = *((s32 *)(dib->region[i].io_remap));
				printk(KERN_INFO "ap6pci: io dereferencing trial:  %d\n",test);
			}
		}

		if(grc>=0){
			dib->region[i].io_flags = pci_resource_flags(pcidev, i);
			if(dib->region[i].io_flags&IORESOURCE_IO){
				printk(KERN_INFO "ap6pci: region %d is IO",i);
			}
			if(dib->region[i].io_flags&IORESOURCE_MEM){
				printk(KERN_INFO "ap6pci: region %d is MEM",i);
			}
			if(dib->region[i].io_flags&IORESOURCE_PREFETCH){
				printk(KERN_INFO "ap6pci: region %d is prefetchable",i);
			}
		}



	}

	/* intialize the character device field of our dib */
	printk(KERN_INFO "ap6pci, initializing cdev %d...\n",i);
	cdev_init(&(dib->cdev), &ap6pci_fops);
	printk(KERN_INFO "cdev at 0x%x\n",&(dib->cdev));


	printk(KERN_INFO "ap6pci, adding cdev %d...\n",i);
	rc = cdev_add(&(dib->cdev),dib->cdevnum, 1);
	if(rc<0){
		printk(KERN_ERR "ap6pci : unable to add device %d\n",dib->cdevnum);
		grc=rc;
	}

	printk(KERN_INFO "creating device %s%d...\n",AP6_PCI_DRIVER_NAME, minor);
	dib->chardev = device_create(ap6pci_class,NULL,dib->cdevnum,dib,"%s%d",AP6_PCI_DRIVER_NAME,minor);
	if(dib->chardev==NULL){
		printk(KERN_ERR "ap6pci : unable create device %s%d\n",AP6_PCI_DRIVER_NAME, minor);
		grc=-1;
	}

	sprintf(dib->name,"%s%d",AP6_PCI_DRIVER_NAME,minor);
//	printk( KERN_INFO "device created: %s\n",dib->chardev->
//	printk( KERN_INFO "device created: %s - %s - %s\n", dib->chardev->init_name, dib->chardev->driver->name, dib->chardev->driver->mod_name);

	printk( KERN_INFO CDEVFMT(dib->cdev));



	/** cleanup if any error */
    if (grc<0) {
    	printk(KERN_ERR "ap6pci something bad happened: cleaning up !\n");
		s32 i=0;
		for(i=0;i<AP6_PCI_N_REGIONS;i++){
			if (dib->region[i].io_remap){
				printk(KERN_ERR "ap6pci: cleaning PCI region %d!\n",i);
				iounmap(dib->region[i].io_remap);
			}
		}

		pci_release_regions(pcidev);

		if (dib->cdevnum){
			printk(KERN_ERR "ap6pci unregistering chrdev_region !\n");
			unregister_chrdev_region(dib->cdevnum, 1);
		}

		if(dib){
			printk(KERN_ERR "ap6pci freeing dib !\n");
			kfree(dib);
		}

		pci_disable_device(pcidev);

	} else {
		minor++;
		printk(KERN_INFO "ap6pci, setting dib as drvdata...\n",i);
		pci_set_drvdata(pcidev, dib);
		nbCharDevices++;
	}

    printk( KERN_INFO CDEVFMT(dib->cdev));
    printk( KERN_INFO "Nb char devices: %d\n", nbCharDevices);
return grc;
}


/**
 * @brief Unregister PCI device
 * @param pcidev
 */
void unregisterPciDevice(struct pci_dev *pcidev){
	ap6_pci_dib *dib=NULL;

	printk( KERN_INFO "AP6 PCI  getting driver data\n");

	if(pcidev){
		/* retrieve dib from driver private data */
		dib = (ap6_pci_dib *) pci_get_drvdata(pcidev);


		if(dib){
			printk( KERN_INFO "AP6 PCI  driver data ok\n");
			if(ap6pci_class){
				device_destroy(ap6pci_class, dib->cdevnum);
				printk( KERN_INFO "AP6 PCI  device destroyed\n");
			}

			/* unmap IO regions */
			s32 i=0;
			for(i=0;i<AP6_PCI_N_REGIONS;i++){
				printk(KERN_ERR "ap6pci: cleaning PCI region %d!\n",i);
				if(dib->region[i].io_remap){
					iounmap(dib->region[i].io_remap);
				}
			}
			pci_release_regions(pcidev);

			cdev_del(&(dib->cdev));

			unregister_chrdev_region(dib->cdevnum,1);

			list_del(&(dib->lnode)); // remove dib from ap6pci_devicelist

			printk(KERN_ERR "ap6pci freeing dib !\n");
			kfree(dib);
			nbCharDevices--;
		}
	}
}





#endif /* AP6_PCI_DEVICES_C_ */
