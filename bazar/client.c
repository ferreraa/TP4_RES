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

#include <stdlib.h>


int sock; //définit la socket pour la connexion au serveur



void client_appli (char *serveur, char *service, char *protocole);


/***********************************************************
/* envoyer une chaine de caractere avec sa longueur */
/************************************************************/
void envoie (char * c){
	int l;
	char temp [2];
	l= strlen(c);
	sprintf(temp,"%d",l);
	if (l<10){
		temp[1]=temp[0];
		temp[0]='0';
	}
	h_writes(sock,temp, 2);
	h_writes(sock,c,l);
	c[l]='\0';
	printf ("J'ai envoye le message %s de longueur %i\n", c,l);
}
				
void recoie(char * c){
	char char_l[2];
	int l;
	// on recupere la longueur de la reponse 
	h_reads(sock, char_l, 2);
	l= atoi(char_l);
	// on recupere ensuite la reponse
	h_reads(sock,c,l);
	c[l]='\0';		//On s'assure que le mot reçu soit bien utilisable comme une chaine de caractères
	printf("\nReponse : %s\n",c);
}

/***************************************************************/


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

void recuperer_nom_fichier (char *nom_fichier,char * commande, char *cmd){
	int i=0;
	int j=0;
	for (i=0; i<3; i++){
		if (commande[i]!=cmd[i]) printf(" il y a un probleme ...\n");
	}
	while (commande[i]==' '){i++;}
	while (commande [i]!= ' ' && commande[i]!='\0'){
		nom_fichier[j]=commande[i];
		i++;
		j++;
	}
}

/*****************************************************************************/
void client_appli (char *serveur,char *service,char *protocole)

/* procedure correspondant au traitement du client de votre application */

{
 
	char tempo2 [2];
  int lg_mot;
  char mot[50];
  int decouvert = 0; //nombre de lettres découvertes sur le mot
  int tempo;
  char reponse[200];
  char  longueur [100];
  int lg_reponse;
char commande [20];
  printf("Bienvenue\n");
	int prout=1;
	char lettre;
	char nom_fichier[20];
  
while (prout ==1){
	printf("\n->");
	scanf("%s",commande);

	switch (commande[0]){
		case 'l': printf("ls");lettre='l';
			break;
		case 'p': {
			printf("put ");
			lettre ='p';
			scanf("%s", nom_fichier);
			printf("\nNom du fichier : %s\n", nom_fichier);}//recuperer le nom du fichier
			break;
		case 'g': {
			printf("get "); 
			lettre='g';
			scanf("%s", nom_fichier);
			printf("\nNom du fichier : %s\n", nom_fichier);}//recuperer le nom du fichier
			break;
		case 'q': {printf("Au revoir\n"); prout = 0;lettre='q';}
			break;
		default : printf("aretter de faire n'importe quoi ! \n");
	}
	h_writes(sock,&lettre,1);
  

  
	viderbuffer();
	
	lg_reponse=2;
	if (prout==1){
		if (lettre=='p' || lettre =='g'){
			envoie (nom_fichier);
		}
		recoie(reponse);
		

	}
}	

}



/*****************************************************************************/
