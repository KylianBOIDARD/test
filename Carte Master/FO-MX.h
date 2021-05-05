//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER HEADER CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER HEADER GLOBAL DES SOUS ENSEMBLES
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
//----------- Fichiers Généraux --------------------------------
#include "FO_M2__Structures_COMMANDES_INFORMATIONS_Serializer.h"
#include "FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "c8051F020_SFR16.h"
#include <C8051F020.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//--------------------------------------------------------------
//---------- Variables Globales Carte Master -------------------

extern COMMANDES commande ;
extern INFORMATIONS infos ;

//---------------------------------------------------------------
//------- Prototypes des fonctions ------------------------------
//------- Initialisation Générale ---------------------
void Init_Device(void);
void Delay(const int time_wait);
//----------- FO-M1 -----------------------------------
void Init_FOM1(void);
void FOM1(void);
//----------- FO-M2 -----------------------------------
INFORMATIONS_SERIALIZER FO_M2(COMMANDES_SERIALIZER);	//COMMANDES_SERIALIZER
void Init_FOM2(void);
//----------- FO-M3 -----------------------------------
void Init_FO_M3(void);
unsigned int FO_M3 (int) ;
//----------- FO-M4 -----------------------------------
void Init_FO_M4(void);
int FO_M4(char ordre);
//----------- FO-M5 -----------------------------------

//----------- FO-M6 -----------------------------------

void FO_M6(void) ;
#define PI 3.14159265359 
INFORMATIONS_SERIALIZER transform_command_Seria( COMMANDES ) ;
INFORMATIONS transform_infos( INFORMATIONS_SERIALIZER ) ;
void command_Telemetre( void ) ;


//--------------------------------------------------------------
