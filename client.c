#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h> 
#include <sys/socket.h>

#include "fon.h"

#include <stdarg.h>
#include <string.h>


#include <errno.h>
#include <string.h>

#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>
#include <arpa/inet.h>

#define AUCUNE_ADR_INET 0xFFFFFFFF
#define DEFPORTBASE 	0



void adr_socket ( char *service, char *serveur, char *protocole,
			  struct sockaddr_in *p_adr_serv)
	{
		struct hostent *p_info_serveur ;  /* pointeur sur infos serveur         */
		struct servent *p_info_service ;  /* pointeur sur infos service	distant	*/
		

		
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


int main (int argc, char *argv[]){
		// en parametre l'adresse et le port du serveur faire pb si il y sont pas 
		
		
		/*******************************
		// connexion au serveur 
		* ***************************/
		
		int sock = socket(PF_UNIX,SOCK_STREAM,IPPROTO_TCP);
			// PF_NIX a changer par AF_INET pour communication en reseaux
			// renvoie -1 si il y a un probleme 
		struct sockaddr_in sin;
		
		//argv[1]->port et argv[2]-> adresse
		adr_socket (argv[1], argv[2], "tcp",&sin);
	
		connect(sock, (struct sockaddr *) &sin , sizeof(struct sockaddr_in));
		
		//Elle retourne 1 si la connexion n’est pas possible 0 sinon.
		
		
		/************************************
		l'utilisateur entre les commande au clavier 
		************************************/
		char buffer[1024];
		buffer[0]='b';
		buffer[1]='o';
		buffer[2]='n';
		buffer[3]='j';
		buffer[4]='o';
		buffer[5]='u';
		buffer[6]='r';
		
		write(sock , buffer, 7);
		
		
		//a un moment fermer la sockets...
		close(sock);
}

/*les commande sont:
 * 	 	ls 
 * 		put nomfichier
 * 		get nom fichier
 * 		quit
 * 		help
 * 		sinon affichage message d'erreur
 */
 
 
 
 
 
 
