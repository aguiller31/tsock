/*
========================================
Guillermin Antoine - 3 IMACS-AE, Groupe E
tsock : application distribuée de boîtes aux lettres sur Internet, utilisant l'API socket.
========================================
README
	Lexique :
	* POO : Programation Orientée Objet
	* BAL : Boîte(s) aux lettres
	* PDU : Protocol Data Unit
	
-> Pour plus de lisibilité, on considère que les printf permettent de décrire le code auxquels ils correspondent (sauf précision par un commentaire) et ne seront donc pas commentés
-> Pour plus de lisibilité, pour faciliter le codage et la maintenance (théorique), le code a été séparé en 6 fichiers distincts :
	- tsock.c correspond au code "principal" appelé par la commande ./tsock [paramètres]
	- modesFunctions.c correspond au code des fonctions décrivant le code spécifique à chaque mode(émetteur,récepteur, boîte aux lettres)
	- usefulFunctions.c correspond à des fonctions dites "utiles" appelées régulièrement et non spécifiques à une classe ou un mode.
	Enfin 3 classes se trouvent dans 3 fichiers nomClass.c
		- PDUClass.c corrspond à une classe en "POO" pour gérer les PDU
		- balClass.c corrspond à une classe en "POO" pour gérer les boîtes aux lettres
		- SocketManagerClass.c corrspond à une classe en "POO" pour gérer les sockets
		
Informations sur la Programation Orientée Objet dans ce programme :
	Le langage C n'étant pas un langage orienté objet, il a fallut établir des règles de programmation afin d'obtenir une POO proche de la POO traditionelle.
	Pour ce faire j'ai suivi la norme proposée par le site developpez.com : https://chgi.developpez.com/c/objet/
	Bien sûr cette norme ne fait qu'approcher la "vraie" POO mais ce choix a été fait pour faciliter le développement, la lisbilité et la maintenance(théorique) de ce code.
========================================
*/

// =====================================
// Remplacer par DEBUG pour afficher plus d'informations lors de l'execution
#define DEBUGNO
// =====================================

/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>

// =====================================
// Fonctions pour afficher du texte en couleur, pour afficher les messages, générer une chaîne aléatoire, afficher les paramètres de l'éxécution, afficher la fin de l'exécution
#include "usefulFunctions.c"
//Classe en "POO" pour gérer les PDU
#include "PDUClass.c"
//Classe en "POO" pour gérer les boîtes aux lettres
#include "balClass.c"
//Classe en "POO" pour gérer les sockets
#include "SocketManagerClass.c"
//Fonctions pour gérer les cas où l'utilisateur définit soit le mode boîte aux lettres, récepteur ou émetteur
#include "modesFunctions.c"
// =====================================

void main (int argc, char **argv)
{
	#ifdef DEBUG
  		printc(36,"Mode debug activé !\n");
	#endif
	#ifndef DEBUG
		//Efface le contenu de la console pour plus de lisibilité
		system("clear");
	#endif
	printf("______________________\n\n");	
	//Initialise les deux classes principales pour gérer l'envoi/récéption des messages
	PDUClass *PDU_r = New_PDU();
	SocketManagerClass *Socket_r = New_SocketManager();
	//Récupère tous les arguments de la commande
	int c;
    while ((c = getopt(argc, argv, "e:br:n:l:")) != -1) {
		switch (c){
			case 'b':
				print_param("Mode","boîte aux lettres");
			break;
			case 'e':
				//Définit le mode "émetteur" dans le PDU qui sera envoyé à la BAL ainsi que l'idenfiant du destinataire
				PDU_r->SetMode(PDU_r,atoi(optarg),'e');
				//Définit l'hostname de la BAL
				Socket_r->SetHostname(Socket_r,argv[argc-2]);
				print_param("Mode","émetteur");
				print_param("Destinataire",optarg);
				print_param("Nom de l'hôte distant",argv[argc-2]);
			break;
			case 'r':
				//Définit le mode "récepteur" dans le PDU qui sera envoyé à la BAL ainsi que l'idenfiant du récepteur
				PDU_r->SetRecept(PDU_r,atoi(optarg));
				//Définit l'hostname de la BAL
				Socket_r->SetHostname(Socket_r,argv[argc-2]);
				print_param("Mode","récepteur");
				print_param("Identifiant",optarg);
				print_param("Nom de l'hôte distant",argv[argc-2]);
			break;
			case 'n':
				//Définit le nombre de messages dans le PDU qui sera envoyé à la BAL
				PDU_r->SetLength(PDU_r,atoi(optarg));
				print_param("Nombre de messages",optarg);
			break;
			case 'l':
				//Définit la taille des messages dans le PDU qui sera envoyé à la BAL
				PDU_r->SetSize(PDU_r,atoi(optarg));
				print_param("Longueur des messages",optarg);
			break;
			default:
				 printc(31,"usage: cmd [-e|-r|-b][dest|-n ## -l ## ][port]\n");
			break;
		}
	}
	//Définit le port du socket
	Socket_r->SetPort(Socket_r,atoi(argv[argc-1]));
	print_param("Port",argv[argc-1]);
	print_param("TP","TCP");
	printf("______________________\n\n");
	//Choisit la fonction correspondant au mode choisi : (cf. modesFunctions.c)
	switch(PDU_r->PDU.RouE){
		case 'e':
			emitter(Socket_r,PDU_r);
		break;
		case 'r': 
			receipter(Socket_r,PDU_r);
		break;
		default:
			bal(Socket_r,PDU_r);
		break;
	}	
}


