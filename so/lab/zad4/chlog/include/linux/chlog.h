/*
 * file changelog exports
 * Maciek Makowski 2002
 */

typedef int unw_t;

extern int chlog_add_entry(struct file *file);
extern int chlog_start(void);
extern int chlog_stop(void);
extern unw_t chlog_oldest_unw(void);
extern unw_t chlog_next_unw(unw_t next_unw);
extern int chlog_read_entry(unw_t unw, char *namebuf, int bufsize, int noblock);
