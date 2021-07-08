#include <linux/unistd.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(mycall)
{
  return(3);
}

/*asmlinkage long sys_mycall(void) {
  return(3);
}*/
