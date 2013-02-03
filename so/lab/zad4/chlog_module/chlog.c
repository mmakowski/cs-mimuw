/*
 * dziennik zmian w plikach
 * rozwiazanie zad. 4 z SO LAB
 * autor: Maciek Makowski (mm189188)
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/chlog.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/signal.h>


#ifndef MODULE
error This is a module!
#endif

#define MODULE_VERSION "1.0"
#define MODULE_NAME "chlog"
MODULE_AUTHOR("Maciek Makowski");
MODULE_DESCRIPTION("file change log");

static int major = 201;
MODULE_PARM(major, "i");


#define MAX_PATH 4096





EXPORT_NO_SYMBOLS;

/* /dev/chlog */

static ssize_t chlog_read(struct file * file, char * buf, size_t count, 
			     loff_t *filepos)
{
  char name[MAX_PATH + 1];
  int err;

  err = chlog_read_entry(file->f_pos, name, MAX_PATH + 1, file->f_flags & O_NONBLOCK);
  
  if (err < 0)
    return err;

  if (MAX_PATH > count)
    return -EMSGSIZE;

  copy_to_user(buf, name, count);

  return count;
}

static int chlog_open(struct inode* ino, struct file* filep)
{
  MOD_INC_USE_COUNT;
  return 0;   
}

static int chlog_close(struct inode* ino, struct file* filep)
{
  MOD_DEC_USE_COUNT;
  return 0;
}


static loff_t chlog_lseek(struct file * file, loff_t pos, int whence) {

  switch (whence) {
  case 2:
    file->f_pos = chlog_oldest_unw();
    break;
  case 1:
    file->f_pos = chlog_next_unw(file->f_pos);
    break;
  case 0:
    file->f_pos = pos;
    break;
  default:
  }
  return file->f_pos;
}

static ssize_t chlog_write(struct file * file, const char * buf, 
			       size_t count, 
			       loff_t *filepos)
{
  char a;

  if (count > 1)
    return -EINVAL;

  copy_from_user(&a, buf, count);
  switch (a) {
  case '0':
    printk("chlog: stopping...\n");
    chlog_stop();
    break;
  case '1':
    printk("chlog: starting...\n");
    chlog_start();
    break;
  default:
    return -EINVAL;
  }
 
  return 1;
}

static struct file_operations chlog_fops = {
  owner: THIS_MODULE,
  read: chlog_read,
  write: chlog_write,
  llseek: chlog_lseek,
  open: chlog_open,
  release: chlog_close,
};

/* 
 * Interfejs do jadra
 */


extern int printk(const char* fmt, ...);


static int __init init_chlog(void)
{
 
  printk( "chlog: initializing module\n");

  /* dev */
  if (register_chrdev(major, "chlog", &chlog_fops)) {
    printk("register_chrdev failed: chlog\n");
    return -EIO;
  }

  return 0;
}

static void __exit cleanup_chlog(void)
{
  printk("chlog: cleaning up module\n");

  chlog_stop();
  if (unregister_chrdev(major, "chlog") != 0)
    printk("unregister_chrdev failed: all_read\n");
}


module_init(init_chlog);
module_exit(cleanup_chlog);









