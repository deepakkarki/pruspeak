#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xae4b0ceb, "module_layout" },
	{ 0xd3a204c1, "platform_driver_unregister" },
	{ 0xefd55b63, "platform_driver_register" },
	{ 0x4059792f, "print_hex_dump" },
	{ 0xf9e73082, "scnprintf" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x73ec879a, "finish_wait" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x38ee1586, "prepare_to_wait" },
	{ 0xca54fee, "_test_and_set_bit" },
	{ 0x48efa767, "mutex_unlock" },
	{ 0xffbf6de, "mutex_lock" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xa83b9525, "pwmchip_add" },
	{ 0x6586f64b, "rproc_boot" },
	{ 0x75bb4cf7, "device_create_file" },
	{ 0x5d799512, "rproc_add" },
	{ 0xe85afa84, "devm_request_threaded_irq" },
	{ 0xb81960ca, "snprintf" },
	{ 0x536ff674, "__init_waitqueue_head" },
	{ 0xb0fab0ca, "__mutex_init" },
	{ 0x5a8936a6, "rproc_alloc" },
	{ 0x3dbf8def, "of_property_read_string" },
	{ 0xc4acd684, "devm_ioremap" },
	{ 0xd01009f1, "platform_get_resource" },
	{ 0x782011c7, "devm_pinctrl_put" },
	{ 0xda1d256e, "__pm_runtime_disable" },
	{ 0x16fdefd2, "of_get_next_child" },
	{ 0xc3577ebb, "platform_get_irq" },
	{ 0xb7eea999, "dma_supported" },
	{ 0xf690d0f0, "of_node_put" },
	{ 0x64119029, "__pm_runtime_resume" },
	{ 0xafde83a0, "pm_runtime_enable" },
	{ 0x1eac9fb, "pinctrl_select_state" },
	{ 0x3f91b9ef, "pinctrl_lookup_state" },
	{ 0x13371c7b, "devm_pinctrl_get" },
	{ 0x5268e264, "devm_kfree" },
	{ 0xd3094c12, "of_property_read_u32_array" },
	{ 0x99b5ce2, "devm_kzalloc" },
	{ 0x912c69a4, "of_find_property" },
	{ 0xa5a2fc3, "__wake_up" },
	{ 0x676bbc0f, "_set_bit" },
	{ 0x2a3aa678, "_test_and_clear_bit" },
	{ 0x27e1a049, "printk" },
	{ 0x53968437, "dev_warn" },
	{ 0xc025a8ef, "rproc_vq_interrupt" },
	{ 0x37a0cba, "kfree" },
	{ 0xf709f70e, "release_firmware" },
	{ 0x8062e3b, "request_firmware" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x20000329, "simple_strtoul" },
	{ 0x349cba85, "strchr" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x9d669763, "memcpy" },
	{ 0xaaebf074, "dev_set_drvdata" },
	{ 0x27c9798e, "rproc_put" },
	{ 0xcb11a04f, "rproc_del" },
	{ 0x3bd5d6a0, "device_remove_file" },
	{ 0x361bbc23, "arm_dma_ops" },
	{ 0x97255bdf, "strlen" },
	{ 0xc22f24be, "dev_get_drvdata" },
	{ 0xd07c8db4, "_dev_info" },
	{ 0x71c90087, "memcmp" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x75a26fbd, "dev_err" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "44343E43E71D214642BD8F0");
