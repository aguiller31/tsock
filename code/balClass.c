// Classe en "POO" pour gérer les BAL, basée sur la "norme" proposée par https://chgi.developpez.com/c/objet/

// Structure permettant le stockage des messages
typedef struct Courrier Courrier;
struct Courrier
{
    int lg;
	int nb;
	char **message;
    Courrier *suivant;
};

//Structure permettant le stockage des BAL
typedef struct BALStruct BALStruct;
struct BALStruct
{
	int dest;
	Courrier *lettre;
   	BALStruct *suivant;
};

// Structure de class permettant de stocker les pointeurs vers les fonctions associées aux BAL, la structure de stockage de BAL ainsi que des structures temporaire Current_BAL,Current_Courrier et temp_PDU permettant de réduire le nombre de paramètres à envoyer à chaque fonction.
typedef struct BALClass
{
		void(*Free)(struct BALClass*);
		void(*AddMessage)(struct BALClass*, char**);
		void(*SetTempPDU)(struct BALClass*, struct ClassPDUStruct);
		BALStruct*(*Find)(struct BALClass*);
		void(*Flush)(struct BALClass*);
		BALStruct *Current_BAL;
		BALStruct *BAL;
		Courrier *Current_Courrier;
		ClassPDUStruct temp_PDU;

} BALClass ;

// Fonction permettant de vider la BAL "actuelle"
void BALClass_Flush(BALClass *This) {
    // Parcours de tous les éléments de la BAL
    Courrier *courrierActuel = This->Current_Courrier;
    while (courrierActuel != NULL) {
        Courrier *prochainCourrier = courrierActuel->suivant;
        // Libération de la mémoire allouée pour le courrier
        for (int i = 0; i < courrierActuel->lg; i++) {
            free(courrierActuel->message[i]);
        }
        free(courrierActuel->message);
        free(courrierActuel);
        courrierActuel = prochainCourrier;
    }
    // Mise à jour de la tête de liste de la BAL pour marquer qu'elle est vide
    This->Current_BAL->lettre = NULL;
}

// Fonction retournant le dernier courrier de la BAL "actuelle", retourne NULL s'il n'y a pas de courrier
Courrier* BALClass_getLastCourrier(BALClass *This)
{
    if (This->Current_Courrier == NULL) {
        return NULL;
    }
    Courrier* derniere_courrier = This->Current_Courrier;
    while (derniere_courrier->suivant != NULL) {
        derniere_courrier = derniere_courrier->suivant;
    }
    return derniere_courrier;
}

// Fonction retournant la dernière BAL, retourne NULL s'il n'y a pas de BAL
BALStruct* BALClass_getLastBAL(BALClass *This)
{
    if (This->BAL == NULL) {
        return NULL;
    }
    BALStruct* derniere_BAL = This->BAL;
    while (derniere_BAL->suivant != NULL) {
        derniere_BAL = derniere_BAL->suivant;
    }
    return derniere_BAL;
}

// Fonction retournant la BAL associée à un récepteur, retourne NULL si aucune BAL n'est trouvée
BALStruct* BALClass_Find(BALClass *This)
{
    BALStruct* bal_courante = This->BAL;
    while (bal_courante != NULL) {
        if (bal_courante->dest == This->temp_PDU.dest) {
            return bal_courante;
        }
        bal_courante = bal_courante->suivant;
    }
    return NULL;
}

// Fonction permettant d'ajouter les données associées à un courrier, non accessible à l'extérieur de la classe d'après la "norme"
static void BALClass_createCourrier(BALClass *This, char **message)
{
		This->Current_Courrier->lg=This->temp_PDU.nb_octet;
		This->Current_Courrier->nb=This->temp_PDU.nb_lignes;
		This->Current_Courrier->message = message;
		This->Current_Courrier->suivant=NULL;
}

// Fonction permettant d'ajouter les données associées à une BAL, non accessible à l'extérieur de la classe d'après la "norme"
static void BALClass_createBAL(BALClass *This)
{
        This->Current_BAL->dest = This->temp_PDU.dest;
        This->Current_BAL->lettre=(Courrier*) malloc(sizeof(Courrier));
        This->Current_Courrier = This->Current_BAL->lettre;
		This->Current_BAL->suivant=NULL;
}

