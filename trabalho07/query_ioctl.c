#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <asm/uaccess.h> 

#include "query_ioctl.h"
 
#define FIRST_MINOR 0
#define MINOR_CNT 5
 
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static int data[MINOR_CNT] = {0};
static int driver = 0;
 
static int my_open(struct inode *i, struct file *f)
{
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    query_arg_t q;
    switch (cmd)
    {
        case QUERY_GET_VARIABLES:
            if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            driver = q.i;
            q.data = data[driver];
            if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            break;
        case QUERY_CLR_VARIABLES:
            data[driver] = 0;
            break;
        case QUERY_SET_VARIABLES:
            if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            driver = q.i;
            data[driver] = q.data + MINOR(dev)+driver;
            break;
        default:
            return -EINVAL;
    }
 
    return 0;
}
 
static struct file_operations query_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,

    .unlocked_ioctl = my_ioctl
};
 
static int __init query_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;
    int i;
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
    {
        return ret;
    }
    
    cdev_init(&c_dev, &query_fops);
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
        
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    for (i=0; i<5; i++){
        if (IS_ERR(dev_ret = device_create(cl, NULL, MKDEV(MAJOR(dev), MINOR(dev) + i), NULL, "query%d", i)))
        {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
        }
    }

	return 0;
}
 
static void __exit query_ioctl_exit(void)
{
    int i;
    for(i=0; i<5; i++){
        device_destroy(cl, MKDEV(MAJOR(dev), MINOR(dev) + i));
    }
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}
 
module_init(query_ioctl_init);
module_exit(query_ioctl_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trabalho MC504");
MODULE_DESCRIPTION("Utilizando multiplos drivers para armazenamento de valores");
