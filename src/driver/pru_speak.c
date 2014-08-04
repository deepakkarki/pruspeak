#include <linux/module.h>
#include <linux/err.h>
#include <linux/dma-mapping.h>
#include <linux/remoteproc.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/genalloc.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/pinctrl/consumer.h>
#include <linux/io.h>
#include <plat/mailbox.h>
#include <linux/virtio_ids.h>
#include <linux/elf.h>
#include <linux/byteorder/generic.h>
#include <linux/virtio.h>
#include <linux/virtio_ring.h>
#include <asm/atomic.h>

#include <linux/pwm.h>

#include <linux/irqreturn.h>
#include <linux/firmware.h>
#include "pru_speak.h"

//1. this is static for now, should just make it as a part of private data of pdev
static struct pru_speak *ps_dev;


static ssize_t pru_speak_debug(int idx, struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;

	if (count ==  0)
		return -1;
	if (buf[0] == '0')
		ret = pruspeak_downcall_idx(idx, 0, 0, 0, 0, 0, 0);/*pp, idx, syscall_id, 5 args*/
	else
		ret = pruspeak_downcall_idx(idx, 0, 1, 0, 0, 0, 0);

	printk( KERN_INFO "write to pru_speak_debug\n");
	return strlen(buf);
}

static ssize_t pru_speak_debug0(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	return pru_speak_debug(0, dev, attr, buf, count);
}

/*
 * syscall #1 - used to initialize the shm info for both PRU and ARM
 * 		*reading* this file will give back hex value of physical address as a string
 *		at the same time PRU is also informed of the address via a downcall
 */

static ssize_t pru_speak_shm_init(int idx, struct device *dev, struct device_attribute *attr, char *buf)
{
        int ret;

	//TODO : modify this
	//ok DONE, delete this comment after checking that everything works.
	struct pru_shm shm_code = ps_dev->shm[BS_CODE];
	struct pru_shm shm_ret  = ps_dev->shm[BS_RET];

	printk("physical addr shm_code, shm_ret : %x, %x\n", (unsigned int)shm_code.paddr, (unsigned int)shm_ret.paddr);

        ret = pruspeak_downcall_idx(idx, 1, (int)shm_code.paddr, (int)shm_ret.paddr, 10, 0, 0); //pp, idx, sys call id, base addr, val, junk,.,.
	//The pru modifies the arg "10" and places "10^2" in the first loc of shm_code
        printk( KERN_INFO "pru_speak_init, pram value : 10, return value : %d, modified value : %d\n", ret, *((int *)shm_code.vaddr));

	return scnprintf(buf, PAGE_SIZE, "%x,%x", (int)shm_code.paddr, (int)shm_ret.paddr);
}

static ssize_t pru_speak_shm_init0(struct device *dev, struct device_attribute *attr, char *buf)
{
        return pru_speak_shm_init(0, dev, attr, buf);
}

/*
 * syscall #2 - ask PRU to start execution.
 *		writing 1 => start/continue execution
 *		writing 0 => pause execution
 *
 * NOTE : pru_shm_init must be triggered before this. otherwise hell will chase you.
 */

static ssize_t pru_speak_execute(int idx, struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        int ret;

	if (count ==  0)
                return -1;
        if (buf[0] == '0')
                ret = pruspeak_downcall_idx( idx, 2, 0, 0, 0, 0, 0);/*pp, idx, syscall_id, start/pause, 4 args*/
        else
                ret = pruspeak_downcall_idx(idx, 2, 1, 0, 0, 0, 0);

        printk( KERN_INFO "write to pru_speak_execute\n");
        return strlen(buf);

}

static ssize_t pru_speak_execute0(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        return pru_speak_execute(0, dev, attr, buf, count);
}

/*
 * syscall #3 - ask pru to abort currently executing BS script
 *		triggered by write to this file
 */

static ssize_t pru_speak_abort(int idx, struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        int ret;

        ret = pruspeak_downcall_idx(idx, 3, 0, 0, 0, 0, 0); /* pp, idx, 5 args*/

        printk( KERN_INFO "write to pru_speak_abort\n");
        return strlen(buf);

}

static ssize_t pru_speak_abort0(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        return pru_speak_abort(0, dev, attr, buf, count);
}

