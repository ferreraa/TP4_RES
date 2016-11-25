
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

#include <unistd.h>


#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define PROTOCOLE_DEFAUT "tcp"

void serveur_appli (char *service, char* protocole);   /* programme serveur */
void get(char *nom_fic); /* procedure qui envoie le contenu du fichier nom_fic au client*/
void put(char *nom_fic); /*procedure qui créé le fichier nom_fic */
void ls();

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


/***********************************************************
/* envoyer une chaine de caractere avec sa longueur */
/************************************************************/
void envoie (char * c, int l){
	
	char temp [2];
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


/******************************************************************************/	
void serveur_appli(char *service, char *protocole)

/* Procedure correspondant au traitemnt du serveur de votre application */

{
	int b=1;
	char lettre[2] = " ";
	char nom_fic [100];

	printf("Bienvenue \n");
	while (b==1){

  

		h_reads(sock,lettre,1); // on lit la commande 
		
		printf("%s\n",lettre);
		
		if (lettre[0]=='q') {
			b=0;
			printf("Au revoir\n");
		}
		else if (lettre[0]=='l'){ // cas du ls ******************************
			ls();
		}		
		else if(lettre[0] == 'g'|| lettre[0] == 'p')
		{
			recoie(nom_fic);
			printf("nom_fic = %s\n",nom_fic);
			if (lettre[0] == 'g')
			{
				get(nom_fic);
			}
			else put(nom_fic);
				
		}
			}
		}



void ls()
{
	int lg_reponse;
	char reponse[100];
	char buf;
	
	FILE* fp = popen("ls *", "r");
	printf("J'ai fait le ls\n");
	int fd=fileno(fp);
	
	lg_reponse=0;
	while (read(fd,&buf,1)==1){
		reponse[lg_reponse]=buf;
		lg_reponse++;
	}
		
		
		// envoyer la longueur -> PROBLEME SI LA LONGUEUR EST SUPERIEUR A 99
		envoie(reponse, lg_reponse);
	}



void get(char *nom_fic)
{
	char fexiste;
	
	printf("Je commence le get !\n");
	FILE * fp;
	
	if(fp = fopen(nom_fic, "r"))
	{
		fexiste = 'Y';
		h_writes(sock,&fexiste,1);

		int nb_lus; //nb d'octets lus dans le fichier
		char buffer [100];
		
		do
		{
			nb_lus = fread(buffer,1,99,fp);
			
			printf("Je vais ecrire %d octets !\n", nb_lus);
			envoie(buffer, nb_lus);
		}while(nb_lus == 99);
		fclose(fp);
	}
	else
	{
		fexiste = 'N';
		h_writes(sock,&fexiste,1);
		
		printf("ce fichier n'existe pas !\n");
	}
	
}


void put(char* nom_fic)
{
	FILE * fp = fopen(nom_fic, "w");
	

	printf("Je commence le put\n");
	
	char tampon[100]; //taille 100 car recoie met un \0 à tampon[99]
	int nb_lus; //resultat du read
		
	do
	{
		recoie(tampon);
		nb_lus=strlen (tampon);
		printf("nb_lus = %d\n", nb_lus);
		fwrite(tampon, sizeof(char), nb_lus, fp);
	}while(nb_lus==99);
	
	fclose(fp);
}


/******************************************************************************/	


