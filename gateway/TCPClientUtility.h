#ifndef  __TCP__
#define  __TCP__
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
int SetupTCPClientSocket(const char *host, const char *service);
#endif
