#include <linux/module.h>  // module_init  module_exit
#include <linux/init.h>    // __init   __exit
#include <linux/fs.h>      // file_operations   没写会报错：xxx has initializer but incomplete type

#define MYNMAJOR  200
#define MYNAME    "chardev"

char kbuf[100];


/*file_operations结构体变量中填充的函数指针的实体，函数的格式要遵守*/
static int test_chrdev_open(struct inode *inode, struct file *file)
{
    //这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
    printk(KERN_ERR "test_module_open\n");
      return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_ERR "test_chrdev_release\n");
    return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t size, loff_t *ppos)//struct file *file：指向我们要操作的文件;const char __user *buf：用户空间的buf
{
    int ret = -1;
    printk(KERN_ERR "test_chrdev_read\n");

    kbuf[0] = '1';
    ret = copy_to_user(ubuf,kbuf,size);
    if(ret){
        printk(KERN_ERR "copy_to_user fail\n");
        return -EINVAL;//在真正的的驱动中没复制成功应该有一些纠错机制，这里我们简单点
    }
    printk(KERN_ERR "copy_to_user success..\n");
    }
    return 0;
}
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)
{
    int ret = -1;
    printk(KERN_INFO "test_chrdev_write\n");
    //使用该函数将应用层的传过来的ubuf中的内容拷贝到驱动空间(内核空间)的一个buf中
    //memcpy(kbuf,ubuf);     //不行，因为2个不在一个地址空间中
    memset(kbuf, 0, sizeof(kbuf));
    ret = copy_from_user(kbuf,ubuf,count);
    if(ret){
        printk(KERN_ERR "copy_from_user fail, ret = %d\n", ret);
        return -EINVAL;//在真正的的驱动中没复制成功应该有一些纠错机制，这里我们简单点
    }
    printk(KERN_ERR "copy_from_user success..\n");
    //到这里我们就成功把用户空间的数据转移到内核空间了

    //真正的驱动中，数据从应用层复制到驱动中后，我们就要根据这个数据去写硬件完成硬件的操作
    //所以下面就应该是操作硬件的代码
    return 0;
}

//自定义一个file_operations结构体变量，并填充
static const struct file_operations test_module_fops = {
    .owner		= THIS_MODULE,         //惯例，所有的驱动都有这一个，这也是这结构体中唯一一个不是函数指针的元素
    .open		  = test_chrdev_open,    //将来应用open打开这个这个设备时实际调用的函数
    .release	= test_chrdev_release,   //对应close，为什么不叫close呢？详见后面release和close的区别的讲解
    .read       = test_chrdev_read,
    .write      = test_chrdev_write,
};

/*********************************************************************************/
// 模块安装函数
static int __init chrdev_init(void)
{
    printk(KERN_ERR "chrdev_init helloworld init\n");

    //在module_init宏调用的函数中去注册字符设备驱动
    int ret = -1;     //register_chrdev 返回值为int类型
    ret = register_chrdev(MYNMAJOR, MYNAME, &test_module_fops);
    //参数：主设备号major，设备名称name,自己定义好的file_operations结构体变量指针，注意是指针，所以要加上取地址符
    //完了之后检查返回值
    if(ret){
        printk(KERN_ERR "register_chrdev fial\n");  //注意这里不再用KERN_INFO
        return -EINVAL; //内核中定义了好多error number 不都用以前那样return -1;负号要加 ！！
    }
    printk(KERN_ERR "register_chrdev success...\n");
    return 0;
}

// 模块卸载函数
static void __exit chrdev_exit(void)
{
    printk(KERN_INFO "chrdev_exit helloworld exit\n");
    unregister_pm_notifier(&pm_notifer);
    //在module_exit宏调用的函数中去注销字符设备驱动
    //实验中，在我们这里不写东西的时候，rmmod 后lsmod 查看确实是没了，但是cat /proc/device发现设备号还是被占着
    unregister_chrdev(MYNMAJOR, MYNAME);  //参数就两个
    //检测返回值

    return 0;
}
/*********************************************************************************/

module_init(chrdev_init);        //insmod 时调用
module_exit(chrdev_exit);        //rmmod  时调用

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");		      // 描述模块的许可证
