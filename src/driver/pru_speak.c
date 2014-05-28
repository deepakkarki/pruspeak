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

static int pru_speak_probe(struct platform_device *pdev)
{
	printk(KERN_INFO "pru_speak probed\n");
	return 0;
}

static int pru_speak_remove(struct platform_device *pdev)
{
	printk(KERN_INFO "pru_speak removed\n");
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


