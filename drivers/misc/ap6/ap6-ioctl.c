/**
 * @file ap6-ioctl.c
 *
 *  @date 20 févr. 2013
 *      Author: mdelacour
 */

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "ap6-ioctl.h"


#include "ap6-ioctl-interface.h"
#include "ap6-drv_interface.h"
#include "ap6-drv.h"
#include "ap6-pci-devices.h"


//static int major = 0; /** Major character device number */
static int minor = 0; /** Minor character device number */

int nb_devs = AP6_NB_DEVS; /** Number of character devices */

//static char msg[DATA_SIZE];
char buf[DATA_SIZE];



/**
 * @brief Main IOCTL
 * @param fd:	file descriptor
 * @param cm:	IOCTL command
 * @param arg:	IOCTL command argument
 * @return 0-> success
 */
static int ap6_ioctl(struct file *fd, unsigned int cmd, unsigned long arg){
	int len = DATA_SIZE;
//	printk( KERN_INFO "ap6driver : ioctl start\n");
	printk( KERN_INFO "ap6driver ioctl: received command: %u\n", cmd);
//	printk( KERN_INFO "ap6driver ioctl: ver command: %u\n", AP6_IOCTL_DRV_INFO);
//	printk( KERN_INFO "sioctld: read command: %u\n", AP6_IOCTL_READ);
//	printk( KERN_INFO "sioctld: write command: %u\n", AP6_IOCTL_WRITE);
	char *name;

	switch(cmd) {
		case AP6_IOCTL_READ:
			printk( KERN_INFO "ap6 : ioctl read\n");
			copy_to_user((char *)arg, buf, 200);
			break;

		case AP6_IOCTL_WRITE:
			printk( KERN_INFO "ap6 : ioctl write\n");
			copy_from_user(buf, (char *)arg, len);
			buf[len] = '\0';
			break;

		case AP6_IOCTL_SIG:
			printk (KERN_INFO "ap6 : ioctl sig -> arming signal\n");
			break;

		case AP6_IOCTL_DRV_INFO:
			copy_to_user((void *)arg, getDriverInfo(),sizeof(ap6driverInfo));
			break;


		case AP6_IOCTL_PCI_INIT:
			break;
		case AP6_IOCTL_PCI_EXIT:
			break;

		case AP6_IOCTL_PCI_NBDEV:
		{
			unsigned int nb = getPciNbCharDevices();
			printk("IOCTL nb pci char devices: %d\n",nb);
			len=4;
			copy_to_user((void*)arg,(void *)&nb,len);
		}
			break;

		case AP6_IOCTL_PCI_ENUM:
			name = enumPciDevices();
			len=strlen(name);
			copy_to_user((void *)arg, name,len);

			break;


		default:
			return -ENOTTY;
	}
	return len;
}





/**
 * @brief main IOCTL read
 * @param fd:	file descriptor
 * @param user_buffer: buffer from user space
 * @param length: read length
 * @param offset: read offset
 * @return read size
 */
static ssize_t ap6_ioctl_read(struct file *fd, char __user *user_buffer, size_t length, loff_t *offset){
	return simple_read_from_buffer(user_buffer, length, offset, buf, 200);
}




/**
 * @brief main IOCTL write
 * @param fd:	file descriptor
 * @param user_buffer: buffer from user space
 * @param length: write length
 * @param offset: write offset
 * @return written size
 */
static ssize_t ap6_ioctl_write(struct file *fd, const char __user *user_buffer, size_t len, loff_t *off){
	if (len > DATA_SIZE-1)
		return -EINVAL;
	copy_from_user(buf, user_buffer, len);
	buf[len] = '\0';
	return len;
}



/**
 * @brief Main IOCTL file operations
 */
static struct file_operations fops = {
	.owner =    THIS_MODULE,
	.read =     ap6_ioctl_read,
	.write =    ap6_ioctl_write,
	.unlocked_ioctl = 	ap6_ioctl
};

/**
 * @brief main IOCTL character device class (static)
 */
static struct class *	main_ioctl_class;

/**
 * @brief main IOCTL character device number (static)
 */
static dev_t 			main_ioctl_devnum;

/**
 * @brief main IOCTL character device cdev (static)
 */
static struct cdev 		main_ioctl_cdev;

/**
 * @brief main IOCTL character device (static)
 */
static struct device *	main_ioctl_chardev;


/**
 * @brief main IOCTL init
 * @return 0->success
 */
int ap6_ioctl_init(){
	int grc=0;
	int rc=0;


	if(grc>=0){
		main_ioctl_class = class_create(THIS_MODULE,AP6_DRIVER_NAME);
		if(main_ioctl_class==0) grc=-1;
	}

	/* allocate character device number */
	if(grc>=0){
		printk(KERN_INFO "ap6pci, allocating character device num:%d, minor:%d, name:%s...\n",main_ioctl_devnum,minor,AP6_DRIVER_NAME);
		rc = alloc_chrdev_region(&main_ioctl_devnum, minor, 1, AP6_DRIVER_NAME);
		if (rc) {
			printk(KERN_ERR "ap6pci, unable to allocate character device\n");
			grc=rc;
		}else printk(KERN_INFO "ap6pci, allocated character device num:%d, minor:%d, name:%s\n",main_ioctl_devnum,minor,AP6_DRIVER_NAME);
	}

	if(grc>=0){
		printk(KERN_INFO "%s, initializing cdev ...\n",AP6_DRIVER_NAME);
		cdev_init(&(main_ioctl_cdev), &fops);
		printk(KERN_INFO "cdev at 0x%x\n",&(main_ioctl_cdev));
	}
	if(grc>=0){
		printk(KERN_INFO "%s, adding cdev...\n",AP6_DRIVER_NAME);
		rc = cdev_add(&(main_ioctl_cdev),main_ioctl_devnum, 1);
		if(rc<0){
			printk(KERN_ERR "%s unable to add device %d\n",AP6_DRIVER_NAME, main_ioctl_devnum);
			grc=rc;
		}
	}

	if(grc>=0){
		printk(KERN_INFO "%s creating device %s...\n",AP6_PCI_DRIVER_NAME, minor);
		main_ioctl_chardev = device_create(main_ioctl_class,NULL,main_ioctl_devnum,NULL,"%s",AP6_DRIVER_NAME);
		if(main_ioctl_chardev==NULL){
			printk(KERN_ERR "unable create device %s\n",AP6_DRIVER_NAME);
			grc=-1;
		}
	}


	/** cleanup if any error */
    if (grc<0) {
    	printk(KERN_ERR "%s something bad happened: cleaning up !\n",AP6_DRIVER_NAME);

		ap6_ioctl_exit();

	} else {
		minor++;
	}

 	return grc;
}



/**
 * @brief main IOCTL exit
 */
void ap6_ioctl_exit(){

	if(main_ioctl_class){

		printk( KERN_INFO "%s  driver data ok\n",AP6_DRIVER_NAME);
		if(main_ioctl_class){
			device_destroy(main_ioctl_class, main_ioctl_devnum);
			printk( KERN_INFO "%s  device destroyed\n",AP6_DRIVER_NAME);
		}

		cdev_del(&(main_ioctl_cdev));

		unregister_chrdev_region(main_ioctl_devnum,1);

		class_destroy(main_ioctl_class);
	}
}
