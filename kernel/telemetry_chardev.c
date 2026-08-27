#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVNAME "telemetry0"
static dev_t dev_num;
static struct cdev my_cdev;
static char buffer[256];
static size_t buf_len;

static ssize_t dev_read(struct file *f, char __user *ubuf,
                        size_t len, loff_t *off)
{
    if (*off >= buf_len)
        return 0;

    if (len > buf_len - *off)
        len = buf_len - *off;

    if (copy_to_user(ubuf, buffer + *off, len))
        return -EFAULT;

    pr_info("telemetry: dev_read: %zu bytes copied to userspace\n", len);

    *off += len;
    return len;
}
static ssize_t dev_write(struct file *f, const char __user *ubuf,
                         size_t len, loff_t *off)
{
    if (len > sizeof(buffer))
        len = sizeof(buffer);

    if (copy_from_user(buffer, ubuf, len))
        return -EFAULT;

    buf_len = len;

    pr_info("telemetry: dev_write: %zu bytes copied to kernel buffer\n", len);

    return len;
}
static struct file_operations fops = { .owner = THIS_MODULE, .read = dev_read, .write = dev_write };

static int __init drv_init(void) {
    alloc_chrdev_region(&dev_num, 0, 1, DEVNAME);
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev_num, 1);
    pr_info("telemetry: loaded, major=%d\n", MAJOR(dev_num));
    return 0;
}
static void __exit drv_exit(void) {
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("telemetry: unloaded\n");
}
module_init(drv_init);
module_exit(drv_exit);
MODULE_LICENSE("GPL");
