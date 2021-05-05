#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <C8051F020.h>
#include "c8051F020_SFR16.h"

#include "decodage.h"
#include "FOM1.h"
#include <math.h>

#include <decodage.h>
#include <FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h>

struct COMMANDES xdata commande; // structure stocké en xdata pour des soucis de taille

char xdata commande_a_decoder[15][15];

int vitesse_max = 160; //vitesse de rotation maximale du moteur pour le déplacement
int indice;
int vitesse_tempo = 0 ;
int Coord_X_absolu = 0;
int Coord_Y_absolu = 0;
int Angle_absolu = 0;
int Angle_relatif = 0 ;
int commande_enable = 0;
	
/**
 * Initialise la structure de commande 
**/
void init_commande()
{
	commande.Etat_Epreuve = Epreuve_non;
	commande.Etat_Mouvement = Mouvement_non;
	commande.Vitesse = 20;
	commande.Coord_X = 0;
	commande.Coord_Y = 0;
	commande.Angle = 0;
	commande.Etat_ACQ_Son = ACQ_non;
	commande.ACQ_Duree = 0;
	commande.Etat_DCT_Obst = DCT_non;
	commande.DCT_Obst_Resolution = 0;
	commande.Etat_Lumiere = Lumiere_non;
	commande.Lumiere_Intensite = 0;
	commande.Lumiere_Duree = 0;
	commande.Lumiere_Extinction = 0;
	commande.Lumiere_Nbre = 0;
	commande.Etat_Servo = Servo_non;
	commande.Servo_Angle = 0;
	commande.Etat_Energie = Energie_non;
	commande.Etat_Position = Position_non;
	commande.Pos_Coord_X = 0;
	commande.Pos_Coord_Y = 0;
	commande.Pos_Angle = 0;
	commande.Etat_Photo = Photo_non;
	commande.Photo_Duree = 0;
	commande.Photo_Nbre = 0;
}


int cTOi(char *str)
{	
  char rev[100];
  int t;
	int i;
	int j;
	int value = 0;
	char *c;
	j = 0;
    t = strlen(str);
 
    rev[t] = '\0'; //le dernier caractère doit toujours être égale à '\r'.
	//inversion de la chaine de caractère
    for (i = t - 1; i >= 0; i--)
    {
      rev[j++] = str[i];
    }
    rev[i] = '\0';
	j = 0;
	
	for( c = rev; *c != '\0' && ((*c >= '0' && *c <= '9' )|| *c == '-'); ++c) {
		if (*c == '-') value = -value;
    else value = (*c - 48)*pow(10, j) +value;
		j++;
	}
	return value;
}

/**
 * Réalise un premier découpage du message de commande pour isoler chaque groupe de caractère
**/
void decoupage_caractere_message(char *commande)
{
	int i = 0;
	int j = 0; int k = 0;
	clear_commande();
	for (i = 0; i<strlen(commande); i++)
	{
		if (commande[i] == ' ' || commande[i] == ':')
		{
			commande_a_decoder[j][k] = '\0';
			j++;
			k = 0;
		}
		else
		{
			commande_a_decoder[j][k] = commande[i];
			k++;
		}
	}
}

/**
* Donne l'indice associé à un paramètre dans la commande à décoder
**/
int recherche_indice(char* motif)
{
	int i = 0;
	for (i = 0; i < sizeof(commande_a_decoder); i++)
	{
		if (!strcmp(commande_a_decoder[i],motif))
		{
       return i;
		}
	}
	return -1;
}
/**
* Permet de vider une chaîne de caractère
**/
void clear_commande(){
	int i = 0;
	int j = 0;
	while (i != sizeof(commande_a_decoder)/sizeof(commande_a_decoder[0])){
		if (commande_a_decoder[i][j] == '\0')
		{
			i++;
			j = 0;
		} else
		{
			commande_a_decoder[i][j] = '\0';
			j++;
		}
	}
}

/*char* convert(int n){
	char ret[4];
	sprintf(ret,"%d",n);
	return ret;
}*/


