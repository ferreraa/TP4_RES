/*******************************************************************************/
/*				FON.C						*/
/********************************************************************************/
/*			Auteurs	: 						*/
/*				Pascal Sicard					*/
/*				Marc Garnier					*/
/*				Elisabeth Maillet				*/
/*				Christian Rabedaoro				*/
/* 			Date		Sept 94					*/
/*  modification correction bug	 aff_debug		Date		03/96	*/
/*  modification correction bug	par Cristophe bind :sizeof (p_adr_socket) */
/* et setsockopt dans h_socket*/

/* Modif P. Sicard 11 97:
	-probleme de l'allocation par le systeme en cas de numero de
port a 0 
	- probleme dans le bind sizeof (struct sockaddr) 
	- probleme de hton dans adr_sock */
/* Modif03/2005 P. Sicard : pb de libraire varargs obsolete (sous LINUX et MacOSX Darwin) */
/* remplacement par stdarg.h , voir sortie_err*/
/* Modif03/2006 P. Sicard : suppression de sortie_err, probleme sous free-BSD*/
/* Modif 05/2011 P. Sicard : suppression des inet_ntoa (seg. fault) obsolete remplace par inet_ntop*/

/********************************************************************************/
/*	Definition de fonctions sur les sockets par encapsulation des	   	*/
/*	 primitives UNIX.   							*/
/*	Il s'agit d'appels avec controles des parametres et des resultats.	*/
/********************************************************************************/
/*										*/
/*	h_socket  : Creer la prise de communication ou socket	( socket )	*/
/*	h_bind    : Associer une socket a ses adresses(@IP,port)( bind )	*/ 				 
/*	h_connect : Demander une connection (a un serveur. TCP) ( connect )	*/
/*	h_listen  : Mettre un serveur en ecoute sur une prise	( listen )	*/
/*	h_accept  : Accepter, cote serveur, une connection	( accept )	*/
/*	h_reads	  : Lire sur une socket < connecte >		( read )	*/
/*	h_writes   : Ecrire   "	  "     < connecte >		( write )	*/
/*	h_sendto  : Emettre  "    "	< Non connecte >	( sendto )	*/
/*	h_recvfrom: Recevoir "    " 	< Non connecte >	( recvfrom )  	*/
/*	h_shutdown: Gerer la fin de connection < connecte > 	( shutdown )  	*/
/*	h_close	  : Fermeture d'une socket			( close )	*/
/*										*/
/********************************************************************************/
/********************************************************************************/
/*	routine de renseignement des adresses socket du processus serveur       */
/* 										*/
/*	adr_socket: renseigne la structure d'adresses socket			*/ 
/********************************************************************************/

/* Les inclusions de fichiers standards ( entrees/sorties et sockets ) sont 
	effectuees dans le fichier de prototypage FON.H 			*/


#include <stdarg.h>
#include <string.h>
#include "fon.h"

/****************************	Definition des parametres **********************/
#define AUCUNE_ADR_INET 0xFFFFFFFF
#define DEFPORTBASE 	0

char aff_debug[120]=">>>>> DEBUG >>>>> \t";

/*			    +===================+			  	*/
/*==========================|	H_SOCKET	|===============================*/
/*			    +===================+				*/
/*										*/
/*	Cree une socket ou prise de communication				*/
/*										*/
/* ENTREE									*/
/*	domaine	: Ensemble de protocoles accessibles				*/
/*			AF_UNIX		<-> protocoles UNIX ( intra-machine )	*/
/*			AF_INET 	<-> protocoles Internet		 	*/
/*			AF_NS 		<-> protocoles Xerox NS 		*/
/*			AF_IMPLINK 	<-> protocoles IMP Link Layer	 	*/
/*										*/
/*	mode	: Mode de communication sur cette prise				*/
/*			SOCK_STREAM	<-> mode connecte ( TCP / AF_INET ) 	*/
/*						flots d'octets 			*/
/*			SOCK_DGRAM	<-> mode non connecte ( UDP / AF_INET)	*/
/*						datagrammes 			*/
/*			SOCK_RAW	<-> mode "brut" ( IP / AF_INET )	*/
/*						protocoles bas niveau		*/
/*			SOCK_SEQPACET	<-> mode sequence			*/
/*										*/
/*	type_protocole	:  Type de protocole					*/
/*				Pour differencier eventuellement les protocoles	*/
/*				d'un meme mode. Classiquement inutile car a  	*/
/*				chaque mode ne correspond qu'un seul protocole !*/
/*						INUTILE !!			*/
/*										*/
/* SORTIE	Numero identificateur de la socket				*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/
int h_socket ( int domaine, int mode )
{
	int res ;	/* Entier resultat de l'operation */
	int sendbuff;


