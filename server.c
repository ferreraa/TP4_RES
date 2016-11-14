#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>



#define SERVICE_DEFAUT "1111"
#define PROTOCOLE_DEFAUT "tcp"
#define AUCUNE_ADR_INET 0xFFFFFFFF
#define DEFPORTBASE 	0


int sock, sock_initiale; //ports des sockets qui seront utilisées
struct sockaddr_in p_adr_server;










void server_test_communication()
{
	char tampon[10];
	printf("valeur de read : %d\n",read(sock,tampon,1));
	printf("tampon[0] : %s \n", tampon);

}


void adr_socket ( char *service, char *serveur, char *protocole,
		  struct sockaddr_in *p_adr_serv)
{
	struct hostent *p_info_serveur ;  /* pointeur sur infos serveur         */
	struct servent *p_info_service ;  /* pointeur sur infos service	distant	*/
	char str[INET_ADDRSTRLEN];

	
#ifdef DEBUG
	printf("\n%s ADR_SOCKET (debut) .........\n",aff_debug);
#endif


	/* RENSEIGNEMENT ADRESSES SOCKET  ------------------------------*/

	/* Mise a zero de la structure d'adresse socket recherchee	*/
	 memset( (char *) p_adr_serv, 0, sizeof(*p_adr_serv) );

	/* Definition du domaine ( famille ) 	*/
	p_adr_serv->sin_family = AF_INET;


	/* ------ RENSEIGNE @IP -----------------------------------------*/
        if (serveur==INADDR_ANY) /* Cas d'un serveur */
				p_adr_serv->sin_addr.s_addr =htonl( INADDR_ANY);
		else
            {
            p_info_serveur = gethostbyname( serveur );
            if (p_info_serveur)	/* Serveur reference dans /etc/hosts ou DNS */
		{	
			memcpy ( (void *)&p_adr_serv->sin_addr, 
				 p_info_serveur->h_addr,p_info_serveur->h_length );
			/* PS: ici pas besoin de htonl*/
		}			  
            else
		
		{
			/* Adresse IP sous forme de chaine a transcrire */
			/* Modif PS 2011: inet addr obsolete ramplace par inet_pton*/
			/* p_adr_serv->sin_addr.s_addr = inet_addr(serveur) ;*/
			inet_pton(AF_INET,serveur,&(p_adr_serv->sin_addr));

			if (p_adr_serv->sin_addr.s_addr == AUCUNE_ADR_INET )
		  		printf ("\nERREUR 'adresse' : adresse  %s incorrecte \n", serveur );
                
		}
            }

	
#ifdef DEBUG
	/* PS 2011: Inet_ntoa obsolete */
	inet_ntop(AF_INET, &(p_adr_serv->sin_addr),str,INET_ADDRSTRLEN);
	printf("%s adresse IP : %s OK..........\n",aff_debug,str);

#endif


	/* --------- RENSEIGNE PORT --------------------------------------------*/
	
	p_info_service = getservbyname(service,protocole);

	if ( p_info_service ) 	       /* Service reference dans /etc/service */
	{
	p_adr_serv->sin_port = htons ( ntohs(p_info_service->s_port) 
							+ DEFPORTBASE );
	#ifdef DEBUG
	printf("%s port : %d associe au service %s OK .........\n", aff_debug, ntohs(p_adr_serv->sin_port), service); /*Modif PS */
	#endif

	}
	else 	
	/* Service repere par un numero ( chaine ) il faut en deduire le numero de port */
	{
		p_adr_serv->sin_port = htons( (u_short)atoi(service) );
	/* Modif P. Sicard port a 0 on remplit avec 0 , il sera affecte
	dynamiquement au niveau du bind */
	/*	if ( p_adr_serv->sin_port == 0 )
		  sortie_err("\nERREUR 'adr_socket' : service %s introuvable \n ",
 					service ); */
	}

#ifdef DEBUG
	printf("%s port : %d OK .........\n", aff_debug, ntohs(p_adr_serv->sin_port)); /*Modif PS */
	printf("%s ADR_SOCKET (fin) .........\n",aff_debug); 
#endif
}











int main(int argc, char const *argv[])
{

	char* service = SERVICE_DEFAUT;
	char* commande = "";

	sock_initiale = socket(PF_UNIX, SOCK_STREAM, IPPROTO_TCP);
	if(sock_initiale < 0) {printf("Probleme dans socket\n");}

	adr_socket(SERVICE_DEFAUT,INADDR_ANY,"tcp", &p_adr_server);


	if(bind( sock_initiale, (struct sockaddr *) &p_adr_server, sizeof(struct sockaddr_in) ) < 0) {printf("Probleme dans bind\n");}

	if(!listen(sock_initiale, 10)) {
		printf("Probleme dans listen\n");
	}

	sock = accept(sock_initiale, &p_adr_server, sizeof( struct sockaddr ));

	server_test_communication(service, "tcp");

	close(sock);

	return 0;
}


