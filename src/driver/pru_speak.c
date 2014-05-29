/*   
  * pru-speak test driver
  *
  */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define BIN_ATTR(_name,_mode,_size,_read,_write,_mmap) { \
       .attr = { .name  =  __stringify(_name),  .mode = _mode  },   \
       .size = _size,                                         \
       .read   = _read,                                      \
       .write  = _write,                                      \
       .mmap = _mmap,                                   \
}


static ssize_t bin_file_read(struct file * f, struct kobject *kobj, struct bin_attribute *attr, char *buffer, loff_t pos, size_t size)
{
        printk(KERN_INFO "READ called on BIN FILE\n");
        return (ssize_t)0;
}

static ssize_t bin_file_write(struct file *f, struct kobject *kobj, struct bin_attribute *attr, char *buffer, loff_t pos, size_t size)
{
        printk(KERN_INFO "WRITE called on BIN FILE\n");
        return (ssize_t)0;
}

static int bin_file_mmap(struct file *f, struct kobject *kobj, struct bin_attribute *attr,  struct vm_area_struct *vma)
{
        printk(KERN_INFO "MMAP called on BIN FILE\n");
        return 0;
}

static struct bin_attribute pru_speak_bin_attr = BIN_ATTR(pru_speak_binfile, S_IWUSR | S_IRUGO, PAGE_SIZE, bin_file_read, bin_file_write, bin_file_mmap);


//this is the buffer which is maintained for read and write into the pru_speak_sysfs file - just temp, to check if everything works
static char data[100];

/*
*functions to handle sysfs load and store for the file "pru_speak_sysfs"
*/

static ssize_t pru_speak_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        printk(KERN_INFO "sysfs show called\n");
        return scnprintf(buf, PAGE_SIZE, "%s\n", data);
}

static ssize_t pru_speak_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{       
        printk(KERN_INFO "sysfs store called\n");
        return scnprintf(data, sizeof(data), "%s", buf);
}

static DEVICE_ATTR(pru_speak_sysfs, S_IWUSR | S_IRUGO, pru_speak_show, pru_speak_store);
//DEVICE_ATTR(name, mode, show, store);

static int pru_speak_probe(struct platform_device *pdev)
{
        int err;
        struct device *dev = &pdev->dev;
        printk(KERN_INFO "pru_speak probed\n");
        err = device_create_file(dev, &dev_attr_pru_speak_sysfs);
        if (err != 0){
                printk(KERN_INFO "sysfs file creation failed");
        }
	err = sysfs_create_bin_file(&(dev->kobj), &pru_speak_bin_attr);
	if (err != 0){
                printk(KERN_INFO "BIN FILE could not be created");
        }
        return 0;
}

static int pru_speak_remove(struct platform_device *pdev)
{
        struct device *dev = &pdev->dev;
        printk(KERN_INFO "pru_speak removed\n");
        device_remove_file(dev, &dev_attr_pru_speak_sysfs);
	sysfs_remove_bin_file( (&dev->kobj), &pru_speak_bin_attr);
        return 0;
}


static const struct of_device_id pru_speak_dt_ids[] = {
	{ .compatible = "ti,pru_speak", .data = NULL, },
	{},
};

//MODULE_DEVICE_TABLE(of, pruss_dt_ids);

static struct platform_driver pru_speak_driver = {
	.driver = {
		.name = "pru_speak",
		.owner = THIS_MODULE,
		.of_match_table = pru_speak_dt_ids,
	},
	.probe = pru_speak_probe,
        .remove = pru_speak_remove,

};

static int __init pru_speak_init(void)
{
	printk(KERN_INFO "pru_speak loaded\n");
	platform_driver_register(&pru_speak_driver);
	return 0;
}

static void __exit pru_speak_exit(void)
{
	printk(KERN_INFO "pru_speak unloaded\n");
	platform_driver_unregister(&pru_speak_driver);
}

module_init(pru_speak_init);
module_exit(pru_speak_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Deepak Karki");	
MODULE_DESCRIPTION("PRU Speak driver");	


