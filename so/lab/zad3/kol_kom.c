/*
 * kolejki komunikatow
 * rozwiazanie zad. 3 z SO LAB
 * autor: Maciek Makowski (mm189188)
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/signal.h>


#ifndef MODULE
error This is a module!
#endif

#define MODULE_VERSION "1.0"
#define MODULE_NAME "kol_kom"
MODULE_AUTHOR("Maciek Makowski");
MODULE_DESCRIPTION("kolejki komunikatow");

static int all_read_major = 42;
static int all_write_major = 43;
static int user_read_major = 44;
static int user_write_major = 45;

MODULE_PARM(all_read_major, "i");
MODULE_PARM(all_write_major, "i");
MODULE_PARM(user_read_major, "i");
MODULE_PARM(user_write_major, "i");

EXPORT_NO_SYMBOLS;

static struct proc_dir_entry *kom_dir, *all_file, *user_file;

/* struktury danych dla kolejek */

#define ADMIN_MSG_LEN 128
#define ADMIN_OP_LEN 3
#define MAX_MESSAGES 16
#define MESSAGE_LEN 128
#define USER_ID_LEN 24

#define OK 0
#define ENOSPACE 1
#define ENOUSER 2
#define EUSEREXISTS 3
#define ENOKMEM 4

struct msg_t {
  char msg[MESSAGE_LEN];
  int uid;
  struct msg_t *next, *prev;
};

struct msgqueue_t {
  int user_id;
  struct msg_t *usermsg;
  struct msg_t *allmsg;
  struct msgqueue_t *next, *prev;
  int user_count;
  int all_count;
  int abandoned;
  int last_uid;
};

struct src_dest_map_t {
  int src;
  int dest;
  struct src_dest_map_t *next, *prev;
};

struct mq_data_t {
  int maxmsgs;
  char running;
  int user_count;
  int msg_count;
  struct msgqueue_t *msgqueues;
  struct src_dest_map_t *src_dest_map;
};

struct mq_data_t mq_data;

/* 
 * funkcje do obslugi kolejek 
 */

/*
 * mq_get_user 
 * zwraca kolejke przyporzadkowana podanemu uzytkownikowi
 * lub 0 jesli uzytkownik nie jest zarejestrowany
 */
struct msgqueue_t *mq_get_user(struct mq_data_t *mq_data, int uid)
{
  struct msgqueue_t *user = mq_data->msgqueues;
 
  if (!user)
    return 0;

  do {
    if (user->user_id == uid)
      return user;
    else
      user = user->next;
  } while (user != mq_data->msgqueues); 

  return 0;
}


/* 
 * mq_remove_user_msg
 * usuwa prywatny komunikat
 */
int mq_remove_user_msg(struct mq_data_t *mq_data, 
		       struct msgqueue_t *user,
		       struct msg_t *msg)
{
  if (user->usermsg == msg) {
    if (msg->next == msg)
      user->usermsg = 0;
    else
      user->usermsg = msg->next;
  }
  msg->prev->next = msg->next;
  msg->next->prev = msg->prev;

  kfree((void *)msg);
  user->user_count--;
  mq_data->msg_count--;

  return OK;
}

/* 
 * mq_remove_all_msg
 * usuwa publiczny komunikat
 */
int mq_remove_all_msg(struct mq_data_t *mq_data, 
		       struct msgqueue_t *user,
		       struct msg_t *msg)
{
  if (user->allmsg == msg) {
    if (msg->next == msg)
      user->allmsg = 0;
    else
      user->allmsg = msg->next;
  }
  msg->prev->next = msg->next;
  msg->next->prev = msg->prev;

  kfree((void *)msg);
  user->all_count--;

  return OK;
}

/*
 * mq_clear_user
 * czysci wszystkie komunikaty dla danego uzytkownika
 */
int mq_clear_user(struct mq_data_t *mq_data, struct msgqueue_t *user)
{
  int rv;
  while (user->usermsg != 0)
    if ((rv = mq_remove_user_msg(mq_data, user, user->usermsg)) != OK)
      return rv;
  while (user->allmsg != 0)
    if ((rv = mq_remove_all_msg(mq_data, user, user->usermsg)) != OK)
      return rv;
  return OK;
}

/*
 * mq_clear_all
 * czysci wszystkie kolejki
 */
