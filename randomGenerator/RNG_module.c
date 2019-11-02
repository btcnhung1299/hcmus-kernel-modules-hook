#include <linux/module.h> // Core header for loading LKMs
#include <linux/random.h> // get_random_bytes
#include <linux/init.h>   // Macros __init & __exit
#include <linux/kernel.h> // Contain types, macros, functions 
#include <linux/types.h>  // dev_t (major & minor number)
#include <linux/fs.h>     // Header for Linux file system support
#include <linux/uaccess.h>// Required for the copy to user_space func
#include <linux/device.h>
#define DEVICE_NAME "RNGChar" // Character device name
#define CLASS_NAME "RNG"      // Character device driver
#define MAX 1000

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Ngoc Bang Tam - 1712747");
MODULE_DESCRIPTION("Random number generation Task");
MODULE_VERSION("1.0");

static int majorNumber;
static int timesOfOpens = 0;
static struct class* RNGClass = NULL;
static struct device* RNGDevice = NULL;

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);

static struct file_operations fops = 
{
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
};


static int __init RandomGeneratorCD_init(void)
{
	// Dynamically allocate a major number for device file
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber < 0){
		printk(KERN_ALERT "RNG failed to register a major number\n");
		return majorNumber;
	}

	printk(KERN_INFO "RNG: registered successfully with major number %d\n", majorNumber);

		
	// Register the device class
	RNGClass = class_create(THIS_MODULE, CLASS_NAME);
	
	if (IS_ERR(RNGClass)){
		// clean up if there's error
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		// return an error on a pointer
		return PTR_ERR(RNGClass);
	}
	printk(KERN_INFO "RNG: device class registered successfully\n");

	// Register the device driver
	RNGDevice = device_create(RNGClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(RNGDevice)){
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device driver\n");
		return PTR_ERR(RNGDevice);
	}
	printk(KERN_INFO "RNG: device class created correctly\n");
	return 0;
}


static void __exit RandomGeneratorCD_exit(void)
{
	// remove the device
	device_destroy(RNGClass, MKDEV(majorNumber, 0)); 
	// unregister the device class
	class_unregister(RNGClass);
	// remove the device class
	class_destroy(RNGClass);
	// unregister the major number
	unregister_chrdev(majorNumber, DEVICE_NAME);

	printk(KERN_INFO "RNG: Random number generator unregistered");
}

static int dev_open(struct inode *inodep, struct file *filep){
	timesOfOpens++;
	printk(KERN_INFO "RNG: Device has been opened %d time(s)\n", timesOfOpens);
	return 0;
}
static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "RNG: Device successfully closed\n");
	return 0;
}
static ssize_t dev_read(struct file *filep, char* usr_space, size_t len, loff_t* offset){
	int randNum;
	int error_cnt;
	get_random_bytes(&randNum, sizeof(randNum));
	randNum %= MAX;

	// copy_to_user has the format (*to, *from, size) and returns 0 on success
	error_cnt = copy_to_user(usr_space, &randNum, sizeof(randNum));

	if (error_cnt == 0){
		printk(KERN_INFO "RNG: Sent random number to the user\n");
		return 0;
	}
	else{
		printk(KERN_INFO "RNG: Failed to send random number to the user\n");
		return -EFAULT;
	}
}

module_init(RandomGeneratorCD_init);
module_exit(RandomGeneratorCD_exit);

