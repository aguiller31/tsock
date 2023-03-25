// Code des fonctions décrivant le code spécifique à chaque mode(émetteur,récepteur, boîte aux lettres)
// ===================================== Fonctions de l'émetteur =====================================
//Fonction correspondant à l'émetteur
void emitter(SocketManagerClass *Socket_r,PDUClass *PDU_r)
{
		// Stockage du PDU correspondant au message à envoyer dans la Class du socket
		Socket_r->SetTempPDU(Socket_r, PDU_r);
		// Création du message contenant le PDU à envoyer, connexion et envoi
		Socket_r->CreateAndSendPDU(Socket_r);
		// Génération du nombre de messages défini, de taille définie, "aléatoirement"
		char **message = generate_random_strings(PDU_r->PDU.nb_lignes,PDU_r->PDU.nb_octet);
		// Affichage des différents messages à envoyer
		for(int i=0; i <PDU_r->PDU.nb_lignes;i++){
			printc(32,"SOURCE : Envoi lettre n°%d à destination du récepteur %d (%d) : ",(i+1), PDU_r->PDU.dest,PDU_r->PDU.nb_octet);
			afficher_message(message[i], PDU_r->PDU.nb_octet);
		}
		// Envoi du lot de messages
		Socket_r->SendMessages(Socket_r,message);
		// Fermeture de la connexion
		Socket_r->Close(Socket_r);
		// Affichage du message de fin
		end();
}
// ================================================================================================== 
// ===================================== Fonctions du récepteur ===================================== 

// Fonction correspondant à la réception de messages par le récepteur
void receipter_getMessage(SocketManagerClass *Socket_r)
{
	// Récupération du PDU de la BAL indiquant le nombre et la taille des messages à récuperer
	Socket_r->GetPDU(Socket_r);
	if(Socket_r->temp_PDU.nb_lignes == 0){
		printc(32,"Vous n'avez pas de nouveaux messages\n");
		end();
	}else if(Socket_r->temp_PDU.nb_lignes == -1){
		printc(32,"Fin des messages\n");
		end();
	}
	else{
		// Récupération des messages
		char **message = NULL;
		message = (char **) malloc(sizeof(char *) * Socket_r->temp_PDU.nb_lignes);
		int lg_recu;
		Socket_r->GetMessages(Socket_r,message,&lg_recu);
		// Affichage des messages
		for(int i = 0; i<Socket_r->temp_PDU.nb_lignes; i++){
			printc(32,"RECEPTION : Récupération lettre n°%d par le récepteur %d (%d) : ",(i+1), Socket_r->temp_PDU.dest,Socket_r->temp_PDU.nb_octet);
			afficher_message(message[i], lg_recu);
		}
		// Fonction récursive, on récupère les messages tant qu'on a pas reçu un PDU indiquant qu'il n'y a plus de messages à récuperer.
		receipter_getMessage(Socket_r);
	}	
}

//Fonction correspondant au récepteur
void receipter(SocketManagerClass *Socket_r,PDUClass *PDU_r)
{
	// Stockage du PDU correspondant au message à envoyer dans la Class du socket pour indiquer à la BAL qu'il s'agit d'un récepteur de N° défini.
	Socket_r->SetTempPDU(Socket_r, PDU_r);
	// Création du message contenant le PDU à envoyer, connexion et envoi
	Socket_r->CreateAndSendPDU(Socket_r);
	#ifdef DEBUG
			// Le récepteur est prêt à récuper les messages
			printc(36,"Passage en mode réception\n");
	#endif
	// Pour passer en réception on inverse les sockets d'émission précedents
	Socket_r->SwapSocket(Socket_r);
	// Récupération des messages
	receipter_getMessage(Socket_r);
}
// ================================================================================================== 
// ====================================== Fonctions de la BAL =======================================