#ifdef DEBUG
	printf("\n%s H_SOCKET (debut) -------------------\n",aff_debug); 
	printf("%s domaine : %d mode : %d \n",aff_debug,domaine,mode);
#endif

	if (domaine!=AF_INET) printf(" Sous internet utiliser -> AF_INET\n");
	if ( (mode!=SOCK_STREAM)&&(mode!=SOCK_DGRAM)) printf(" Mode a choisir parmi : SOCK_STREAM, SOCK_DGRAM \n");

	res = socket( domaine, mode, 0 ) ;

setsockopt(res, SOL_SOCKET, SO_REUSEADDR, (char *) &sendbuff, sizeof(sendbuff));

	if ( res < 0 ) printf ( "\nERREUR 'h_socket' : Creation socket impossible \n" );
#ifdef DEBUG
	printf("%s H_SOCKET (fin) ----- socket %d cree ---\n",aff_debug,res); 
#endif	
	return res;
}


/*			    +===================+				*/
/*==========================|	H_BIND		|===============================*/
/*			    +===================+				*/
/*										*/	
/*  Associe une socket (numero de socket) et ses adresses (@IP, port)		*/
/*										*/
/*  En mode connecte ( TCP ) 							*/
/*  Procedure reservee au serveur ( le socket client sera automatiquement lie 	*/ 
/*   lors de l'appel a h_connect c.a.d. connect )				*/
/*										*/
/*  En mode non connecte ( UDP ) 						*/
/*  Procedure utilisee par le serveur, et par le client s'il envisage ensuite 	*/
/*	de converser avec les procedures read et write ( comme en connecte )	*/ 
/*										*/
/* ENTREE									*/
/*	num_soc	: 	Numero identifiant la socket				*/
/*										*/
/*	p_adr_socket :	pointeur sur les adresses socket			*/
/*										*/
/* SORTIE								  	*/
/*	     	aucune ( le rapport d'execution est envoye sur la console )	*/
/********************************************************************************/
void h_bind ( int num_soc, struct sockaddr_in *p_adr_socket )
{

	int res;	/* Entier resultat de l'operation 	*/
	struct sockaddr_in s,s1;
	int lensa;
	char str[INET_ADDRSTRLEN];


#ifdef DEBUG
	printf("\n%sH_BIND (debut)\n",aff_debug);	
	printf ("%ssocket : %d \n",aff_debug,num_soc);
	
	/* PS2011: Inet_ntoa obsolete-printf("%sadresse IP : %s \n",aff_debug,inet_ntoa(p_adr_socket->sin_addr));*/
	inet_ntop(AF_INET, &(p_adr_socket->sin_addr),str,INET_ADDRSTRLEN);
	printf("%sadresse IP : %s \n",aff_debug,str);
	
	printf("%sport : %d OK \n", aff_debug, ntohs(p_adr_socket->sin_port));
#endif
	
	res=bind( num_soc, (struct sockaddr *)p_adr_socket, sizeof(struct
	sockaddr_in) ); /* Modif PS */


#ifdef DEBUG
/*bzero (&s,sizeof(s));*/
lensa=sizeof(struct sockaddr_in);
if	(!getsockname (num_soc, (struct sockaddr *) &s, &lensa))
	{
	printf("%sport apres le bind: %d OK \n", aff_debug, ntohs(s.sin_port));
	}
	else
	printf ("Probleme getsockname\n");
#endif

	if ( res < 0 ) printf( "\nERREUR 'h_bind' : liaison socket %d impossible\n",num_soc);



#ifdef DEBUG
	printf("%sH_BIND (fin)  \n",aff_debug); 
#endif
}