/**
 * Defini la structure de commande correspondante à la commande rentrante
**/
int traitement_commande()
{
		if(!strcmp(commande_a_decoder[0],"D"))
		{
			//serOutstring("\n Epreuve 1 \n");
			commande.Etat_Epreuve = epreuve1;
			
				if(0 <= cTOi(commande_a_decoder[1]) && cTOi(commande_a_decoder[1])<=8)
				{
					commande.Etat_Epreuve = cTOi(commande_a_decoder[1]);
				}
				serOutstring("\n\rInvite de commande \n\r");
				commande_enable = 1;
			
			return 1;
		}

		else if(!strcmp(commande_a_decoder[0],"E"))
		{
			commande.Etat_Epreuve = Fin_Epreuve;
			commande_enable = 0;
			return 1;
		}
		if (commande_enable == 1)//commande.Etat_Epreuve != Epreuve_non)
		{
		 if(!strcmp(commande_a_decoder[0],"Q"))
		{
					commande.Etat_Epreuve = Stop_Urgence;
					commande.Etat_Mouvement = Stopper;
					commande.Etat_ACQ_Son = ACQ_non;
					commande.Etat_DCT_Obst = DCT_non;
					commande.Etat_Lumiere = Eteindre;
					commande.Etat_Servo = Servo_non;
					commande.Etat_Energie = Energie_non;
					commande.Etat_Position = Position_non;
					commande.Etat_Photo = Photo_stop;
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"TV"))
		{
				if(5 <= cTOi(commande_a_decoder[1]) && cTOi(commande_a_decoder[1]) <= 100)
				{
					commande.Vitesse = cTOi(commande_a_decoder[1]);
					return 1;
				}
		}
		else if(!strcmp(commande_a_decoder[0],"A"))
		{
	
						if(5 <= cTOi(commande_a_decoder[1]) && cTOi(commande_a_decoder[1]) <= 100)
						{
								vitesse_tempo = cTOi(commande_a_decoder[1]);
						}
						commande.Etat_Mouvement = Avancer;
						return 1;

		}
		else if(!strcmp(commande_a_decoder[0],"B"))
		{				
			if(5 <= cTOi(commande_a_decoder[1]) && cTOi(commande_a_decoder[1]) <= 100)
					{
							vitesse_tempo = cTOi(commande_a_decoder[1]);
					}
					commande.Etat_Mouvement = Reculer;
			return 1;

		}
		else if(!strcmp(commande_a_decoder[0],"S"))
		{
					commande.Etat_Mouvement=Stopper;
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"RD"))
		{
				commande.Angle = -90 ;
				commande.Etat_Mouvement = Rot_90D;	
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"RG"))
		{
				commande.Angle = 90 ;
				commande.Etat_Mouvement = Rot_90G;
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"RC"))
		{
				if(!strcmp(commande_a_decoder[1],"D") || !strcmp(commande_a_decoder[1],""))
				{
					commande.Etat_Mouvement = Rot_180D;
					return 1;
				}
				if(!strcmp(commande_a_decoder[1],"G"))
				{
					commande.Etat_Mouvement = Rot_180G;
					return 1;
				}
		}


		else if(!strcmp(commande_a_decoder[0],"RA"))
		{
			int xdata indD = recherche_indice("D");
			int xdata indG = recherche_indice("G");
			commande.Angle = -90;
			commande.Etat_Mouvement = Rot_AngD;

			if(indD >= -1)
			{
				commande.Etat_Mouvement = Rot_AngD;
				commande.Angle = cTOi(commande_a_decoder[indD+1]);
			}
			if((indG >= -1))
			{
				commande.Etat_Mouvement = RotAngG;
				commande.Angle = cTOi(commande_a_decoder[indG+1]);
			}
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"G"))
		{		
			int xdata indX = recherche_indice("X");
			int xdata indY = recherche_indice("Y");
			int xdata indA = recherche_indice("A");
				if ((indX >= 0) && (-99 <= cTOi(commande_a_decoder[indX+1])) && (cTOi(commande_a_decoder[indX+1] ) <= 99) &&
					 (indY >= 0) && (-99 <= cTOi(commande_a_decoder[indY+1])) && (cTOi(commande_a_decoder[indY+1] ) <=99) &&
					 (indA >= 0) && (-180 <= cTOi(commande_a_decoder[indA+1])) && (cTOi(commande_a_decoder[indA+1])<=180))
						{
							commande.Etat_Mouvement = Depl_Coord;
							commande.Coord_X = cTOi(commande_a_decoder[indX+1]);
							commande.Coord_Y = cTOi(commande_a_decoder[indY+1]);
							commande.Angle = cTOi(commande_a_decoder[indA+1]);
						}
						return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"ASS"))
		{
			commande.Etat_ACQ_Son = ACQ_oui;
			commande.ACQ_Duree = 1;
			
			if(1<= cTOi(commande_a_decoder[1]) && cTOi(commande_a_decoder[1]) <= 99)
			{
			commande.ACQ_Duree = cTOi(commande_a_decoder[1]);
				
			}
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"MI"))
		{
			commande.Etat_Energie = Mesure_I;
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"ME"))
		{
			commande.Etat_Energie = Mesure_E;
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"IPO")) 
		{
			int xdata indX = recherche_indice("X");
			int xdata indY = recherche_indice("Y");
			int xdata indA = recherche_indice("A");
			if ((indX >= 0) && (-99 <= cTOi(commande_a_decoder[indX+1])) && (cTOi(commande_a_decoder[indX+1] ) <= 99) &&
					(indY >= 0) && (-99 <= cTOi(commande_a_decoder[indY+1])) && (cTOi(commande_a_decoder[indY+1] ) <= 99) &&
					(indA >= 0) && (-180 <= cTOi(commande_a_decoder[indA+1])) && (cTOi(commande_a_decoder[indA+1]) <= 180))
					{
						commande.Pos_Coord_X = cTOi(commande_a_decoder[indX+1]);
						Coord_X_absolu = commande.Pos_Coord_X ;
						commande.Pos_Coord_Y = cTOi(commande_a_decoder[indY+1]);
						Coord_Y_absolu = commande.Pos_Coord_Y ;
						commande.Pos_Angle = cTOi(commande_a_decoder[indA+1]);
						Angle_relatif = commande.Pos_Angle ;
						Angle_absolu = Angle_relatif ; 
						return 1;
					}
		}
		else if(!strcmp(commande_a_decoder[0],"POS"))
		{
			/** RENVOYER LES VARIABLES ABSOLUES DEFINIES AVANT**/
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"MOU"))
		{
			commande.Etat_DCT_Obst = oui_180;
			commande.Type_DCT_Obst = DCT_unique;
			commande.DCT_Obst_Resolution = 30;
			if((!strcmp(commande_a_decoder[1],"D")))
			{
				commande.Etat_DCT_Obst = oui_360;
				return 1;
			}
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"MOB"))
		{
			int xdata indD = recherche_indice("D");
			int xdata indA = recherche_indice("A");
			commande.Etat_DCT_Obst = oui_360;
			commande.Type_DCT_Obst = DCT_balayage;
			commande.DCT_Obst_Resolution = 30;
			if(indD >= 0)
			{
				commande.Etat_DCT_Obst = oui_180;
				return 1;

			}
			if(indA >= 0 && (5 <= cTOi(commande_a_decoder[indA+1])) && (cTOi(commande_a_decoder[indA+1] ) <= 45))
			{
				commande.DCT_Obst_Resolution = commande_a_decoder[indA+1];
				return 1;
			}
			return 1 ;
		}
		else if(!strcmp(commande_a_decoder[0],"MOS"))
		{
			int xdata indD = recherche_indice("D");
			int xdata indA = recherche_indice("A");
			commande.Etat_DCT_Obst = oui_360;
			commande.Type_DCT_Obst = DCT_balayage_plus_proche;
			commande.DCT_Obst_Resolution = 30;
			if(indD >= 0)
			{
				commande.Etat_Mouvement = oui_180;
				return 1;
			}
			if(indA >= 0 && (5 <= cTOi(commande_a_decoder[indA+1])) && (cTOi(commande_a_decoder[indA+1] ) <= 45))
			{
				commande.DCT_Obst_Resolution = commande_a_decoder[indA+1];
				return 1;
			}
		}
		else if(!strcmp(commande_a_decoder[0],"SD"))
		{
			int xdata indF = recherche_indice("F");
			int xdata indP = recherche_indice("F");
			int xdata indW = recherche_indice("W");
			int xdata indB = recherche_indice("B");
			return 1;
			/**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**/
		}
		else if(!strcmp(commande_a_decoder[0],"L"))
		{
				int xdata indI = recherche_indice("I");
				int xdata indD = recherche_indice("D");
				int xdata indE = recherche_indice("E");
				int xdata indN = recherche_indice("N");
			  commande.Etat_Lumiere = Allumer;
				commande.Lumiere_Intensite = 100;
				commande.Lumiere_Duree = 99;
				commande.Lumiere_Extinction = 0;
				commande.Lumiere_Nbre = 1; 

				if (indI >= 0 && 1 <= cTOi(commande_a_decoder[indI+1]) && (cTOi(commande_a_decoder[indI+1]) <= 100)) 	
				{
					commande.Lumiere_Intensite = cTOi(commande_a_decoder[indI+1]);
					return 1;
				}
				if (indD >= 0 && 1 <= cTOi(commande_a_decoder[indD+1]) && (cTOi(commande_a_decoder[indD+1]) <= 99))
				{
					commande.Lumiere_Duree = cTOi(commande_a_decoder[indD+1]); 
					return 1;
				}
				if (indE >= 0 && 0<= (cTOi(commande_a_decoder[indE+1])) && (cTOi(commande_a_decoder[indE+1]) <= 99) )
				{
					commande.Lumiere_Extinction = cTOi(commande_a_decoder[indE+1]);
					return 1;
				}
				if(indN >= 0 && 1 <= cTOi(commande_a_decoder[indN+1]) && (cTOi(commande_a_decoder[indN+1]) <= 99))
				{
					commande.Lumiere_Nbre = cTOi(commande_a_decoder[indN+1]); 
					return 1;
				}
		}
		else if(!strcmp(commande_a_decoder[0],"LS"))
		{
				commande.Etat_Lumiere = Eteindre;
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"CS"))
		{
			int xdata indHV = recherche_indice("H") + recherche_indice("V") +1 ;
			int xdata indA = recherche_indice("A");
			commande.Etat_Servo = Servo_H;
			commande.Servo_Angle = 0;
			
			if(indHV >= 0 && !strcmp(commande_a_decoder[indHV],"H"))
			{
			commande.Etat_Servo = Servo_H;
			}
			if(indHV >= 0 && !strcmp(commande_a_decoder[indHV],"V"))
			{
			commande.Etat_Servo = Servo_V;
			}
			if(!strcmp(commande_a_decoder[indA],"A") && -90 <= (cTOi(commande_a_decoder[indA+1])) && (cTOi(commande_a_decoder[indA+1]) <= 90))
			{
			commande.Servo_Angle = (char)cTOi(commande_a_decoder[indA+1]);
				return 1;
			}
			return 1;
		}
		else if (!strcmp(commande_a_decoder[0],"PPH"))
		{	int xdata indOCS = recherche_indice("O") + recherche_indice("C") + recherche_indice("S") + 2 ;
			int xdata indE = recherche_indice("E");
			int xdata indN = recherche_indice("N");
			
			commande.Etat_Photo = Photo_1;
			commande.Photo_Duree = 1;
			
			if(!strcmp(commande_a_decoder[indOCS],"C"))
			{
			commande.Etat_Photo = Photo_continue;
			}
			if(!strcmp(commande_a_decoder[indOCS],"S"))
			{
			commande.Etat_Photo = Photo_Multiple;
			}
			if(indE >= 0 && 0 <= (cTOi(commande_a_decoder[indE+1])) && (cTOi(commande_a_decoder[indE+1]) <= 99))
			{
			commande.Photo_Duree = cTOi(commande_a_decoder[indE+1]);
			}
			if(indN >= 0 && 1 <= (cTOi(commande_a_decoder[indN+1])) && (cTOi(commande_a_decoder[indN+1]) <= 255))
			{
			commande.Photo_Duree = cTOi(commande_a_decoder[indN+1]);
				
			}
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"SPH"))
		{
			commande.Etat_Photo = Photo_stop;
			return 1;
		}
		else if(!strcmp(commande_a_decoder[0],"AUX"))
		{
			/**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**/
		}
		}
		serOutstring("\n\r#\n\r");
		return 0;
}














