#include <asm/unistd.h>
#include <asm/cacheflush.h>
#include <asm/pgtable_types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("1712747-1712645");

void **system_call_table_addr;

asmlinkage long      (*original_open) (const char *, int, mode_t);
asmlinkage ssize_t   (*original_write) (int, const void *, size_t);

asmlinkage long hooked_open (const char *pathname, int flags, mode_t mode) {
   char *kfname   = kmalloc(1024, GFP_KERNEL);
   copy_from_user(kfname, pathname, 1024);
   printk(KERN_INFO "Process %s opens %s", current->comm, kfname);
   kfree(kfname);
   return original_open(pathname, flags, mode);
}

asmlinkage ssize_t hooked_write (int fd, const void *buf, size_t nbytes) {
   /* Retrieve pathname from file descriptor using file descriptor table
   Reference: https://code.woboq.org/linux/linux/include/linux/fdtable.h.html#fdtable */

   char *buffer   = kmalloc(1024, GFP_KERNEL);
   char *kfname   = d_path(&fcheck_files(current->files, fd)->f_path, buffer, 1024);
   printk(KERN_INFO "Process %s writes %zu bytes to %s", current->comm, nbytes, kfname);
   kfree(buffer);
   return original_write(fd, buf, nbytes);
}

/* Make page writeable */
int make_rw (unsigned long address) {
   unsigned int level;
   pte_t *pte  = lookup_address(address, &level);
   if (pte->pte &~_PAGE_RW) {
      pte->pte |=_PAGE_RW;
   }
   return 0;
}

/* Make page write protected */
int make_ro (unsigned long address) {
   unsigned int level;
   pte_t *pte  = lookup_address(address, &level);
   pte->pte    = pte->pte &~_PAGE_RW;
   return 0;
}

static int __init entry_point(void) {
   printk(KERN_INFO "Loaded hook successfully...\n");

   /* Get current sys_call_table address */
   system_call_table_addr = (void **) kallsyms_lookup_name("sys_call_table");
   
   /* Save current system call */
   original_open  = system_call_table_addr[__NR_open];
   original_write = system_call_table_addr[__NR_write];

   /* Disable page protection */
   make_rw((unsigned long) system_call_table_addr);

   /* Load hooked system call */
   system_call_table_addr[__NR_open]   = hooked_open;
   system_call_table_addr[__NR_write]  = hooked_write;

   /* Renable page protection */
   make_ro((unsigned long) system_call_table_addr);
   return 0;
}

static void __exit exit_point(void) {
   printk(KERN_INFO "Unloaded hook successfully\n");

   /* Disable page protection */
   make_rw((unsigned long) system_call_table_addr);

   /* Restore original system call */
   system_call_table_addr[__NR_open]   = original_open;
   system_call_table_addr[__NR_write]  = original_write;

   /* Renable page protection */
   make_ro((unsigned long) system_call_table_addr);
}

module_init(entry_point);
module_exit(exit_point);