int mq_clear_all(struct mq_data_t *mq_data)
{
  struct msgqueue_t *user = mq_data->msgqueues;
  int rv = OK;

  if (!user)
    return OK;

  do {
    rv = mq_clear_user(mq_data, user);
    if (rv != OK)
      return rv;
    user = user->next;
  } while (user != mq_data->msgqueues); 

  return OK;
}

/*
 * mq_read_all
 * zwraca pierwszy komunikat w kolejce publicznej i usuwa go z kolejki
 */
int mq_read_all(struct mq_data_t *mq_data, int uid, char *buf, int len)
{
  struct msgqueue_t *user = mq_get_user(mq_data, uid);

  if (!user)
    return 0;
  if (user->allmsg == 0)
    return 0;

  if (len > MESSAGE_LEN)
    len = MESSAGE_LEN;

  strncpy(buf, user->allmsg->msg, len);
  mq_remove_all_msg(mq_data, user, user->allmsg);
  return len;
}

/*
 * mq_read_user
 * zwraca pierwszy komunikat w kolejce prywatnej i usuwa go z kolejki
 */
int mq_read_user(struct mq_data_t *mq_data, int uid, char *buf, int len)
{
  struct msgqueue_t *user = mq_get_user(mq_data, uid);

  if (!user)
    return 0;
  if (user->usermsg == 0)
    return 0;

  if (len > MESSAGE_LEN)
    len = MESSAGE_LEN;

  strncpy(buf, user->usermsg->msg, len);
  mq_remove_user_msg(mq_data, user, user->usermsg);
  return len;
}

/* 
 * mq_set_user_maxmsgs
 * czysci nadmiarowa liczbe komunikatow w kolejkach uzytkownika
 */
int mq_set_user_maxmsgs(struct mq_data_t *mq_data,
			struct msgqueue_t *user,
			int nmax)
{
  int rv;

  while (user->user_count > nmax)
    if ((rv = mq_remove_user_msg(mq_data, user, user->usermsg->prev)) != OK)
      return rv;
  while (user->all_count > nmax)
    if ((rv = mq_remove_all_msg(mq_data, user, user->allmsg->prev)) != OK)
      return rv;

  return OK;
}
 
/*
 * mq_set_maxmsgs
 * ustawia maksymalna liczbe komunikatow w kolejce, usuwajac
 * nadmiarowe, jesli to konieczne
 */
int mq_set_maxmsgs(struct mq_data_t *mq_data, int nmax)
{
  struct msgqueue_t *user = mq_data->msgqueues;
  int rv = OK;

  if (nmax > MAX_MESSAGES)
    nmax = MAX_MESSAGES;

  if (nmax < mq_data->maxmsgs && user)
    do {
      rv = mq_set_user_maxmsgs(mq_data, user, nmax);
      if (rv != OK)
	return rv;
      user = user->next;
    } while (user != mq_data->msgqueues); 

  mq_data->maxmsgs = nmax;
  return OK;
}  

/*
 * mq_add_user
 * rejestruje nowego uzytkownika
 */
int mq_add_user(struct mq_data_t *mq_data, int uid)
{
  struct msgqueue_t *user;
  
  if (mq_get_user(mq_data, uid) != 0)
    return EUSEREXISTS;

  if ((user = kmalloc(sizeof(struct msgqueue_t), GFP_KERNEL)) == 0)
    return ENOKMEM;
  user->user_id = uid;
  user->usermsg = user->allmsg = 0;
  user->next = user->prev = user;
  user->user_count = user->all_count = user->abandoned = 0;
  user->last_uid = -1;

  if (mq_data->msgqueues == 0)
    mq_data->msgqueues = user;
  else {
    user->prev = mq_data->msgqueues->prev;
    user->next = mq_data->msgqueues;
    user->next->prev = user;
    user->prev->next = user;
  }
  mq_data->user_count++;

  return OK;
    
}

/*
 * mq_remove_user
 * wyrejestrowuje uzytkownika
 */
