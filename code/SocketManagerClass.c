// Classe en "POO" pour gérer les sockets, basée sur la "norme" proposée par https://chgi.developpez.com/c/objet/
struct sockaddr_in addrlocal;
struct sockaddr_in addrdistant;

// Structure de class permettant de stocker les pointeurs vers les fonctions associées aux sockets, les différents paramètres associés aux communications et temp_PDU permettant de réduire le nombre de paramètres à envoyer à chaque fonction.
typedef struct SocketManagerClass
{
		void(*SetPort)(struct SocketManagerClass*,int);
		void(*SetHostname)(struct SocketManagerClass*,char*);
		void(*Free)(struct SocketManagerClass*);
		void(*Create)(struct SocketManagerClass*);
		void(*AdressEmitter)(struct SocketManagerClass*);
		void(*Adress)(struct SocketManagerClass*);
		void(*Associate)(struct SocketManagerClass*);
		void(*Accept)(struct SocketManagerClass*);
		void(*AssociateAdresseSocketDns)(struct SocketManagerClass*);
		void(*Connect)(struct SocketManagerClass*);
		void(*SwapSocket)(struct SocketManagerClass*);
		void(*SendPDU)(struct SocketManagerClass*);
		void(*Close)(struct SocketManagerClass*);
		void(*SendMessages)(struct SocketManagerClass*, char**);
		void(*GetMessages)(struct SocketManagerClass*, char**, int*);
		void(*GetPDU)(struct SocketManagerClass*);
		void(*SetTempPDU)(struct SocketManagerClass*, struct PDUClass*);
		void(*CreateAndSendPDU)(struct SocketManagerClass*);
		int Sock;
		int SockBis;
		int Port;
		char *Hostname;
		ClassPDUStruct temp_PDU;
} SocketManagerClass ;

// Fonction permettant de définir l'hostname de la BAL
void SocketManagerClass_SetHostname(SocketManagerClass *This,char *hostname)
{
    This->Hostname = hostname;
}
// Fonction permettant de définir le port de la BAL
void SocketManagerClass_SetPort(SocketManagerClass *This,int port)
{
    if(!port){
		printc(31,"usage: cmd [-e|-r|-b][dest|-n ## -l ## ][port]\n");
		exit(1) ;
	}
    else if(port < 8000){
        printc(31,"Pour éviter les conflits avec d'autres processus, veuillez choisir un port supérieur à 8000\n");
        exit(1);
    } 
    This->Port = port;
}

// Fonction permettant d'échanger Sock et Sockbis pour passer de lecture à écriture sur le socket et réciproquement
void SocketManagerClass_SwapSocket(SocketManagerClass *This)
{
    int temp = This->Sock;
    This->Sock = This->SockBis;
    This->SockBis = temp;
}

// Fonction permettant de récuperer les messages 
void SocketManagerClass_GetMessages( SocketManagerClass *This, char **messagereceived, int *lg_recu)
{
	int j;
	int max = This->temp_PDU.nb_lignes;   
	for (j=0;j<max;j++)
	{
		messagereceived[j] =  (char *) malloc(sizeof(char ) * This->temp_PDU.nb_octet);
		if((*lg_recu = read(This->SockBis,messagereceived[j],This->temp_PDU.nb_octet))<0)
		{printf("echec du read\n");exit(1);}

	}        
}

// Fonction permettant de récuperer un PDU
void SocketManagerClass_GetPDU( SocketManagerClass *This)
{
    int bytes_received = read(This->SockBis, &This->temp_PDU, sizeof(This->temp_PDU));
	#ifdef DEBUG
		if (bytes_received == sizeof(This->temp_PDU))
	        printc(36,"Received structure: dest=%d, nb_octet=%d, nb_lignes=%d, RouE=%c\n", This->temp_PDU.dest,This->temp_PDU.nb_octet,This->temp_PDU.nb_lignes, This->temp_PDU.RouE);
		else
			printf("Error receiving structure.\n");
	#endif 
}

// Fonction permettant d'accepter une requête
void SocketManagerClass_Accept(SocketManagerClass *This)
{
          struct sockaddr sock_recep;
           int lg_struct =  sizeof(sock_recep);
           listen(This->Sock, 100);
           if ((This->SockBis = accept(This->Sock,(struct sockaddr*)&sock_recep, &lg_struct))==-1){
               printf("échec du accept\n");
               exit(1) ;
           }
           #ifdef DEBUG
	            printc(36,"La connexion avec le serveur a été acceptée.\n");
           #endif 
}

// Fonction permettant l'adressage d'un socket
void SocketManagerClass_Adress (SocketManagerClass *This)
{
	memset((char*)&addrlocal,0,sizeof(addrlocal));
    addrlocal.sin_family = AF_INET;
    addrlocal.sin_port = This->Port;
    addrlocal.sin_addr.s_addr = INADDR_ANY;
}

// Fonction permettant d'envoyer un message
void SocketManagerClass_SendMessages( SocketManagerClass *This, char **messagesent)
{
	for (int i=0;i<This->temp_PDU.nb_lignes;i++){
		if (send(This->Sock,messagesent[i],This->temp_PDU.nb_octet,MSG_NOSIGNAL)==-1){     	
        	printf("echec du send\n");
        	exit(1);
   	    }
	} 
}

// Fonction permettant de fermer le socket
void SocketManagerClass_Close(SocketManagerClass *This)
{
    close(This->SockBis);
    close(This->Sock);
}

