//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER HEADER CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER HEADER DU SOUS ENSEMBLE FO-M4
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
//----------- Fichiers Généraux --------------------------------
#include "c8051F020.h"

//------- Prototypes des fonctions -----------------------------
void Init_M4(void);
void Init_FO_M4(void);
int FO_M4(char ordre);
void MES_Dist_AV (void);
void MES_Dist_AR (void);
void Trigger_AV (void);
void Trigger_AR (void);
void Timer4_ISR (void) ;
unsigned int convert (int);