/*
 * syscall #4 - get status of botspeak interpreter
 *		"read" returns 0 - if no BS code is being executed
 *		returns 1 - if BS code is being executed 
 */

static ssize_t pru_speak_status(int idx, struct device *dev, struct device_attribute *attr, char *buf)
{

        int ret = pruspeak_downcall_idx(idx, 4, 0, 0, 0, 0, 0); //pp, idx, sys call id, 5 params

	printk( KERN_INFO "read to pru_speak_status\n");
        
	return scnprintf(buf, PAGE_SIZE, "%d", ret);
}

static ssize_t pru_speak_status0(struct device *dev, struct device_attribute *attr, char *buf)
{
        return pru_speak_status(0, dev, attr, buf);
}

/*
 * syscall #5 - ask PRU to execute single BS instruction
 *		The Byte code of the BS instruction to be executed is written
 *		to this file.
 *		return value of downcall = 4 if OK, else value = ?
 */

static ssize_t pru_speak_single_cmd(int idx, struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        int inst = 0, ret, i;

        if (count ==  0)
                return -1;

	//buf[0] = LSB; buf[3] = MSB
	//the for loop packs the 4 incoming character into a 1 word long integer 
	//viz one botspeak instruction (byte code)
	for(i = 0; i < 4; i++){
		inst |= ((int)buf[i]) << i*8;
	}

        ret = pruspeak_downcall_idx(idx, 5, inst, 0, 0, 0, 0);/*pp, idx, syscall_id, 5 args*/

        printk( KERN_INFO "write to pru_speak_single_cmd. return value of downcall : %d\n", ret);
	printk( "STRLEN(buf) : %d\n", strlen(buf)); //prints 0!!
        //return strlen(buf); 
	return 4; //quick hack
}

static ssize_t pru_speak_single_cmd0(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        return pru_speak_single_cmd(0, dev, attr, buf, count);
}

/*
 * syscall #6 - ask PRU to execute single 64 bit BS instruction
 *		The Byte code of the BS instruction to be executed is written
 *		to this file.
 *		return value of downcall = 8 if OK, else value = ?
 */

static ssize_t pru_speak_single_cmd_64(int idx, struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        int inst_a = 0, ret, i;
        int inst_b = 0;

	//buf[0] = LSB; buf[3] = MSB : for 1st word
	//buf[4] = LSB; buf[7] = MSB : for 2nd word
	//the for loop packs the 8 incoming character into two, 1 word long integers
	//viz one botspeak instruction (byte code)
	for(i = 0; i < 4; i++){
		inst_a |= ((int)buf[i]) << i*8;
	}

	for(i = 4; i < 8; i++){
		inst_b |= ((int)buf[i]) << (i-4)*8;
	}

        ret = pruspeak_downcall_idx(idx, 6, inst_a, inst_b, 0, 0, 0);/*pp, idx, syscall_id, 5 args*/

	printk("**** inst_a : %d, inst_b : %d", inst_a, inst_b);
        printk( KERN_INFO "write to pru_speak_single_cmd_64. return value of downcall : %d\n", ret);
	return 8; //quick hack
}

static ssize_t pru_speak_single_cmd0_64(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        return pru_speak_single_cmd_64(0, dev, attr, buf, count);
}

static DEVICE_ATTR(pru_speak_debug, S_IWUSR, NULL, pru_speak_debug0);
static DEVICE_ATTR(pru_speak_shm_init, S_IWUSR | S_IRUGO, pru_speak_shm_init0, NULL);
static DEVICE_ATTR(pru_speak_execute, S_IWUSR, NULL, pru_speak_execute0);
static DEVICE_ATTR(pru_speak_abort, S_IWUSR, NULL, pru_speak_abort0);
static DEVICE_ATTR(pru_speak_status, S_IRUGO, pru_speak_status0, NULL);
static DEVICE_ATTR(pru_speak_single_cmd, S_IWUSR, NULL, pru_speak_single_cmd0);
static DEVICE_ATTR(pru_speak_single_cmd_64, S_IWUSR, NULL, pru_speak_single_cmd0_64);

static struct attribute *pru_speak_attributes[] = {
	&dev_attr_pru_speak_debug.attr,
	&dev_attr_pru_speak_shm_init.attr,
	&dev_attr_pru_speak_execute.attr,
	&dev_attr_pru_speak_abort.attr,
	&dev_attr_pru_speak_status.attr,
	&dev_attr_pru_speak_single_cmd.attr,
	&dev_attr_pru_speak_single_cmd_64.attr,
	NULL
};

