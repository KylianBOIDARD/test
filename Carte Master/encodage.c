#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <C8051F020.h>
#include "c8051F020_SFR16.h"

#include "encodage.h"
#include "FOM1.h"


#include <FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h>

struct INFORMATIONS xdata struct_information; // structure stocké en xdata pour des soucis de taille

char xdata information_encoder[40];

/**
 * Initialise la structure d'information 
**/
void init_struct_information()
{
	strcpy(information_encoder,"");
	struct_information.Etat_Invite=Invite_non;
	struct_information.MSG_Invit="";

	struct_information.Etat_BUT_Mouvement=BUT_Atteint_non;
	struct_information.Etat_BUT_Servo=BUT_Servo_non;
	struct_information.Etat_DCT_Obst=DCT_Obst_non;
	//struct_information.Tab_Val_Obst=null;
	struct_information.Nbre_Val_obst=0;
	struct_information.Etat_RESULT_Courant=RESULT_Courant_non;
	struct_information.Mesure_Courant=0;
	struct_information.Etat_RESULT_Energie=RESULT_Energie_non;
	struct_information.Mesure_Energie=0;
	struct_information.Etat_RESULT_Position=RESULT_Position_non;
	struct_information.Pos_Coord_X=0;
	struct_information.Pos_Coord_Y=0;
	struct_information.Pos_Angle=0;
	struct_information.Etat_Aux=Aux_non;
	struct_information.MSG_Aux="";
}

void traitement_information()
{
	if(struct_information.Etat_Invite == Invite_oui) //I
	{	
			int i = 0;
			char char_msg;
			//char msg[32];
			strcat(information_encoder, "I ");
			
			while (*(struct_information.MSG_Invit+i) != '\0')
			{
				char_msg = *(struct_information.MSG_Invit+i);
				//strcpy();
				//strcat(msg,char_msg);
				i++;
			}
	}
	if(struct_information.Etat_BUT_Mouvement == Invite_oui) //B
	{
			strcat(information_encoder, "B");
	}
	if(struct_information.Etat_BUT_Servo != BUT_Servo_non) //AS
	{
			strcat(information_encoder, "AS ");
			if (struct_information.Etat_BUT_Servo == BUT_Servo_H) 
			{
					strcat(information_encoder, "H");
			} else if (struct_information.Etat_BUT_Servo == BUT_Servo_V) 
			{
					strcat(information_encoder, "V");
			}
	}
	if(struct_information.Etat_DCT_Obst == DCT_Obst_180_oui || struct_information.Etat_DCT_Obst == DCT_Obst_360_oui)
	{
			strcat(information_encoder, "B");
	}
	if(struct_information.Etat_RESULT_Courant == RESULT_Courant_oui)//KI
	{
			char courant[4];
			sprintf(courant,"%d",struct_information.Mesure_Courant);
			strcat(information_encoder, "KI ");
			strcat(information_encoder, courant);
	}
	if(struct_information.Etat_RESULT_Energie == RESULT_Energie_oui) //KE
	{
			char energie[4];
			sprintf(energie,"%d",struct_information.Mesure_Energie);
			strcat(information_encoder, "KE ");
			strcat(information_encoder, energie);
	}
	if(struct_information.Etat_RESULT_Position == RESULT_Position_oui) //VPO
	{
		char x[3];
		char y[3];
		char angle[4];
		sprintf(x,"%d",struct_information.Pos_Coord_X);
		sprintf(y,"%d",struct_information.Pos_Coord_Y);
		sprintf(angle,"%d",struct_information.Pos_Angle);

		strcat(information_encoder, "VPO ");
		strcat(information_encoder, "X:");
		strcat(information_encoder, x);
		
		strcat(information_encoder, " Y:");
		strcat(information_encoder, y);
		
		strcat(information_encoder, " A:");
		strcat(information_encoder, angle);
	}
	if(struct_information.Etat_Aux == Aux_oui) //IA
	{
			int i = 0;
			strcat(information_encoder, "IA ");
					while (*(struct_information.MSG_Aux+i) != '\0')
			{
				//strcat(information_encoder,*(struct_information.MSG_Aux+i));
			}
	}
}
