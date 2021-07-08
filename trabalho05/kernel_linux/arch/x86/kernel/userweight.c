#include <linux/unistd.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#define MAX_USERS 100

static int user_weights[MAX_USERS] = {-1};

SYSCALL_DEFINE1(getuserweight, int, uid)
{
  if(uid > MAX_USERS){
    return -1;
  }
  else{
    return(user_weights[uid]);
  } 
}

SYSCALL_DEFINE2(setuserweight, int, uid, int, weight)
{
  if(uid > MAX_USERS){
    return -1;
  }
  else{
    user_weights[uid] = weight;
    return(0);
  }
}