int mq_remove_user(struct mq_data_t *mq_data, int uid)
{
  struct msgqueue_t *user = mq_get_user(mq_data, uid);

  if (user == 0)
    return ENOUSER;

  
  mq_data->user_count--;
  if (mq_clear_user(mq_data, user) != OK)
    mq_data->msg_count -= user->user_count;
  
  if (user->prev == user)
    mq_data->msgqueues = 0;
  else {
    if (mq_data->msgqueues == user)
      mq_data->msgqueues = user->next;
    user->prev->next = user->next;
    user->next->prev = user->prev;
  }
  kfree((void *)user);

  return OK;
}

/*
 * mq_add_user_msg
 * wstawia prywatna wiadomosc dla uzytkownika
 */
int mq_add_user_msg(struct mq_data_t *mq_data, int uid, int src, char *msg)
{
  struct msg_t *newmsg;
  struct msgqueue_t *user = mq_get_user(mq_data, uid);
  
  if (user == 0)
    return ENOUSER;
  
  if (user->user_count >= mq_data->maxmsgs) {
    user->abandoned++;
    return ENOSPACE;
  }

  if ((newmsg = kmalloc(sizeof(struct msg_t), GFP_KERNEL)) == 0)
    return ENOKMEM;
  strcpy(newmsg->msg, msg);
  newmsg->uid = src;
  newmsg->next = newmsg->prev = newmsg;
  
  if (user->usermsg == 0) 
    user->usermsg = newmsg;
  else {
    newmsg->prev = user->usermsg->prev;
    user->usermsg->prev = newmsg;
    newmsg->prev->next = newmsg;
    newmsg->next = user->usermsg;
  }
  user->last_uid = src;
  user->user_count++;
  mq_data->msg_count++;
  
  return OK;
}

/*
 * mq_add_all_msg
 * wstawia publiczna wiadomosc
 */
int mq_add_all_msg(struct mq_data_t *mq_data, int src, char *msg)
{
  struct msg_t *newmsg;
  struct msgqueue_t *user = mq_data->msgqueues;
  
  if (user == 0)
    return OK;
  
  do {
    if (user->all_count >= mq_data->maxmsgs)
      user->abandoned++;
    else {
      if ((newmsg = kmalloc(sizeof(struct msg_t), GFP_KERNEL)) == 0)
	return ENOKMEM;
      strcpy(newmsg->msg, msg);
      newmsg->uid = src;
      newmsg->next = newmsg->prev = newmsg;
    
      if (user->allmsg == 0) 
	user->allmsg = newmsg;
      else {
	newmsg->next = user->allmsg;
	newmsg->prev = user->allmsg->prev;
	user->allmsg->prev = newmsg;
	newmsg->prev->next = newmsg;
      }
      user->all_count++;
    }
    user = user->next;
  } while (user != mq_data->msgqueues);

  return OK;
}

/*
 * mq_get_src_dest_map
 * zwraca mape nadawca -> adresat
 */
struct src_dest_map_t *mq_get_src_dest_map(struct mq_data_t *mq_data,
					   int src)
{
  struct src_dest_map_t *sd = mq_data->src_dest_map;
 
  if (!sd)
    return 0;

  do {
    if (sd->src == src)
      return sd;
    else
      sd = sd->next;
  } while (sd != mq_data->src_dest_map); 

  return 0;
}

/*
 * mq_set_src_dest_map
 * ustawia adresata dla podanego nadawcy
 */
int mq_set_src_dest_map(struct mq_data_t *mq_data, int src, int dest)
{
  struct src_dest_map_t *sd = mq_get_src_dest_map(mq_data, src);
  
  if (sd == 0) {
    if ((sd = kmalloc(sizeof(struct src_dest_map_t), GFP_KERNEL)) == 0)
      return ENOKMEM;
    sd->src = src;
    sd->dest = dest;
    sd->next = sd->prev = sd;
    if (mq_data->src_dest_map == 0)
      mq_data->src_dest_map = sd;
    else {
      sd->next = mq_data->src_dest_map;
      sd->prev = sd->next->prev;
      sd->next->prev = sd;
      sd->prev->next = sd;
    }
  } else {
    sd->dest = dest;
  }
  return OK;
}

/*
 * mq_clean_src_dest_map
 * czysci mapowanie nadawca -> adresat dla zadanego nadawcy
 */
