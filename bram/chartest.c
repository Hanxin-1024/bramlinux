#include "stdio.h"
#include "string.h"
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>

// static struct file_operations test_fops; //就是设备的操作函数集合，只是此时还没有初始化 test_fops 中的 open、 release 等这些成员变量，所以这个操作函数集合还是空的
/*cat /proc/devices可以查看设备号*/

 /*打开设备*/
 static int chrtest_open(struct inode *inode, struct file *filp)
 {
    /*实现具体功能*/
     return 0;
 }

 /*从设备读取*/
 static ssize_t chrtest_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
 {
   /*实现具体功能*/
   return 0;
 }

 /*向设备写数据*/
 static ssize_t chrtest_write(struct file *filp,const char __user *buf,size_t cnt, loff_t *offt)
{
   /*实现具体功能*/
   return 0;
}

/*关闭释放设备*/
 static int chrtest_release(struct inode *inode, struct file *filp)
 {
   /*实现具体功能*/
   return 0;
 }

 static struct file_operations test_fops = {
              .owner = THIS_MODULE,
              .open = chrtest_open,
              .read = chrtest_read,
              .write = chrtest_write,
              .release = chrtest_release,
   };

  /* 驱动入口函数 */
    static int __init xxx_init(void)
   {
       /* 入口函数具体内容 */
       int retvalue = 0;

      /* 注册字符设备驱动 */
       retvalue = register_chrdev(200, "chrtest", &test_fops);//调用函数 register_chrdev 注册字符设备，主设备号为 200，设备名为“ chrtest”
       if(retvalue < 0){
      /* 字符设备注册失败,自行处理 */
   }
      return 0;
 }

 /* 驱动出口函数 */
  static void __exit xxx_exit(void)
   {
       /* 注销字符设备驱动 */
       unregister_chrdev(200, "chrtest");//调用函数 unregister_chrdev 注销主设备号为 200 的这个设备。
   }

 /* 将上面两个函数指定为驱动的入口和出口函数 */
 module_init(xxx_init);
 module_exit(xxx_exit);

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("xin.han");