// Fonction permettant d'envoyer un PDU
void SocketManagerClass_SendPDU(SocketManagerClass *This)
{
   #ifdef DEBUG
		printc(36,"Envoie d'un PDU \nMode: %c Nb Messages : %d, Nb Octets %d, Dest : %d\n",This->temp_PDU.RouE,This->temp_PDU.nb_lignes,This->temp_PDU.nb_octet,This->temp_PDU.dest);
   #endif 
   int message_size = sizeof(struct ClassPDUStruct);
   char *message_buffer = (char *) &This->temp_PDU;
   int bytes_sent = 0;
    while (bytes_sent < message_size) {
        int result = send(This->Sock, message_buffer + bytes_sent, message_size - bytes_sent, 0);
        if (result == -1) {
            exit(EXIT_FAILURE);
        }
        bytes_sent += result;
    }
}

// Fonction permettant de connecter un socket
void SocketManagerClass_Connect(SocketManagerClass *This)
{
	struct sockaddr sock_emetteur;
	int lg_struct =  sizeof(sock_emetteur);
	if ((This->SockBis = connect(This->Sock,(struct sockaddr*)&addrdistant,lg_struct))==-1){
		printf("echec de la connexion \n");
		close(This->SockBis);
		exit(1);
	}
	#ifdef DEBUG
		printc(36,"La connexion avec le client a été réussie.\n");
	#endif 
}

// Fonction permettant d'associer un socket avec l'hôte distant via le protocole DNS
void SocketManagerClass_AssociateAdresseSocketDns (SocketManagerClass *This)
{
    struct hostent *hp;
	memset((char*)&addrdistant,0,sizeof(addrdistant));
	addrdistant.sin_family = AF_INET;
	addrdistant.sin_port = This->Port;
	if ((hp = gethostbyname(This->Hostname))== NULL){
		printf("erreur gethostbyname\n");
		exit(1);
	}
	memcpy((char*)&(addrdistant.sin_addr.s_addr), hp->h_addr, hp->h_length ) ;
}

// Fonction permettant d'associer un socket
void SocketManagerClass_Associate(SocketManagerClass *This)
{
	int lg_adr_local = sizeof(addrlocal);
	if (bind(This->Sock,(struct sockaddr*)&addrlocal,lg_adr_local)==-1){
		printf("echec du bind\n");
		exit(1);
	}
}

//Fonction permettant d'adresser un socket d'émission
void SocketManagerClass_AdressEmitter(SocketManagerClass *This)
{
    memset((char*)&addrlocal,0,sizeof(addrlocal));
    addrlocal.sin_family = AF_INET;
    addrlocal.sin_port = rand() % 5000 + 1000;
    addrlocal.sin_addr.s_addr = INADDR_ANY;
} 

// Fonction permettant de créer un socket
void SocketManagerClass_Create(SocketManagerClass *This)
{
	int optval = 1;
	This->Sock=socket(AF_INET,SOCK_STREAM ,0);
	// Permet de réutiliser le même port lors d'executions successives
    setsockopt(This->Sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
    #ifdef DEBUG
	    printc(36,"Vous avez bien crée un socket avec protocole TCP\n");
    #endif 
}

// Fonction permettant la connexion, la création d'un PDU et son envoi
void SocketManagerClass_CreateAndSendPDU(SocketManagerClass *This)
{
	This->Create(This);
    This->AdressEmitter(This);
    This->Associate(This);
    This->AssociateAdresseSocketDns(This);
    This->Connect(This);
    This->SendPDU(This);
}

// Fonction permettant le stockage d'un PDU temporaire, non nécessaire mais utilisée pour la lisibilité
void SocketManagerClass_SetTempPDU(SocketManagerClass *This, PDUClass* PDU_r)
{
    This->temp_PDU = PDU_r->PDU;
}

//=================================== Utilisation de la norme proposée par  https://chgi.developpez.com/c/objet/

static void SocketManagerClass_Init(SocketManagerClass *This)
{
    This->Create = SocketManagerClass_Create;
    This->Associate= SocketManagerClass_Associate;
    This->AdressEmitter= SocketManagerClass_AdressEmitter;
    This->Adress= SocketManagerClass_Adress;
    This->AssociateAdresseSocketDns=SocketManagerClass_AssociateAdresseSocketDns;
    This->Connect=SocketManagerClass_Connect;
    This->Accept=SocketManagerClass_Accept;
    This->SendPDU=SocketManagerClass_SendPDU;
    This->Close=SocketManagerClass_Close;
    This->SendMessages=SocketManagerClass_SendMessages;
    This->GetMessages=SocketManagerClass_GetMessages;
    This->GetPDU=SocketManagerClass_GetPDU;
    This->SwapSocket=SocketManagerClass_SwapSocket;
    This->SetPort=SocketManagerClass_SetPort;
    This->SetHostname=SocketManagerClass_SetHostname;
    This->SetTempPDU = SocketManagerClass_SetTempPDU;
    This->CreateAndSendPDU=SocketManagerClass_CreateAndSendPDU;
       
}

// Fonction qui pourrait être supprimée car elle n'est jamais appelée dans ce code mais conservée dans le cas où elle pourrait servir s'il faut libérer l'espace mémoire
void SocketManagerClass_New_Free(SocketManagerClass *This)
{
        
        free(This);        
        puts("Destruction de la pile dynamique.\n");
}

SocketManagerClass* New_SocketManager()
{
       SocketManagerClass *This = malloc(sizeof(SocketManagerClass));
       if(!This) return NULL;
       SocketManagerClass_Init(This);
       This->Free = SocketManagerClass_New_Free;
       return This;
}
