/******************************************************************************/
/*			Application: Le pendu			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  Alexandre Ferrera et Boris Odièvre		      */
/*		Date :  2016						      */
/*									      */
/******************************************************************************/	

#include<stdio.h>
#include <curses.h>
#include<string.h>

#include<sys/signal.h>
#include<sys/wait.h>
#include <stdlib.h>

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define PROTOCOLE_DEFAUT "tcp"

void serveur_appli (char *service, char* protocole);   /* programme serveur */

struct sockaddr_in p_adr_socket;
int sock_initiale,sock; //port des sockets qui seront utilisées
/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

 int main(int argc,char *argv[])
{

	char *service= SERVICE_DEFAUT; /* numero de service par defaut */
	char *protocole= PROTOCOLE_DEFAUT; /* protocole par defaut */




	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  printf("defaut protocole = %s\n", protocole);
		  break;
 	case 2:
		  service=argv[1];
		  printf("defaut protocole = %s\n", protocole);
		  break;
 	case 3 :
		  service=argv[1];
		  protocole=argv[2];
		  break;

   	default :
		  printf("Usage:serveur service (nom ou port) protocole (TCP ou UDP)\n");
		  exit(1);
 	}

	/* service est le service (ou numŽro de port) auquel sera affectŽ
	ce serveur*/
	/* protocole est le protocole qui sera utilisŽ */
	

	sock_initiale = h_socket(AF_INET,SOCK_STREAM);
//nous utilisons SOCK_STREAM car cela correspond au protocole tcp, et AF_INET pour indiquer l'utilisation du protocole ethernet

	adr_socket(service, INADDR_ANY, protocole, &p_adr_socket); //renseignement des adresses distantes.

	h_bind(sock_initiale,&p_adr_socket); 	//association entre la socket et son adresse

	
	h_listen(sock_initiale,1);   //attente d'un client

	sock=h_accept(sock_initiale,&p_adr_socket);	//récupération de la socket client


	serveur_appli(service, protocole); //lancement de l'application


	h_close(sock);//fermeture de la connexion
}



/******************************************************************************/	
void serveur_appli(char *service, char *protocole)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
	int b=1;
  char tempo[2];//longueur du mot exprimée sous forme d'une chaine de caractères.   Puis caractère envoyé par le client. 
char reponse[20];
int lg_reponse;
  printf("Bienvenue \n");
	while (b==1){

  
		h_reads(sock,tempo,1); // on lit la commande 
		printf("%s\n entre la reponse :",tempo);
		
		scanf("%s",reponse);
		
		lg_reponse= strlen(reponse);
		printf(" La Longueur est :%d\n",lg_reponse);
		sprintf(tempo,"%d",lg_reponse);
		if (lg_reponse<10){
			tempo[1]=tempo[0];
			tempo[0]='0';
		}
		printf ("On envoie %s \n",tempo);
		
		h_writes(sock,tempo, 2);
		printf(" On a fini d'envoyer\n");
		
		printf("On envoie la reponse\n");
		h_writes(sock,reponse,6);//On envoie la reponse
		printf ("on as fini d'ecrire la reponse\n");
		if (tempo[0]=='q') {
			b=0;
			printf("Au revoir\n");
		}
	}
  }



/******************************************************************************/	