int mq_clean_src_dest_map(struct mq_data_t *mq_data, int src)
{
  struct src_dest_map_t *sd = mq_get_src_dest_map(mq_data, src);

  if (sd == 0)
    return ENOUSER;
  
  if (sd->prev == sd)
    mq_data->src_dest_map = 0;
  else {
    if (mq_data->src_dest_map == sd)
      mq_data->src_dest_map = sd->next;
    sd->prev->next = sd->next;
    sd->next->prev = sd->prev;
  }
  kfree((void *)sd);

  return OK;
}

/* /dev/all_read */

static ssize_t all_read_read(struct file * file, char * buf, size_t count, 
			     loff_t *filepos)
{
  int len;
  char lbuf[MESSAGE_LEN];

  if (!mq_data.running) {
    printk("kol_kom: stopped\n");
    return 0;
  }

  if (mq_get_user(&mq_data, current->uid) == 0) {
    printk("kol_kom: user %d not registered\n", current->uid);
    return 0;
  }
    
  len = mq_read_all(&mq_data, current->uid, lbuf, count);
  copy_to_user(buf, lbuf, len);

  return len;
}

static int all_read_open(struct inode* ino, struct file* filep)
{
  MOD_INC_USE_COUNT;
  return 0;   
}

static int all_read_close(struct inode* ino, struct file* filep)
{
  MOD_DEC_USE_COUNT;
  return 0;
}

static struct file_operations all_read_fops = {
  owner: THIS_MODULE,
  read: all_read_read,
  open: all_read_open,
  release: all_read_close,
};


/* /dev/all_write */

static ssize_t all_write_write(struct file * file, const char * buf, 
			       size_t count, 
			       loff_t *filepos)
{
  char msg[MESSAGE_LEN];
  int len, i;

  if (!mq_data.running) {
    printk("kol_kom: stopped\n");
    return count;
  }

  if (mq_get_user(&mq_data, current->uid) == 0) {
    printk("kol_kom: user %d not registered\n", current->uid);
    return count;
  }

  for (i = 0; i < MESSAGE_LEN; i++)
    msg[i] = ' ';

  len = count;
  if (count > MESSAGE_LEN)
    len = MESSAGE_LEN;

  copy_from_user(msg, buf, len);
  mq_add_all_msg(&mq_data, current->pid, msg);

  return count;
}

static int all_write_open(struct inode* ino, struct file* filep)
{
  MOD_INC_USE_COUNT;
  return 0;   
}

static int all_write_close(struct inode* ino, struct file* filep)
{
  MOD_DEC_USE_COUNT;
  return 0;
}

static struct file_operations all_write_fops = {
  owner: THIS_MODULE,
  write: all_write_write,
  open: all_write_open,
  release: all_write_close,
};


/* /dev/user_read */

static ssize_t user_read_read(struct file * file, char * buf, size_t count, 
			      loff_t *filepos)
{
  int len;
  char lbuf[MESSAGE_LEN];

  if (!mq_data.running) {
    printk("kol_kom: stopped\n");
    return 0;
  }

  if (mq_get_user(&mq_data, current->uid) == 0) {
    printk("kol_kom: user %d not registered\n", current->uid);
    return 0;
  }

  len = mq_read_user(&mq_data, current->uid, lbuf, count);
  copy_to_user(buf, lbuf, len);

  return len;
}

static int user_read_open(struct inode* ino, struct file* filep)
{
  MOD_INC_USE_COUNT;
  return 0;   
}

static int user_read_close(struct inode* ino, struct file* filep)
{
  MOD_DEC_USE_COUNT;
  return 0;
}

static struct file_operations user_read_fops = {
  owner: THIS_MODULE,
  read: user_read_read,
  open: user_read_open,
  release: user_read_close,
};


/* /dev/user_write */

static ssize_t user_write_write(struct file * file, const char * buf, 
				size_t count, 
				loff_t *filepos)
{
  char msg[MESSAGE_LEN];
  int len, i;

  if (!mq_data.running) {
    printk("kol_kom: stopped\n");
    return count;
  }

  if (mq_get_user(&mq_data, current->uid) == 0) {
    printk("kol_kom: user %d not registered\n", current->uid);
    return count;
  }

  for (i = 0; i < MESSAGE_LEN; i++)
    msg[i] = ' ';

  len = count;
  if (count > MESSAGE_LEN)
    len = MESSAGE_LEN;

  copy_from_user(msg, buf, len);
  mq_add_user_msg(&mq_data, 
		  mq_get_src_dest_map(&mq_data, current->pid)->dest, 
		  current->pid, 
		  msg);

  return count;
}

