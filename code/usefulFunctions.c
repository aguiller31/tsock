//Fonctions dites "utiles" appelées régulièrement et non spécifiques à une classe ou un mode.

#include <stdarg.h> //Nécessaire pour les couleurs, pour avoir la fonction va_list pour la fonction print_c

//Fonction équivalente à printf mais dont le premier argument est un entier permettant de définir une couleur pour le texte à afficher
void printc(int couleur, const char* format, ...)
{
	// Stockage du début de la chaîne permettant de définir une couleur
	char code_couleur[10] = "\033[0;";
	// Stockage de la fin de la chaîne permettant de définir une couleur
	char fin_code[5] = "m";
	// Chaîne qui recevra l'entier
	char str_couleur[3];
	// Conversion de l'entier en chaîne
	sprintf(str_couleur, "%d", couleur);
	// Ajout de l'entier converti à la chaîne du début
	strcat(code_couleur, str_couleur);
	// Ajout de la fin de la chaîne
	strcat(code_couleur, fin_code);
	// Affichage du début = début du texte en couleur
	printf("%s", code_couleur);
	// Récupération des arguments : partie équivalente à printf
	// D'après la documentation : holds the information needed by va_start, va_arg, va_end
    va_list args;
	// D'après la documentation : enables access to variadic function arguments
    va_start(args, format);
   // Affichage de la châine
    vprintf(format, args);
	// Affichage de la fin de la chaîne = fin du texte en couleur
    printf("\033[0m");
	// D'après la documentation : performs cleanup for an ap object initialized by a call to va_start. May modify ap so that it is no longer usable.
    va_end(args);
}

// Fonction permettant de générer un tableau contenant des chaînes de caractères aléatoires de tailles définies.
char** generate_random_strings(int num_strings, int string_length)
{
	// Allocation d'un tableau de pointeurs de chaînes
    char** string_array = malloc(num_strings * sizeof(char*));
	// Initialise le générateur de nombres aléatoires avec une valeur fixe
    srand(42);
    for (int i = 0; i < num_strings; i++) {
		// Alloucution d'un tableau de caractères pour chaque chaîne
        string_array[i] = malloc((string_length + 1) * sizeof(char));
        for (int j = 0; j < string_length; j++) {
			// Génère un caractère aléatoire compris entre 'a' et 'z'
            string_array[i][j] = 'a' + rand() % 26;
        }
		// Termine la chaîne avec le caractère nul
        string_array[i][string_length] = '\0';
    }
    return string_array;
}

// Fonction permettant d'afficher les messages en affichant caractère par caractère
void afficher_message(char *message, int lg)
{
	int i;
	for (i=0 ; i<lg ; i++) {
		printf("%c", message[i]) ;
	}
	printf("\n");
}

// Fonction permettant d'afficher les informations des modes, sous le format <couleur>paramètre</couleur> : valeur
void print_param(char *param,char *val)
{
    printc(35,"%s : ",param);
    printf("%s\n",val);
}

// Fonction affichant le message de fin
void end()
{
    printf("\n_______________________\n\n");
    printc(35,"Fin.\n");
}