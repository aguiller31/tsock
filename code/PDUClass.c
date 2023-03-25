// Classe en "POO" pour gérer les PDU, basée sur la "norme" proposée par https://chgi.developpez.com/c/objet/
// Cette classe n'est pas forcément nécessaires puisqu'elle ne contient que des Set.. mais permet plus de lisibilité dans le code

// Structure permettant le stockage du PDU
typedef struct ClassPDUStruct{
	int nb_octet;
	int nb_lignes;
	int dest;
	char RouE;
        int port;
} ClassPDUStruct;

// Structure de class permettant de stocker les pointeurs vers les fonctions associées aux PDU, ainsi que la structure permettant le stockage des PDU
typedef struct PDUClass
{
		//int(*Send)(struct PDUClass*, int);
		void(*Free)(struct PDUClass*);
		void (*SetRecept)(struct PDUClass*, int);
		void (*SetMode)(struct PDUClass*, int,char);
		void (*SetLength)(struct PDUClass*, int);
		void (*SetSize)(struct PDUClass*, int);
		
		ClassPDUStruct PDU;

} PDUClass ;

// Fonction permettant de définir le mode du PDU (Emetteur ou Récepteur) ainsi que le numéro du récepteur associé
void PDUClass_SetMode(PDUClass *This, int dest,char mode)
{
	if(!dest){
		printc(31,"usage: cmd [-e|-r|-b][dest|-n ## -l ## ][port]\n");
		exit(1) ;
	}
	This->PDU.dest = dest;
	This->PDU.RouE = mode;
}

// Fonction permettant de définir un PDU récepteur
void PDUClass_SetRecept(PDUClass *This, int dest)
{
    This->PDU.nb_lignes = 0;
    This->PDU.nb_octet = 0;
    PDUClass_SetMode(This,dest,'r');
}

// Fonction permettant de définir le nombre de message à envoyer dans le PDU
void PDUClass_SetLength(PDUClass *This, int nb)
{
    This->PDU.nb_lignes = nb;
}
// Fonction permettant de définir la taille des messages à envoyer dans le PDU
void PDUClass_SetSize(PDUClass *This, int lg)
{
    This->PDU.nb_octet = lg;
}


//=================================== Utilisation de la norme proposée par  https://chgi.developpez.com/c/objet/

static void PDUClass_Init(PDUClass *This)
{
        This->SetRecept = PDUClass_SetRecept;
        This->SetMode = PDUClass_SetMode; 
        This->SetLength = PDUClass_SetLength; 
        This->SetSize = PDUClass_SetSize; 
        
}

// Fonction qui pourrait être supprimée car elle n'est jamais appelée dans ce code mais conservée dans le cas où elle pourrait servir s'il faut libérer l'espace mémoire
void PDUClass_New_Free(PDUClass *This)
{
        free(This);        
        puts("Destruction de la pile dynamique.\n");
}

PDUClass* New_PDU()
{
       PDUClass *This = malloc(sizeof(PDUClass));
       if(!This) return NULL;
       PDUClass_Init(This);
       This->Free = PDUClass_New_Free;
       return This;
}
