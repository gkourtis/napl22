#include "includes.cpp"

int main(void){
	std::cout<<"main started mmStart"<<&M<<std::endl;
	assert0(chdir("/home/gk/Dropbox/workspace01/napl22")==0,"unable to change dir");
	stopETs.store(false,0);
	threads.reserve(0x100);

	int fin,i;
	if(	(fin=open("naplStart0.napl",0,S_IRUSR))>=0)
		newThread(fin,dup(fileno(stdout)));

	HandleIncomingConnections(8000); 

	return 0;
}
