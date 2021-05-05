//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER HEADER CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER HEADER DU SOUS ENSEMBLE F0-M2
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include "FO_M2__Structures_COMMANDES_INFORMATIONS_Serializer.h"
#include "c8051F020_SFR16.h"
#include <C8051F020.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

INFORMATIONS_SERIALIZER FO_M2(COMMANDES_SERIALIZER);	//COMMANDES_SERIALIZER
void Init_FOM2(void);
void Init_UART1(void);
void Init_Timer1(void);
void create_command(COMMANDES_SERIALIZER);
void send_command(char *);
void wait_for_answer(void);
void recup_infos(void);
void create_infos(void) ;


char *strcat(char *chaine1, const char *chaine2){
    long i = strlen(chaine1), j = 0 ;
    for (; chaine2[j] != '\0'; i++, j++)
    {
        chaine1[i] = chaine2[j] ;
    }
    return chaine1 ;
}

char *vider_chaine(char *chaine){
		char i = 0 ;
		char taille = strlen(chaine) ; 
		for ( i; i <= taille; i++){
				chaine[i] = 0 ;
		}
		return chaine ;
}


//------------------- Partie test -------------------------------------------
/*
void testfct(void){
		COMMANDES_SERIALIZER test ;
		test.Set_P = 10 ;
		test.Set_I = 0 ;
		test.Set_D = 0 ;
		test.Set_L_A = 60 ;					//valeur ok pour vpid ??
		test.Vitesse_Mot1 = -25 ;
		test.Ticks_mot1 = 2500 ;
		test.Vitesse_Mot2 = -25 ;
		test.Ticks_mot2 = 2500 ;
		
		test.Etat_Commande = Vpid_read ;
		create_command(test) ;
		vider_chaine(reponse);
		//send_command(command_to_transmit);
		wait_for_answer();
		create_infos();
	
		test.Etat_Commande = mogo_1 ;
		create_command(test) ;
		vider_chaine(reponse);
		send_command(command_to_transmit);
		wait_for_answer();
		create_infos();
		
		test.Etat_Commande = Stop ;
		create_command(test) ;
		send_command(command_to_transmit);
		wait_for_answer();
		create_infos();

}*/


