#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x1fdc7df2, "_mcount" },
	{ 0x122c3a7e, "_printk" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x72b0c4fd, "cdev_init" },
	{ 0xc274f52b, "cdev_add" },
	{ 0xfd1437f4, "class_create" },
	{ 0x2dbfdd75, "device_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x6dca73de, "cdev_del" },
	{ 0xf2012b72, "class_destroy" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x8d5e9f5f, "device_destroy" },
	{ 0x206d880f, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "330995109663E781B88F802");
