#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <netinet/in.h>
#include <netdb.h>

void adr_socket( char *service, char *serveur, char *protocole, 
		 struct sockaddr_in *p_adr_serv);
