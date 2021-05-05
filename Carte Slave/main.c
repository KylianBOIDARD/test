//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER SOURCE CARTE SLAVE
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
#include "FF_SX.h"


void main (void) {
//----- INITIALISATION DES SOUS ENSEMBLES ET PERIPHERIQUES ASSOCIES -----------
		Init_Device();
	
		init_FFS1();
		Init_FF_S3();

	
//---- PHASE D'UTILISATION CONTINUE ------------------------------------	
		while(1){
		
		}
	
}










