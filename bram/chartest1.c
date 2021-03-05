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

#define CHRDEVBASE_MAJOR 200 // 主设备号
#define CHRDEVBASE_NAME "chrdevbase" // 设备名

static char readbuf[100]; // 读缓冲区
static char writebuf[100]; // 写缓冲区
static char kerneldata[] = {"kernel data!"};

 /*
   * @description : 打开设备
   * @param – inode : 传递给驱动的 inode
   * @param - filp : 设备文件， file 结构体有个叫做 private_data 的成员变量
   * 一般在 open 的时候将 private_data 指向设备结构体。
   * * @return : 0 成功;其他 失败
   */
 static int chrtest_open(struct inode *inode, struct file *filp)
 {
    /*实现具体功能*/
      //printk("chrdevbase open!\r\n");
     return 0;
 }

 /*
   * @description : 从设备读取数据
   * @param - filp : 要打开的设备文件(文件描述符)
   * @param - buf : 返回给用户空间的数据缓冲区
   * @param - cnt : 要读取的数据长度
   * @param - offt : 相对于文件首地址的偏移
   * @return : 读取的字节数，如果为负值，表示读取失败
   */
 static ssize_t chrtest_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
 {
   /*实现具体功能*/
     int retvalue = 0;

   /* 向用户空间发送数据 */
     memcpy(readbuf, kerneldata, sizeof(kerneldata));
     retvalue = copy_to_user(buf, readbuf, cnt);//因为内核空间不能直接操作用户空间的内存，因此需要借助 copy_to_user 函数来完成内核空间的数据到用户空间的复制
     if(retvalue == 0)
     {
     printk("kernel senddata ok!\r\n");
     }
     else
     {
     printk("kernel senddata failed!\r\n");
     }

   //printk("chrdevbase read!\r\n");
   return 0;
 }

 /*
   * @description : 向设备写数据
   * @param - filp : 设备文件，表示打开的文件描述符
   * @param - buf : 要写给设备写入的数据
   * @param - cnt : 要写入的数据长度
   * @param - offt : 相对于文件首地址的偏移
   * @return : 写入的字节数，如果为负值，表示写入失败
   */
 static ssize_t chrtest_write(struct file *filp,const char __user *buf,size_t cnt, loff_t *offt)
{
      int retvalue = 0;
    /* 接收用户空间传递给内核的数据并且打印出来 */
     retvalue = copy_from_user(writebuf, buf, cnt);
     if(retvalue == 0)
     {
     printk("kernel recevdata:%s\r\n", writebuf);
     }
     else
     {
     printk("kernel recevdata failed!\r\n");
     }
    
     //printk("chrdevbase write!\r\n");
   return 0;
}

  /*
   * @description : 关闭/释放设备
   * @param - filp : 要关闭的设备文件(文件描述符)
   * @return : 0 成功;其他 失败
   */
 static int chrtest_release(struct inode *inode, struct file *filp)
 {
   /*实现具体功能*/
   //printk("chrdevbase release！ \r\n");
   return 0;
 }


  /*
   * 设备操作函数结构体
   */
 static struct file_operations test_fops = {
              .owner = THIS_MODULE,
              .open = chrtest_open,
              .read = chrtest_read,
              .write = chrtest_write,
              .release = chrtest_release,
   };

    /*
    * @description : 驱动入口函数
    * @param : 无
    * @return : 0 成功;其他 失败
    */
    static int __init chrdevbase_init(void)
   {
       /* 入口函数具体内容 */
       int retvalue = 0;

      /* 注册字符设备驱动 */
       retvalue = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);//调用函数 register_chrdev 注册字符设备，主设备号为 200，设备名为“ chrtest”
       if(retvalue < 0)
       {
         printk("chrdevbase driver register failed\r\n");
      /* 字符设备注册失败,自行处理 */
       }
       printk("chrdevbase_init()\r\n");
      return 0;
 }

 /* 驱动出口函数 */
  static void __exit xxx_exit(void)
   {
       /* 注销字符设备驱动 */
       unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);//调用函数 unregister_chrdev 注销主设备号为 200 的这个设备。
       printk("chrdevbase_exit()\r\n");
   }

 /* 将上面两个函数指定为驱动的入口和出口函数 */
 module_init(xxx_init);
 module_exit(xxx_exit);

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("xin.han");