static struct attribute_group pru_speak_attr_group = {
	.attrs = pru_speak_attributes
};

static int pru_speak_remove(struct platform_device *pdev)
{
	int i;
	struct device *pruproc_dev = &(ps_dev->pdev_rproc->dev);
	//free the pru speak data struct
	kfree(ps_dev);

	//free the shm segments
	for(i=0; i < ps_dev->shm_count; i++){
		kfree(ps_dev->shm[i].vaddr);
	}

	//remove all the sysfs entries
	sysfs_remove_group(&pruproc_dev->kobj, &pru_speak_attr_group);

	return 0;
}

static int pru_speak_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev; 
	struct device_node *node = dev->of_node;
	int shm_count, err, x;
	int tmparr[MAX_SHARED];

	printk(KERN_INFO "entering pru_speak_probe\n");

	ps_dev = kzalloc(sizeof(*ps_dev), GFP_KERNEL);

	printk(KERN_INFO "ps_dev allocated : %p\n", ps_dev);

	ps_dev->pdev_rproc = pruspeak_get_pdev_rproc();
	if(ps_dev->pdev_rproc == (struct platform_device *)NULL){
		//dev_error(dev, "can't get pdev_rproc\n");
		goto err_fail;
	}
	printk(KERN_INFO "call made to pruspeak_get_pdev_rproc\n");
	platform_set_drvdata(pdev, ps_dev);

	/* get the number of shared memory segment */
	err = of_property_read_u32(node, "shm-count", &shm_count);

	if (err != 0) {
		//dev_err(dev, "can't find property %s\n", "shm-count");
		goto err_fail;
	}		

	/* make sure the count is within limits expected */
	if ((shm_count < MIN_SHARED) || (shm_count > MAX_SHARED)){
		//dev_err(dev, "expected shm segments, min : %d, max : %d\n", MIN_SHARED, MAX_SHARED);
		goto err_fail;
	}

	err = of_property_read_u32_array(node, "shm-size", tmparr, shm_count);
	if (err != 0) {
		//dev_err(dev, "no shm-size property\n");
		goto err_fail;
	}

	printk(KERN_INFO "entering shm allocation\n");

	/* assign idx, shared memory size for each segment for this pru */
	for(x=0; x < shm_count; x++){
		ps_dev->shm[x].size_in_pages = tmparr[x];
		ps_dev->shm[x].idx = 0; //shm always pru0

		/* mark valid each shm segment, allocate space for it*/
		printk("Initializing shm #%d", x);
		ps_dev->shm[x].is_valid = 1;
		ps_dev->shm[x].vaddr = dma_zalloc_coherent(dev, ps_dev->shm[x].size_in_pages * PAGE_SIZE, 
									(dma_addr_t *) &(ps_dev->shm[x].paddr), GFP_DMA);

		if(!(ps_dev->shm[x].vaddr)){
			ps_dev->shm[x].is_valid = 0;
			printk("shm init failed\n");
		}
	}

	ps_dev->shm_count = shm_count;

	printk(KERN_INFO"SHM initalization sequence is now complete\n");

	//create sysfs
	struct device *pruproc_dev = &(ps_dev->pdev_rproc->dev);
	err = sysfs_create_group(&pruproc_dev->kobj, &pru_speak_attr_group);
	if (err) {
		//dev_err(dev, "creation of sysfs failed!\n");
		goto err_fail;
	}

	return 0;

	err_fail:
		return -1;
}



static const struct of_device_id pru_speak_dt_ids[] = {
	{ .compatible = "ti,pru_speak", .data = NULL, },
	{},
};
//MODULE_DEVICE_TABLE(of, pruss_dt_ids);

static struct platform_driver pru_speak_driver = {
	.driver	= {
		.name	= "pru_speak",
		.owner	= THIS_MODULE,
		.of_match_table = pru_speak_dt_ids,
	},
	.probe	= pru_speak_probe,
	.remove	= pru_speak_remove,
};

//module_platform_driver(pruproc_driver);


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

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("PRU speak driver");
MODULE_AUTHOR("Deepak Karki <deepak6k@gmail.com>");
