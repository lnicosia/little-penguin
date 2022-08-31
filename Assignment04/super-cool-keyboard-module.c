#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Nicosia <lnicosia@student.42.fr>");
MODULE_DESCRIPTION("USB keyboard loading.");

static int test_probe(struct usb_interface *interface,
			const struct usb_device_id *id)
{
	pr_info("USB Keyboard mega cool driver plugged in");
	return 0;
}

static void test_disconnect(struct usb_interface *interface)
{
	pr_info("USB Keyboard mega cool driver unplugged");
}

static struct usb_device_id id_table_test [] = {
	{ USB_INTERFACE_INFO(
		USB_INTERFACE_CLASS_HID,
		USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_KEYBOARD)},
	{}
};
MODULE_DEVICE_TABLE(usb, id_table_test);

static struct usb_driver my_driver = {
	.name = "super cool usb device",
	.probe = test_probe,
	.disconnect = test_disconnect,
	.id_table = id_table_test,
};

static int __init	usb_module_init(void)
{
	pr_info("USB Keyboard mega cool module loaded !\n");
	usb_register(&my_driver);
	return 0;
}

static void __exit	usb_module_cleanup(void)
{
	pr_info("Cleaning up USB Keyboard mega cool module.\n");
	usb_deregister(&my_driver);
}

module_init(usb_module_init);
module_exit(usb_module_cleanup);
