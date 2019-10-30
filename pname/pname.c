#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/string.h>
#include "pname.h"
#define TASK_COMM_LEN 16

asmlinkage long sys_pnametoid(char *process_name) {

   /* Structure of a task or process */
   struct task_struct *task;
   char *task_name[TASK_COMM_LEN];

   /* Copy process_name from user-space to task_name in kernal-space */
   copy_from_user(task_name, process_name, TASK_COMM_LEN);

   for_each_process(task) {

      /* Found the task whose name is task_name */
      if (strcmp(task->comm, task_name) == 0) {

         /* Return id of current task or process */
         return task_pid_nr(task);
      }
   }

   return 0;
}