/*			    +===================+				*/
/*==========================| 	H_CONNECT	|===============================*/
/*			    +===================+				*/
/*										*/
/*      Cree une connection entre 2 sockets en mode connecte			*/
/*	Procedure reservee au client en mode connecte				*/
/*										*/
/* ENTREE									*/
/*		num_soc	: 	Numero identifiant la socket			*/
/*										*/
/*		p_adr_serv :	adresses socket d'acces au service		*/												/*										*/
/* SORTIE									*/
/*	     	aucune ( le rapport d'execution est envoye sur la console )	*/
/********************************************************************************/
void h_connect( int num_soc, struct sockaddr_in *p_adr_serv)
{
	
	int res;		/* Entier resultat de l'operation 	*/
	char str[INET_ADDRSTRLEN];


#ifdef DEBUG
	printf("\n%sH_CONNECT (debut) -------------------\n",aff_debug); 
	printf ("%ssocket : %d \n",aff_debug,num_soc);
	/* PS 2011: Inet_ntoa obsolete */
	inet_ntop(AF_INET, &(p_adr_serv->sin_addr),str,INET_ADDRSTRLEN);
	printf("%sadresse IP serveur: %s \n",aff_debug,str);
	
	printf("%sport service : %d OK \n", aff_debug, ntohs(p_adr_serv->sin_port));
#endif
	
	res = connect ( num_soc, (struct sockaddr *)p_adr_serv, sizeof(struct sockaddr) );
	
	if ( res < 0 ) {
			inet_ntop(AF_INET, &(p_adr_serv->sin_addr),str,INET_ADDRSTRLEN);
			printf( "\nERREUR 'h_connect' : connexion serveur %s impossible\n"
				   ,str);}

#ifdef DEBUG
	printf("%sH_CONNECT (fin) -------------------\n",aff_debug); 	
#endif

}



/*			    +===================+				*/
/*==========================|  H_LISTEN  	|===============================*/
/*			    +===================+				*/
/*										*/
/*	Met la socket en 2tat d'attente de demandes de connections		*/
/*	Procedure reservee au serveur en mode connecte				*/
/*										*/
/* ENTREE									*/
/*	num_soc	: Numero identifiant la socket					*/
/*										*/
/*	nb_req_att :	Nombre maximum de requetes dans la file d'attente 	*/
/*										*/
/* SORTIE									*/
/*	aucune ( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/
void h_listen ( int num_soc, int nb_req_att )
{

	int res;		/* Entier resultat de l'operation 	*/



#ifdef DEBUG
	printf("\n%sH_LISTEN (debut) -------------------\n",aff_debug); 
#endif

	res = listen( num_soc, nb_req_att );
	if ( res < 0 ) 
	  	printf("\nERREUR 'h_listen' : ecoute sur socket %d impossible \n",num_soc );

#ifdef DEBUG
	printf("%sH_LISTEN (fin)\n",aff_debug); 
#endif
}



/*			    +===================+				*/
/*==========================| H_ACCEPT 		|===============================*/
/*			    +===================+				*/
/*										*/
/*	Accepte une demande de connection (communication en mode connecte)	*/
/*	Procedure reservee au serveur en mode connecte				*/
/*										*/
/* ENTREE									*/
/*	num_soc	: Numero identifiant la socket du serveur			*/
/*										*/
/*	p_adr_client	: Pointeur sur les infos adresse client		  	*/
/*										*/
/* SORTIE	identificateur de la nouvelle socket cree			*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/
int h_accept( int num_soc, struct sockaddr_in *p_adr_client )
{

	int longueur ;
	int res;			/* Entier resultat de l'operation 	*/

#ifdef DEBUG
	printf("\n%sH_ACCEPT (debut)\n",aff_debug); 
#endif

	longueur = sizeof( struct sockaddr );
	res = accept ( num_soc, (struct sockaddr *)p_adr_client, &longueur );
	if ( res < 0 ) 
	  printf ( "\nERREUR 'h_accept' : Acceptation impossible \n" );

#ifdef DEBUG
	printf("%sH_ACCEPT (fin)\n",aff_debug);
#endif

	
	return res ;
	
}



