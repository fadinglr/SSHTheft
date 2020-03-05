#include "./inject.hpp"

int main(int argc, char *argv[])
{
	pid_t pid =  atol(argv[1]);
	char * origfunc = argv[2];
	char * hookfunc = argv[3];
	char * libPath = argv[4];
	std::cout<<"So Injection: "<<libPath <<" into pid: "<< pid<<std::endl;
	void * dlopen=find_symbol(pid,"__libc_dlopen_mode",LIBCNAME);
	load_so(pid, dlopen, libPath);
	gothook(pid, origfunc, hookfunc,libPath);
}


