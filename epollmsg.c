#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/msg.h>
#include <sys/socket.h>


#define NQ 3
#define MAXMSZ 512
#define KEY 0x123

struct thread_info {
	int qid;
	int fd;
};

struct mymesg {
	long mttype;
	char mtext[MAXMSZ];

};

void* helper (void*arg){
	
	int n;
	struct mymesg m;
	struct thread_info* tip = arg;
	
	for(;;){
		memset(&m, 0, sizeof(m));
		
		if((n = msgrcv(tip->qid, &m, MAXMSZ, 0, MSG_NOERROR)) < 0)
			err_sys("msgrcv error");
		
		if(write(tip->fd, m.mtext, n) < 0)
			err_sys("write error");
		
	}
	
}

int main(){
	
	int i,n,err;
	int fd[2];
	int qid[NQ];
	pthread_t tid[NQ];
	struct thread_info ti[NQ];
	//struct pollfd pfd[NQ];
	char buf[MAXMSZ];

	struct epoll_event ev;
	int epollfd, nfds;
	int epoll_fd[NQ];

	epollfd = epoll_create(10);
	if(epollfd < 0){
		err_exit(err, "epoll_create fail.");
	}
	

	for(i =0; i<NQ; i++){
		if((qid[i] = msgget((KEY+i), IPC_CREAT|0666)) < 0)
			err_sys("msgget error");
		
		printf("queue ID %d is %d\n", i, qid[i]);
		
		if(socketpair(AF_UNIX, SOCK_DGRAM, 0, fd) < 0)
			err_sys("socketpair error");
		
		//pfd[i].fd = fd[0];
		//pfd[i].events = POLLIN;
		
		epoll_fd[i] = fd[0];

		ev.events = EPOLLIN;
		ev.data.fd = fd[0];
		if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd[0], &ev) == -1)
			err_exit(err, "epoll_ctl add fail");
		
		ti[i].qid = qid[i];
		ti[i].fd = fd[1];
		
		if((err = pthread_create(&tid[i], NULL, helper, &ti[i])) != 0)
			err_exit(err, "pthread_create error");
	
	}


	for(;;){
		
		//if (poll(pfd, NQ, -1) < 0)
		nfds = epoll_wait(epollfd, &ev, NQ, -1);
		if(nfds< 0)
			err_sys("poll err");

		if((n=read(ev.data.fd, buf, sizeof(buf))) < 0)
				err_sys("read error");
			buf[n] = 0;
		
		//this "for" loop just to get qid[i].
		for(i = 0; i< NQ; i++){
			if(ev.data.fd == epoll_fd[i])
				break;
		}
		
		printf("queue id %d, message %s\n", qid[i], buf);		
	}
	exit(0);
}



