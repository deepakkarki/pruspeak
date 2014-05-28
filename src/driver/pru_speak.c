 /*   
  * pru-speak test driver
  *
  */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

static int __init pru_speak_init(void)
{
	printk(KERN_INFO "pru_speak loaded\n");
	return 0;
}

static void __exit pru_speak_exit(void)
{
	printk(KERN_INFO "pru_speak unloaded\n");
}

module_init(pru_speak_init);
module_exit(pru_speak_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Deepak Karki");	
MODULE_DESCRIPTION("PRU Speak driver");	
