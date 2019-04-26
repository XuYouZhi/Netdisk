#include "func.h"

//跨进程dup机制
int sendFd(int sockPipeFd,int fd,int exitFlag)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));//一定要清空
	struct iovec iov[2];
	iov[0].iov_base=&exitFlag;
	iov[0].iov_len=4;
	char buf[10]="world";
	iov[1].iov_base=buf;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	int cmsglen=CMSG_LEN(sizeof(int));
	struct cmsghdr *cmsg;
	cmsg=(struct cmsghdr *)calloc(1,cmsglen);
	cmsg->cmsg_len=cmsglen;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;
	msg.msg_control=cmsg;//内核控制信息指针赋值
	msg.msg_controllen=cmsglen;
	int ret;
	ret=sendmsg(sockPipeFd,&msg,0);
	ERROR_CHECK(ret,-1,"sendmsg");
	return 0;
}
int recvFd(int sockPipeFd,int *fd,int *exitFlag)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));//一定要清空
	struct iovec iov[2];
	char buf1[10];
	iov[0].iov_base=exitFlag;
	iov[0].iov_len=4;
	iov[1].iov_base=buf1;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	int cmsglen=CMSG_LEN(sizeof(int));
	struct cmsghdr *cmsg;
	cmsg=(struct cmsghdr *)calloc(1,cmsglen);
	cmsg->cmsg_len=cmsglen;
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control=cmsg;//内核控制信息指针赋值
	msg.msg_controllen=cmsglen;
	int ret;
	ret=recvmsg(sockPipeFd,&msg,0);
	ERROR_CHECK(ret,-1,"recvmsg");
	*fd=*(int*)CMSG_DATA(cmsg);
	return 0;
}