/*			    +===================+				*/
/*==========================| 	H_READS		|===============================*/
/*			    +===================+				*/
/*										*/
/*	lit les donnees d'une socket en mode connecte				*/
/*										*/
/* ENTREE									*/
/*		num_soc	: Numero identifiant la socket 				*/
/*										*/
/*		tampon	: Pointeur sur le tampon (buffer) de donnees.		*/
/*										*/
/*		nb_octets : Nombre d'octets du tampon.				*/
/*										*/
/* SORTIE	Nombre d'octets effectivement lus ( 0 si fin de socket )	*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/

int h_reads ( int num_soc, char *tampon, int nb_octets )
{
	int nb_restant, nb_lus;


#ifdef DEBUG
	printf("\n%sH_READS (debut) 	-------------------\n",aff_debug);
	printf("%sH_READS nb_octets : %d \n",aff_debug, nb_octets); 
#endif


	nb_restant=nb_octets ;
	while ( nb_restant > 0 )
	{
		nb_lus = read ( num_soc, tampon, nb_restant );


#ifdef DEBUG
	printf("%sH_READS apres read ..................\n",aff_debug);
	printf("%sH_READS nb octets lus: %d \n",aff_debug, nb_lus);
#endif


		if ( nb_lus<0 )
		{
	  		printf( "\nERREUR 'Lire_mes' : lecture socket %d impossible\n", num_soc );
			return (nb_lus);
		}

		nb_restant = nb_restant - nb_lus;

		/*if (nb_lus==0 || tampon[nb_lus]=='\0')*/		/* fin transfert */
		if (nb_lus==0)
		{
#ifdef DEBUG
	printf("%sH_READS (fin) 	-------------------\n",aff_debug); 
#endif
			return (nb_octets-nb_restant);
		}
		tampon = tampon + nb_lus;

	}


#ifdef DEBUG
	printf("%sH_READS (fin) 	-------------------\n",aff_debug); 
#endif

	return (nb_octets-nb_restant);
}


/*			    +===================+				*/
/*==========================|	H_WRITES	|===============================*/
/*			    +===================+				*/
/*										*/
/*	Ecrit 'dans' une socket en mode connecte				*/
/*										*/
/* ENTREE									*/
/*		num_soc	: Numero identifiant la socket 				*/
/*										*/
/*		tampon	: Pointeur sur le tampon (buffer) de donnees.		*/
/*										*/
/*		nb_octets : Nombre d'octets du tampon.				*/
/*										*/
/*										*/
/* SORTIE	Nombre d'octets effectivement ecrits ( 0 si fin de socket )	*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/

int h_writes ( int num_soc, char *tampon, int nb_octets )
{
	int nb_restant, nb_ecrits;	/* nb octets restant a ecrire, nb octets ecrits */



#ifdef DEBUG
	printf("\n%sH_WRITES (debut) 	-------------------\n",aff_debug); 
#endif


	nb_restant=nb_octets ;
	while ( nb_restant > 0 )
	{

		nb_ecrits = write ( num_soc, tampon, nb_restant );

		if ( nb_ecrits<0 ) 
		{
				printf( "\nERREUR 'Ecrire_mes' : ecriture socket %d impossible\n", num_soc );
				return (nb_ecrits);
		}	  

		nb_restant = nb_restant - nb_ecrits;
		tampon = tampon + nb_ecrits;
	}

#ifdef DEBUG
	printf("%sH_WRITES (fin) 	-------------------\n",aff_debug); 
#endif


	return (nb_octets-nb_restant);
}


/*			    +===================+				*/
/*==========================|  	H_SENDTO	|===============================*/
/*			    +===================+				*/
/* ENTREE									*/
/*		num_soc	: Numero identifiant la socket 				*/
/*										*/
/*		tampon	: Pointeur sur le tampon (buffer) de donnees.		*/
/*										*/
/*		nb_octets : Nombre d'octets du tampon.				*/
/*										*/
/*		p_adr_distant :	Pointeur sur les infos adresse interlocuteur. 	*/
/*										*/
/*	SORTIE	Nombre d'octets effectivement emis 				*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/
int h_sendto( int num_soc, char * tampon, int nb_octets,
						struct sockaddr_in *p_adr_distant )
{
	int res;		/* Entier resultat de l'operation 	*/




#ifdef DEBUG
	printf("\n%sH_SENDTO (debut)\n",aff_debug); 
#endif


	res = sendto( num_soc, tampon, nb_octets, 0, (struct sockaddr *) p_adr_distant,
					 	sizeof(*p_adr_distant) );
	if ( res < 0 ) 
	  printf( "\nERREUR 'h_sendto' : emission socket %d impossible\n",
	 							num_soc );

#ifdef DEBUG
	printf("%sH_SENDTO (fin)\n",aff_debug); 
#endif


	return res;

}

