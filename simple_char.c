#include <linux/kernel.h>
#include <linux/module.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kthread.h>
#include<linux/fs.h>
#include<linux/ioport.h>
#include<linux/wait.h>
#include<linux/poll.h>
#include "myioctl.h"


MODULE_AUTHOR("SHIVA");
MODULE_LICENSE("GPL");


static int mychar_open(struct inode *inode,struct file *file);
static int mychar_close(struct inode *inode,struct file *file);
static ssize_t mychar_read(struct file *file,char *buff,size_t len,loff_t *off_set);
static ssize_t mychar_write(struct file *file,const char *buff,size_t len,loff_t *off_set);

static long mychar_ioctl(struct file *file,unsigned int cmd,unsigned long arg);
struct d_context
{
   wait_queue_head_t wq;
   int no_chars;
   int IRQ_line_no;
   char buf[1024];
   struct cdev c_dev;
   int base_address;
}my_context;

   static struct class *dev_class;
   dev_t devno=0;
  
  
  static struct file_operations my_routine=
  {
       .open=mychar_open,
       .release=mychar_close,
       .read=mychar_read,
       .write=mychar_write,
       .unlocked_ioctl=mychar_ioctl
  };
    
 int mydev_init(void)
    {
    int res=0;
    devno=MKDEV(42,0);
    res=register_chrdev_region(devno,1,"shiva_char");
    if(res<0)
    {
       printk("<1>""shiva char is not initialised \n");
       return res;
     }
     else
     {
        printk("<1>""shivaa char is initialised successfully\n");
       
     }
    
    
     cdev_init(&my_context.c_dev,&my_routine);
      
     my_context.c_dev.owner=THIS_MODULE;
     
     res=cdev_add(&my_context.c_dev,devno,1);
     
    init_waitqueue_head(&my_context.wq);
     
     
     if(res<0)
     {
        printk("1""cdev object is not loaded successfully\n");
        unregister_chrdev_region(devno,1);
      }
      
       
       printk("<1>""cdev object is loaded successfully\n");
      	 
      	 
      	 
      
      my_context.no_chars=0;
      if((dev_class = class_create(THIS_MODULE,"char_class"))==NULL)
      {
         printk(KERN_INFO "cannot create struct class for device\n");
         goto r_class;
      }
      if((device_create(dev_class,NULL,devno,NULL,"char_device"))==NULL)
      {
         printk(KERN_INFO"can't create device file\n");
         goto r_device;
      }
      
      
      printk(KERN_INFO"kernel module inserted successfully..\n");
      return res;
  r_class:
   
      class_destroy(dev_class);
      
  r_device:
    
      unregister_chrdev_region(devno,1);
      return -1;
    //pritnk("<1>""shiva char unloaded successfully\n");
 
 
 
 
 }
 
 static void mydev_release(void)
 {
    dev_t devno=MKDEV(42,0);
    device_destroy(dev_class,devno);
    class_destroy(dev_class);
    cdev_del(&my_context.c_dev);
    unregister_chrdev_region(devno,1);
    printk("<1>""shiva char is unloaded successfully\n");
    
 }

 
 
 static int mychar_open(struct inode *inode, struct file *file)
 {
    try_module_get(THIS_MODULE);
    file->private_data=&my_context;
    if(file->f_mode & FMODE_READ)
    printk("<1>""open for read mode\n");
    if(file->f_mode & FMODE_WRITE);
    printk("<1>""open for write mode\n");
    return 0;
 }
 
 
 static int mychar_close(struct inode *inode,struct file *file)
 {
     module_put(THIS_MODULE);
     printk("<1>""device file is closed\n");
     return 0;
 }
 static ssize_t mychar_write(struct file *file,const char *buff,size_t len,loff_t *offset)
 
 {
    struct d_context *tdev;
    tdev=file->private_data;
    if(len>1024)
    len=1024;
    copy_from_user(tdev->buf,buff,len);
    tdev->no_chars=len;
    wake_up_interruptible(&tdev->wq);
    printk("<1>""write is called\n");
    return (ssize_t)len;
 }   
    
static ssize_t mychar_read(struct file *file,char *buff,size_t len,loff_t *offset)
 
 {
    struct d_context *tdev;
    tdev=file->private_data;
    if(len>tdev->no_chars)
    len=tdev->no_chars;
    wait_event_interruptible(tdev->wq,tdev->no_chars>0);
    copy_to_user(buff,tdev->buf,len);
    tdev->no_chars=0;
    printk("<1>""read is called\n");
    return (ssize_t)len;
 }
 
 
 static long int mychar_ioctl(struct file *file,unsigned int cmd, unsigned long arg)
 {
 	char rbuf[20];
 	char wbuf[20]="hello";
 	switch(cmd)
 	{
 		case RD:
 			copy_to_user((char*)arg,wbuf,sizeof(wbuf));//put_usr()
 			break;
 		case WR:
 			copy_from_user(rbuf,(char*)arg,sizeof(rbuf));//get_usr()
 			printk("<1>""received data at kernel level :%s\n",rbuf);
 			break;
 		default :return -1;
 	}
 	return 0;
 }
 
 				
 
module_init(mydev_init); 
module_exit(mydev_release);
 
