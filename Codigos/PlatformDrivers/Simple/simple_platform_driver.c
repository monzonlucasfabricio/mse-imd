#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

static int my_platform_driver_probe(struct platform_device *pdev)
{
	pr_info("Hello! Device probed!\n");
	return 0;
}

static int my_platform_driver_remove(struct platform_device *pdev)
{
	pr_info("Good bye reader! Removing me\n");
	return 0;
}

static struct platform_driver pdrv =
{
	.probe = my_platform_driver_probe,
	.remove = my_platform_driver_remove,
	.driver =
    {
		.name = "hanes_driver",
		.owner = THIS_MODULE,
	},
};

static int __init platform_driver_init(void)
{
	pr_info("Init and register!\n");

	/* Registering with Kernel */
	platform_driver_register(&pdrv);
	return 0;
}

static void __exit platform_driver_exit(void)
{
	pr_info("Exit and unregister\n");

	/* Unregistering from Kernel */
	platform_driver_unregister(&pdrv);
}

module_init(platform_driver_init);
module_exit(platform_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Fabricio Monzon Languasco");
MODULE_DESCRIPTION("My platform Hello World module - MSE08");