/*			    +===================+				*/
/*==========================|  H_RECVFROM	|===============================*/
/*			    +===================+				*/
/* ENTREE									*/
/*		num_soc	: Numero identifiant la socket 				*/
/*										*/
/*		tampon	: Pointeur sur le tampon (buffer) de donnees.		*/
/*										*/
/*		nb_octets : Nombre d'octets du tampon.				*/
/*										*/
/*		p_adr_distant :	Pointeur sur les infos adresse interlocuteur. 	*/
/*										*/
/*	SORTIE	Nombre d'octets effectivement recus 				*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/
int h_recvfrom( int num_soc, char * tampon, int nb_octets, struct sockaddr_in *p_adr_distant )
{
	int res;		/* Entier resultat de l'operation 	*/
	int taille;




#ifdef DEBUG
	printf("\n%sH_RECVFROM (debut)\n",aff_debug); 
#endif


	taille = sizeof(*p_adr_distant);
	res = recvfrom( num_soc, tampon, nb_octets, 0, (struct sockaddr *) p_adr_distant, &taille );
	if ( res < 0 ) 
	  printf( "\nERREUR 'h_recvfrom' : reception socket %d impossible\n",
								 num_soc );


#ifdef DEBUG
	printf("%sH_RECVFROM (fin)\n",aff_debug); 
#endif

	return res;

}

/*			    +===================+				*/
/*==========================| H_SEND		|===============================*/
/*			    +===================+				*/

/*			    +===================+				*/
/*==========================| H_RECV		|===============================*/
/*			    +===================+				*/

/*			    +===================+				*/
/*==========================| 	H_SHUTDOWN	|===============================*/
/*			    +===================+				*/

/* ENTREE									*/
/*	num_soc	: Numero identifiant la socket 					*/
/*	sens	: sens de fermeture (FIN_RECEPTION, FIN_EMISSION, FIN_ECHANGES)
*/
/*										*/
/* SORTIE	Aucune								*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/
void h_shutdown ( int num_soc, int sens )
{
	int res;		/* Entier resultat de l'operation 	*/




#ifdef DEBUG
	printf("\n%s H_SHUTDOWN (debut)\n",aff_debug); 
#endif

	res = shutdown ( num_soc, sens );
	if ( res < 0 ) 
	  printf( "\nERREUR 'h_shutdown' : fermeture socket %d impossible\n",
								 num_soc );

#ifdef DEBUG
	printf("%s H_SHUTDOWN (fin)\n",aff_debug); 
#endif

}

/*			    +===================+				*/
/*==========================|	H_CLOSE		|===============================*/
/*			    +===================+				*/
/* ENTREE									*/
/*	num_soc	: Numero identifiant la socket 					*/
/*										*/
/* SORTIE	Aucune								*/
/*	     	( le rapport d'execution est envoye sur la console )		*/
/********************************************************************************/
void h_close ( int num_soc )
{
	int res;		/* Entier resultat de l'operation 	*/




#ifdef DEBUG
	printf("\n%s H_CLOSE (debut)\n",aff_debug); 
#endif

	res = close ( num_soc );
	if ( res < 0 ) 
	  printf( "\nERREUR 'h_close' : fermeture socket %d impossible\n",
								 num_soc );

#ifdef DEBUG
	printf("%s H_CLOSE (fin)\n",aff_debug); 
#endif

}


/*			    +===================+				*/
/*==========================|	ADR_SOCKET	 |==============================*/
/*			    +===================+				*/
/*										*/
/* ENTREE									*/
/*		service	:	Chaine designant le service applicatif		*/
/*				  Nom ou numero du service.			*/
/*				  La chaine doit etre transcriptible en entier  */
/*				   32 bits  					*/
/*										*/
/*		serveur :	Nom de la machine ou s'execute le service	*/
/*										*/
/*		protocole :	nom du protocole				*/
/*										*/
/*										*/
/*										*/
/* SORTIE									*/
/* par effet de bord affectation de la structure de type sockaddr_in pointe
par adr_serv */

/* La structure sockaddr_in est la suivante: 				*/
/* struct sockaddr_in {                      				*/
/*       short sin_family;                   				*/
		/*domaine d'utilisation (AF_INET pour TCP-UDP) 		*/
/*       ushort sin_port;                     				*/
		/* numero de port sur 16 bits 				*/
/*       struct in_addr sin_addr;            				*/
		/* adresse IP declare u_long sur certains systemes 	*/
/*       char sin_zero [8]						*/
		/* inutilise : mis a zero 				*/
/*		      }  						*/
/*										*/
/*	     	 ( le rapport d'execution est envoye sur la console )	*/
/********************************************************************************/


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


// vider le buffer : 
void viderbuffer()
{
	int c;
	while((c=getchar()) != EOF && c!='\n');
}
