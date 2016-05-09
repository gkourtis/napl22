#define BUFFSIZE 0x1
#define MAXPENDING 0x10

bool fcntl_add_flag(int fd,int flags){
	int f;bool r;
	f=fcntl(fd,F_GETFL);if(f<0) return false;
	f=fcntl(fd,F_SETFL,f|flags);r=(f>=0);
	std::cout<< (r ? "changed flags of file descriptor:" : "unable to change flags of file descriptor:");
	return r;
}


void removeEndedThreads(){
// the loop is done in opposite direction because due to iterator invalidation there could be probems
	for(auto i=threads.end();i--!=threads.begin();)
		if(i->joinable() && threadVar(*i,threadEnded)){
			i->join();threads.erase(i);wait();
		}
}

in_port_t HandleIncomingConnections(in_port_t socketNum){
	int serversock;
	struct sockaddr_in serverT, clientT;
	socklen_t sizeof_clientT = sizeof(clientT);

	while((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) <0)
		std::cout<<"socket creation failed...";
	
	fcntl_add_flag(serversock,O_NONBLOCK);
/*
		int f;
		if(f=fcntl(serversock,F_GETFL)>=0)
			if(fcntl(serversock,F_SETFL,f|O_NONBLOCK)>=0)
				std::cout<<"socket changed to non blocking"<<std::endl;
*/	
	int i;for(i=socketNum;i<socketNum+100;i++){
		memset(&serverT, 0, sizeof(serverT));       	
		serverT.sin_family = AF_INET;
		serverT.sin_addr.s_addr = htonl(INADDR_ANY);
		serverT.sin_port = htons(i);								
		if(bind(serversock,(struct sockaddr *)&serverT,sizeof(serverT)) < 0){
			std::cout<<"unable to bind server socket:"<<std::dec<<i<<std::endl;
		}else{
			std::cout<<"serversock bound:"<<std::dec<<i<<std::endl;
			break;
		}
	}
	const size_t sn=0x40;char s[sn];
	assert0(listen(serversock, MAXPENDING) >= 0
		,"Failed to listen on server socket"
		,(snprintf(s,sn,"Listening on server socket:%i",i),s)
	);

	System("sleep 0.1;putty -load ",i,"&");	

	while(!abandon){
		int clientsocket;
		if( (clientsocket = accept(serversock, (struct sockaddr *) &clientT,&sizeof_clientT))>=0 ){
			fcntl_add_flag(clientsocket,O_NONBLOCK);
			std::cout<<"Telnet connection accepted";
			newThread(clientsocket,dup(clientsocket));
		}
		wait();
		removeEndedThreads();
	}
	while(threads.size())
		removeEndedThreads();
}


