/******************************************************************************/
/*			Application: Le Pendu				      */
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : Alexandre Ferrera & Boris Odièvre		      */
/*									      */
/******************************************************************************/	


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'Žcran */
#include <sys/signal.h>
#include <sys/wait.h>
#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define PROTOCOLE_DEFAUT "tcp"
#define SERVEUR_DEFAUT "localhost"


int sock; //définit la socket pour la connexion au serveur



void client_appli (char *serveur, char *service, char *protocole);


/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */
	char *protocole= PROTOCOLE_DEFAUT; /* protocole par defaut */


struct sockaddr_in p_adr_serv; //on créé une sockaddr_in pour l'utilisation de adr_socket)



	

	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  printf("protocole par defaut: %s\n",protocole);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  printf("protocole par defaut: %s\n",protocole);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  printf("protocole par defaut: %s\n",protocole);
		  break;
  	case 4:			/* serveur, service, protocole renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  protocole=argv[3];
		  break;
	default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port)  protocole (TCP ou UDP)\n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	/* protocole le protocole qui sera utilise pour la communication */
	


	sock = h_socket(AF_INET,SOCK_STREAM); //Nous utilisons AF_INET pour utiliser le protocole ethernet
//De plus, nous utilisons SOCK_STREAM car cela correspond au protocole tcp.

	adr_socket(service, serveur, protocole, &p_adr_serv); //renseignement des adresses distantes.

	h_connect(sock,&p_adr_serv); //Demande de connexion au serveur



	client_appli(serveur,service,protocole); //On lance l'appli

	h_close(sock); //On ferme la connexion
	return 0;
}

//renvoie le nombre de tirets dans le mot
int analyse_nouveau_mot(char * mot)
{
  int nb_tirets=0; //nb de tirets restants dans le mot caché
  int i=0;
  while(mot[i]!='\0')
  {
	if(mot[i]=='-') nb_tirets++;
	i++;
  }
  return nb_tirets;
}


/*****************************************************************************/
void client_appli (char *serveur,char *service,char *protocole)

/* procedure correspondant au traitement du client de votre application */

{
  char tampon[50];
  int nb_essais; //définit le nombre d'essais autorisés à l'origine et le nbr d'essaie restant au cours du programme
  int lg_mot;
  char mot[50];
  int decouvert = 0; //nombre de lettres découvertes sur le mot
  int tempo;

  printf("Bienvenue dans le jeu du pendu. Veuillez choisir votre nombre de tentatives maximales :\n");
  scanf("%d",&nb_essais); //saisie de la difficulté du jeu

  h_reads(sock,tampon,2); //recoit la longueur du mot à deviner
  lg_mot = atoi(tampon); //on convertit la chaine de caractère en entier pour connaitre la longueur du mot


  printf("Le mot mystere fait %d lettres.\n",lg_mot);
	viderbuffer();					//On vide le buffer afin de ne pas influencer scanf (viderbuffer() a été définie dans fon.c)
  while(nb_essais>0 && decouvert<lg_mot )  //tant que le joueur n'a pas complété le mot et n'a pas utilisé toutes ses vies
  {
	char lettre_temp; //retiendra la lettre choisie par le joueur
	printf("Quelle lettre proposez vous ?\n");


	scanf("%c",&lettre_temp);

	viderbuffer(); //On vide le buffer après l'appel de scanf pour ne pas causer de problème avec les prochains appels de scanf


	h_writes(sock,&lettre_temp,1); //On envoie au serveur la lettre demandée par l'utilisateur
	h_reads(sock,mot,lg_mot);	//On récupère le mot modifié en fonction de la lettre proposée

	mot[lg_mot]='\0';		//On s'assure que le mot reçu soit bien utilisable comme une chaine de caractères

	tempo=lg_mot-analyse_nouveau_mot(mot);	//tempo récupère la valeur du nombre de lettres découvertes dans le mot reçu
	if(tempo==decouvert) nb_essais--;	//Si le nouveau nombre de lettres découvertes est identique à l'ancien, ça signifie que la lettre proposée n'y est pas présente
	else decouvert=tempo;			//sinon, decouvert prend la nouvelle valeur du nombre de lettres découvertes

	printf("\nMot mystere : %s\n",mot);


	printf("\nEssais restants : %d\n",nb_essais);

 }

	if(nb_essais>0) printf("\nFélicitations, vous etes victorieux !\n");
	else printf("Vous avez ete pendu...\n");
}
/*****************************************************************************/

