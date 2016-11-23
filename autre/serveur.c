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

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define PROTOCOLE_DEFAUT "tcp"

void serveur_appli (char *service, char* protocole);   /* programme serveur */

struct sockaddr_in p_adr_socket;
int sock_initiale,sock; //port des sockets qui seront utilisées
/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

main(int argc,char *argv[])
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


//met à jour le mot connu par le client en fonction de sa proposition
void maj(char *mot, char *mot_cache,char *caract)
{

  int i=0;
  while(mot[i]!='\0')
  {
	if(mot[i]==*caract) mot_cache[i]=*caract;
	i++;
  }

}

/******************************************************************************/	
void serveur_appli(char *service, char *protocole)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
  int i; //indice
  int lg_mot,decouverte=0;	//longueur du mot à découvrir et nombre de lettres découvertes par le client
  char mot[50],mot_cache[50]; //le mot secret et le mot transmis au client
  char tempo[2];//longueur du mot exprimée sous forme d'une chaine de caractères.   Puis caractère envoyé par le client. 

  printf("Bienvenue dans le jeu du pendu, quel mot souhaitez vous faire deviner ?\n");
  scanf("%s",mot);
  lg_mot=strlen(mot); // On a saisi le mot à deviner et on en a déduit sa longueur

//On envoie le mot mystère sous forme d'une suite de tirets
  for(i=0;i<lg_mot;i++) mot_cache[i]='-';
  mot_cache[i]='\0';

  sprintf(tempo,"%d",lg_mot);
//On indique à tempo la longueur du mot à deviner, pour qu'il l'envoie sous forme de chaine de caractères au client


  if(lg_mot<10) //si la longueur du mot se code sur 1 caractères, on place un 0 sur le premier. Ainsi, le client lira toujours 2 caractères, sans avoir à se soucier si la longueur est > ou < à 10
  {
    tempo[1]=tempo[0];
    tempo[0]='0';
  }


  h_writes(sock,tempo,strlen(tempo)); //On envoie au client la longueur du mot


  while(decouverte<lg_mot) //tant que le mot n'a pas été découvert
  {

	h_reads(sock,tempo,1);	//On lit la proposition du client
	maj(mot,mot_cache,tempo);//On met à jour le mot connu par le client
	h_writes(sock,mot_cache,lg_mot);//On envoie le nouveau mot au client (après sa maj)
  }

}

/******************************************************************************/	

