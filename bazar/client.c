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
void get(char * nom_fic);
void put(char *nom_fic);
void ls();

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
 
	char commande [105];
	int prout=1;
	char nom_fichier[100];

  printf("Bienvenue\n");

  
	while (prout ==1){
		printf("\n->");
		scanf("%s",commande);

		
		if(strcmp(commande, "ls")==0)
		{
			ls();
		}
		else if (strncmp(commande, "put",3)==0)
		{
			scanf("%s",nom_fichier);
			printf("nom_fichier = %s\n", nom_fichier);
			put(nom_fichier);
		}
		else if (strncmp(commande, "get",3)==0)
		{
			scanf("%s",nom_fichier);
			printf("nom_fichier = %s\n", nom_fichier);
			get(nom_fichier);
		}
		else if (strcmp(commande,"q")==0)
		{
			printf("Au revoir\n");
			prout = 0;
			h_writes(sock,"q",1);
		}
		else {
			printf("Commande : %s non reconnue !\n", commande);
			printf("strcmp put  = %d\n",strncmp(commande, "put ",4));
			printf("strcmp get  = %d\n",strncmp(commande, "get ",4));
	  }
	  
	  
		viderbuffer();




	}
}	




void ls()
{
	char reponse [100];
	h_writes(sock,"l",1);

	recoie(reponse); //à modifier pour les grandes réponses

	printf("%s",reponse);
}


//créé un fichier nom_fic et y écrit les messages envoyés par le serveur.
void get(char * nom_fic)
{
	h_writes(sock,"g",1); //on indique qu'on veut faire un get.
	
	envoie(nom_fic); //on envoie le nom du fichier recherché
	
	
	char fexiste;
	h_reads(sock,&fexiste,1);

	if(fexiste == 'Y')
	{
		printf("Je commence le get\n");
		
		char tampon[100]; //taille 100 car recoie met un \0 à tampon[99]
		int nb_lus; //resultat du read
		
		
		FILE * fp = fopen(nom_fic, "w");
		do
		{
			recoie(tampon);
			nb_lus=strlen (tampon);
			fwrite(tampon, sizeof(char), nb_lus, fp);
		}while(nb_lus==99);
		
		fclose(fp);
	}
	else {
		printf("Ce fichier n'existe pas !\n");
	}
}
	

//la lecture peut surement être optimisée.
void put(char *nom_fic)
{
	
	h_writes(sock,"p",1);
	
	envoie(nom_fic);
	
	printf("Je commence le put !\n");
	FILE * fp;
	if(fp = fopen(nom_fic, "r")) //vérifie que le fichier existe
	{

		int nb_lus; //nb d'octets lus dans le fichier
		char buffer [100];
		
		do
		{
			nb_lus = fread(buffer,1,99,fp);
			buffer[nb_lus] = '\0';
			printf("Je vais ecrire %d octets !\n", nb_lus);
			envoie(buffer);
		}while(nb_lus == 99);
		fclose(fp);
	}
	else
	{
		printf("ce fichier n'existe pas !\n");
	}
	
}

	
	
	
/*****************************************************************************/



