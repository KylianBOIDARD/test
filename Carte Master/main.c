//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER SOURCE CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER MAIN 
//
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include "FO-MX.h"

//-----------------------------------------------------------------------------
// 				PROTOTYPES DES FONCTIONS  
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void main (void) {
//----- INITIALISATION DES SOUS ENSEMBLES ET PERIPHERIQUES ASSOCIES ---------	
	Init_Device();
	
	Init_FOM1();
	Init_FOM2();
	Init_FO_M3();
	Init_FO_M4();
	
//---- PHASE D'UTILISATION CONTINUE ------------------------------------	
	while (1){			//Phase d'usage classique
			FOM1();
	}




}