// Fonction permettant d'ajouter un message à une BAL et si elle n'existe pas, de la créer
void BALClass_AddMessage(BALClass *This, char **message)
{
    #ifdef DEBUG
	    printc(36,"Ajout d'un nouveau lot de messages à la BAL.\n");
    #endif
   if(This->BAL->dest == -1){
		#ifdef DEBUG
				printc(36,"La BAL n'a pas encore été initialisée.\n ");
		#endif
		// Définition de la BAL actuelle
		This->Current_BAL = This->BAL;
		// Ajout des données associées à la nouvelle BAL
		BALClass_createBAL(This);
		// Ajout des données associées aux courriers de la nouvelles BAL
		BALClass_createCourrier(This, message);
	}
	else{
		#ifdef DEBUG
			printc(34,"La BAL a déjà été initialisée.\n ");
		#endif	
		// Recherche de la BAL associée au récepteur
		This->Current_BAL = This->Find(This);
		if(This->Current_BAL==NULL){
			#ifdef DEBUG
	            printc(36,"Il n'y a pas encore de BAL pour ce récepteur.\n ");
			#endif
			// Récupération de la dernière BAL
			This->Current_BAL = BALClass_getLastBAL(This);
			// Création d'une BAL à la suite
            This->Current_BAL->suivant = (BALStruct*) malloc(sizeof(BALStruct));
			// Définition de la BAL actuelle
            This->Current_BAL = This->Current_BAL->suivant;
			// Ajout des données associées à la nouvelle BAL
            BALClass_createBAL(This);
			// Ajout des données associées aux courriers de la nouvelles BAL
            BALClass_createCourrier(This, message);
		}
		else{
			#ifdef DEBUG
				printc(36,"Il y a déjà une BAL pour ce récepteur.\n ");
			#endif
			// Définition du courrier actuel, le premier de la BAL
            This->Current_Courrier = This->Current_BAL->lettre;
			// Récupération du dernier courrier de la BAL, devenant le courrier actuel
			This->Current_Courrier = BALClass_getLastCourrier(This);
			if(This->Current_Courrier == NULL){
				#ifdef DEBUG
	                printc(36,"Il n'y a pas encore de courrier pour ce récepteur.\n ");
			    #endif
				// Création d'un nouveau courrier dans la BAL
                This->Current_BAL->lettre = (Courrier*) malloc(sizeof(Courrier));
				// Définition du courrier actuel
                This->Current_Courrier = This->Current_BAL->lettre;
				// Ajout des données associées aux courriers
                BALClass_createCourrier(This, message);
			}
			else{ 
                #ifdef DEBUG
	                printc(36,"Il y a déjà du courrier pour ce récepteur.\n ");
			    #endif
				// Création d'un nouveau courrier dans la BAL
				This->Current_Courrier->suivant = (Courrier*) malloc(sizeof(Courrier));
				// Définition du courrier actuel
				This->Current_Courrier = This->Current_Courrier->suivant;
				// Ajout des données associées aux courriers
				BALClass_createCourrier(This, message);
			}	
		} 
	} 
}

// Fonction permettant le stockage d'un PDU temporaire, non nécessaire mais utilisée pour la lisibilité
void BALClass_SetTempPDU(BALClass *This, ClassPDUStruct PDU)
{
    This->temp_PDU = PDU;
}

//=================================== Utilisation de la norme proposée par  https://chgi.developpez.com/c/objet/

static void BALClass_Init(BALClass *This)
{
	// Création de la BAL
    This->BAL = malloc(sizeof(BALStruct));
	if(!This->BAL) exit(EXIT_FAILURE);
	if (This->BAL != NULL) {
        This->BAL->dest = -1;
        This->BAL->lettre = NULL;
        This->BAL->suivant = NULL;
    }
	// Allocution d'espace pour la BAL temporaire (="actuelle")
    This->Current_BAL = malloc(sizeof(BALStruct));
	// Allocution d'espace pour le courrier temporaire (="actuelle")
    This->Current_Courrier = malloc(sizeof(Courrier));
    This->AddMessage = BALClass_AddMessage;
    This->SetTempPDU = BALClass_SetTempPDU;
    This->Find = BALClass_Find;
    This->Flush = BALClass_Flush;
}

// Fonction qui pourrait être supprimée car elle n'est jamais appelée dans ce code mais conservée dans le cas où elle pourrait servir s'il faut libérer l'espace mémoire
void BALClass_New_Free(BALClass *This)
{
        free(This);        
        puts("Destruction de la pile dynamique.\n");
}

BALClass* New_BAL()
{
       BALClass *This = malloc(sizeof(BALClass));
       if(!This) return NULL;
       BALClass_Init(This);
       This->Free = BALClass_New_Free;
       return This;
}