#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

void error_handling(const char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
	char *addr = "127.232.124.79";
	struct sockaddr_in addr_inet;

	if(!inet_aton(addr, &addr_inet.sin_addr))
		error_handling("conversion error");
	else
		printf("network ordered integer addr : %#x \n",
			 addr_inet.sin_addr.s_addr);

	return 0;
}