static int user_write_ioctl(struct inode* ino, struct file* filep,
			    unsigned int dest, unsigned long args)
{
  if (!mq_data.running) {
    printk("kol_kom: stopped\n");
    return 0;
  }

  if (mq_get_user(&mq_data, current->uid) == 0) {
    printk("kol_kom: user %d not registered\n", current->uid);
    return 0;
  }

  mq_set_src_dest_map(&mq_data, current->pid, dest);

  return 0;
}

static int user_write_open(struct inode* ino, struct file* filep)
{
  MOD_INC_USE_COUNT;
  mq_set_src_dest_map(&mq_data, current->pid, -1);
  return 0;   
}

static int user_write_close(struct inode* ino, struct file* filep)
{
  MOD_DEC_USE_COUNT;
  mq_clean_src_dest_map(&mq_data, current->pid);
  return 0;
}

static struct file_operations user_write_fops = {
  owner: THIS_MODULE,
  write: user_write_write,
  ioctl: user_write_ioctl,
  open: user_write_open,
  release: user_write_close,
};


/* /proc/komunikaty/all */

static int proc_all_read(char *page, char **start,
			 off_t off, int count, 
			 int *eof, void *data)
{
  int len;
  struct mq_data_t *mq_data = (struct mq_data_t *)data;
  
  MOD_INC_USE_COUNT;
  
  len = sprintf(page, "size: %d\nusers: %d\ncount: %d\n", 
		mq_data->maxmsgs, 
		mq_data->user_count, 
		mq_data->msg_count);

  MOD_DEC_USE_COUNT;

  return len;
}


static int proc_all_write(struct file *file,
			  const char *buffer,
			  unsigned long count, 
			  void *data)
{

  int len, size;
  struct mq_data_t *mq_data = (struct mq_data_t *)data;
  char lbuf[ADMIN_MSG_LEN + 1];
  char op[ADMIN_OP_LEN + 1];
  char *i;

  MOD_INC_USE_COUNT;

  len = count;
  if (len > ADMIN_MSG_LEN)
    len = ADMIN_MSG_LEN;

  if (copy_from_user(lbuf, buffer, len)) {
    MOD_DEC_USE_COUNT;
    return -EFAULT;
  }
  lbuf[len - 1] = 0;
  
  strncpy(op, lbuf, ADMIN_OP_LEN);
  op[ADMIN_OP_LEN] = 0;

  if (strcmp(op, "cle") == 0) {
    if (mq_clear_all(mq_data) != OK)
      printk("kol_kom: cannot clean all message queues\n");
  } else if (strcmp(op, "siz") == 0) {
    size = 0;
    for (i = lbuf + ADMIN_OP_LEN + 2; *i != 0; i++)
      size = size * 10 + (*i - '0');
    if (mq_set_maxmsgs(mq_data, size) != OK)
      printk("kol_kom: cannot change queue size to %d\n", size);
  } else if (strcmp(op, "sta") == 0) {
    if (mq_data->running)
      printk("kol_kom: already running\n");
    else
      mq_data->running = 1;
  } else if (strcmp(op, "sto") == 0) {
    if (!mq_data->running)
      printk("kol_kom: already stopped\n");
    else
      mq_data->running = 0;
  } else
    printk("kol_kom: invalid opertion: %s\n", op);


  MOD_DEC_USE_COUNT;

  return len;
}


/* /proc/komunikaty/user */


static int proc_user_read(char *page, char **start,
			 off_t off, int count, 
			 int *eof, void *data)
{
  int len;
  struct mq_data_t *mq_data = (struct mq_data_t *)data;
  struct msgqueue_t *user;

  MOD_INC_USE_COUNT;
 	
  user = mq_get_user(mq_data, current->uid);
  if (user == 0)
    len = sprintf(page, "user %d not registered\n", current->uid);
  else
    len = sprintf(page, "count: %d\nderelict: %d\nw_pid: %d\n",
		  user->user_count,
		  user->abandoned,
		  user->last_uid);

  MOD_DEC_USE_COUNT;

  return len;
}