// Fonction permettant de gérer le cas où l'interlocuteur est un récepteur				
void bal_deal_receipter(SocketManagerClass *Socket_r, PDUClass *PDU_r,BALClass *BAL_r)
{
	// Pour passer en émission on inverse les sockets de réception précedents.
	Socket_r->SwapSocket(Socket_r);
	// Stockage du PDU reçu du recepteur dans la class de la BAL de manière temporaire
	BAL_r->SetTempPDU(BAL_r, Socket_r->temp_PDU);
	// Recherche de la BAL correspondant au N° du récepteur
	BAL_r->Current_BAL = BAL_r->Find(BAL_r);
	if (BAL_r->Current_BAL == NULL) {
		#ifdef DEBUG
	            printc(36,"Aucun message trouvé pour le récepteur %d.\n",Socket_r->temp_PDU.dest);
		#endif
		// Création d'un PDU pour informer le récepteur qu'il n'y a aucun message pour lui puisque sa BAL n'existe pa
		PDU_r->SetLength(PDU_r,0);
		PDU_r->SetSize(PDU_r,0);
		// Stockage du PDU
		Socket_r->SetTempPDU(Socket_r, PDU_r);
		// Envoi du PDU
		Socket_r->SendPDU(Socket_r);
	}
	else{
		// Une BAL pour ce recepteur a été trouvée
		BAL_r->Current_Courrier = BAL_r->Current_BAL->lettre;
		// Parcourt des messages présents dans la BAL
        while (BAL_r->Current_Courrier != NULL) {
			#ifdef DEBUG
	            printc(36,"Il y a des messages pour le récepteur %d.\n",Socket_r->temp_PDU.dest);
			#endif
			// Création d'un PDU correspondant aux messages stockés à envoyer
			PDU_r->SetLength(PDU_r,BAL_r->Current_Courrier->nb);
			PDU_r->SetSize(PDU_r,BAL_r->Current_Courrier->lg);
			PDU_r->PDU.dest = Socket_r->temp_PDU.dest;
			// Stockage du PDU
			Socket_r->SetTempPDU(Socket_r, PDU_r);
			// Envoi du PDU
            Socket_r->SendPDU(Socket_r);
			// Envoi des messages
			Socket_r->SendMessages(Socket_r,BAL_r->Current_Courrier->message);
			//Passage aux messages suivants
            Courrier *temp = BAL_r->Current_Courrier;
            BAL_r->Current_Courrier = BAL_r->Current_Courrier->suivant;
        }
		// Supression de tous les messages de la BAL du récepteur
		BAL_r->Flush(BAL_r);
		// Création, Stockage et envoi d'un PDU indiquant qu'il n'y a plus de message à recevoir pour le récepteur.
		PDU_r->SetLength(PDU_r,-1);
		PDU_r->SetSize(PDU_r,0);
		Socket_r->SetTempPDU(Socket_r, PDU_r);
		Socket_r->SendPDU(Socket_r);
	}
	// Fermeture de la connexion
	Socket_r->Close(Socket_r);
}

// Fonction permettant de gérer le cas où l'interlocuteur est un émetteur
void bal_deal_emitter(SocketManagerClass *Socket_r,BALClass *BAL_r)
{
	// Réception et stockage des messages correspondants au PDU reçu
    char **message = NULL;
    message = (char **) malloc(sizeof(char *) * Socket_r->temp_PDU.nb_lignes);
    int lg_recu;
    Socket_r->GetMessages(Socket_r,message,&lg_recu);
	// Affichage des messages
    for(int i = 0; i<Socket_r->temp_PDU.nb_lignes; i++){
		printc(32,"PUITS : Réception et stockage lettre n°%d pour le récepteur %d (%d) : ",(i+1), Socket_r->temp_PDU.dest,Socket_r->temp_PDU.nb_octet);
        afficher_message( message[i], lg_recu);
    }
	//Ajout des messages à la boîte aux lettres du destinataire
	BAL_r->SetTempPDU(BAL_r, Socket_r->temp_PDU);
	BAL_r->AddMessage(BAL_r,message);
	//Fermeture de la connexion
	Socket_r->Close(Socket_r);
}

// Fonction permettant de récuperer un PDU initial permettant d'identifier le mode de l'interlocuteur
void bal_Get(SocketManagerClass *Socket_r,PDUClass *PDU_r, BALClass *BAL_r)
{
	// Création du socket
	Socket_r->Create(Socket_r);
	// Adressage du socket
    Socket_r->Adress(Socket_r);
	//Associate du socket 
	Socket_r->Associate(Socket_r);
	// Acceptation d'une requête et récupération d'un PDU
	Socket_r->Accept(Socket_r);
	Socket_r->GetPDU(Socket_r);
	if(Socket_r->temp_PDU.RouE == 'r')
		bal_deal_receipter(Socket_r,PDU_r,BAL_r);
	else
		bal_deal_emitter(Socket_r,BAL_r);
	// Récursivité pour permettre de nouvelles requêtes
	bal_Get(Socket_r,PDU_r,BAL_r);
}
//Fonction correspondant à la BAL
void bal(SocketManagerClass *Socket_r,PDUClass *PDU_r)
{
	// Création de la BAL
	BALClass *BAL_r = New_BAL();
	// Mise en état de réception de PDU
    bal_Get(Socket_r,PDU_r,BAL_r);
}