static int proc_user_write(struct file *file,
			  const char *buffer,
			  unsigned long count, 
			  void *data)
{
  int len;
  struct mq_data_t *mq_data = (struct mq_data_t *)data;
  char lbuf[ADMIN_MSG_LEN + 1];
  char op[ADMIN_OP_LEN + 1];
  int uid;
  char *i;

  MOD_INC_USE_COUNT;

  len = count;
  if (len > ADMIN_MSG_LEN)
    len = ADMIN_MSG_LEN;

  if (copy_from_user(lbuf, buffer, len)) {
    MOD_DEC_USE_COUNT;
    return -EFAULT;
  }
  lbuf[len - 1] = 0;
  
  strncpy(op, lbuf, ADMIN_OP_LEN);
  op[ADMIN_OP_LEN] = 0;

  uid = 0;
  for (i = lbuf + ADMIN_OP_LEN + 1; *i != 0; i++)
    uid = uid * 10 + (*i - '0');

  if (strcmp(op, "add") == 0) {
    if (mq_add_user(mq_data, uid) != OK)
      printk("kol_kom: cannot add user %d\n", uid);
  } else if (strcmp(op, "del") == 0) {
    if (mq_remove_user(mq_data, uid) != OK)
      printk("kol_kom: cannot delete user %d\n", uid);
  } else
    printk("kol_kom: invalid opertion: %s\n", op);


  MOD_DEC_USE_COUNT;

  return len;
}



/* 
 * Interfejs do jadra
 */


extern int printk(const char* fmt, ...);


static int __init init_kol_kom(void)
{
 
  printk( "kol_kom: initializing module\n");

  /* dane */
  mq_data.maxmsgs = MAX_MESSAGES;
  mq_data.running = 1;
  mq_data.user_count = 0;
  mq_data.msg_count = 0;
  mq_data.msgqueues = 0;
  mq_data.src_dest_map = 0;

  /* dev */
  if (register_chrdev(all_read_major, "all_read", &all_read_fops)) {
    printk("register_chrdev failed: all_read\n");
    return -EIO;
  }
  if (register_chrdev(all_write_major, "all_write", &all_write_fops)) {
    printk("register_chrdev failed: all_write\n");
    return -EIO;
  }
  if (register_chrdev(user_read_major, "user_read", &user_read_fops)) {
    printk("register_chrdev failed: user_read\n");
    return -EIO;
  }
  if (register_chrdev(user_write_major, "user_write", &user_write_fops)) {
    printk("register_chrdev failed: user_write\n");
    return -EIO;
  }

  /* proc */

  kom_dir = proc_mkdir("komunikaty", NULL);
  if(kom_dir == NULL) {
    printk("proc_mkdir failed: komunikaty\n");
    return -ENOMEM;
  }
        
  kom_dir->owner = THIS_MODULE;
        
  all_file = create_proc_entry("all", 0644, kom_dir);
  if(all_file == NULL) {
    printk("create_proc_entry failed: all\n");
    return -ENOMEM;
  }

  all_file->data = &mq_data;
  all_file->read_proc = proc_all_read;
  all_file->write_proc = proc_all_write;
  all_file->owner = THIS_MODULE;
                
  user_file = create_proc_entry("user", 0644, kom_dir);
  if(user_file == NULL) {
    printk("create_proc_entry failed: user\n");
    return -ENOMEM;
  }

  user_file->data = &mq_data;
  user_file->read_proc = proc_user_read;
  user_file->write_proc = proc_user_write;
  user_file->owner = THIS_MODULE;
                
  return 0;
}

static void __exit cleanup_kol_kom(void)
{
  printk("kol_kom: cleaning up module\n");

  if (unregister_chrdev(all_read_major, "all_read") != 0)
    printk("unregister_chrdev failed: all_read\n");
  if (unregister_chrdev(all_write_major, "all_write") != 0)
    printk("unregister_chrdev failed: all_write\n");
  if (unregister_chrdev(user_read_major, "user_read") != 0)
    printk("unregister_chrdev failed: user_read\n");
  if (unregister_chrdev(user_write_major, "user_write") != 0)
    printk("unregister_chrdev failed: user_write\n");
  remove_proc_entry("all", kom_dir);
  remove_proc_entry("user", kom_dir);
  remove_proc_entry("komunikaty", NULL);
}


module_init(init_kol_kom);
module_exit(cleanup_kol_kom);






