//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER SOURCE CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER SOURCE DU SOUS ENSEMBLE F0-M6
//		
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include "FO-MX.h"

extern COMMANDES commande ;
INFORMATIONS infos ;
extern vitesse_tempo ;
extern Coord_X_absolu ;
extern Coord_Y_absolu ;
extern Angle_absolu ;
extern Angle_relatif ;
const char taille_tab_mesures = 20 ;
int tab_mesures[20] = {0} ; //initialise un tableau de distance
char nb_mesures = 0 ;		//initialise le nb de mesures dans la tableau

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//--------- Fonction principale -----------------------------------------------
void FO_M6( void ){
	
			if ( commande.Etat_Mouvement != Mouvement_non ){		//si la commande demande un mouvement
					INFORMATIONS_SERIALIZER infos_Seria ;
					infos_Seria = transform_command_Seria(commande) ;			//on crée une variable de commande compréhensible à envoyer à FO_M2
					commande.Etat_Mouvement = Mouvement_non ;
			}
			if ( commande.Etat_DCT_Obst != DCT_non ){						//si elle demande une detection d'obstacle
					command_Telemetre();
				
			}
			if ( commande.Etat_Servo != BUT_Servo_non ){		//demande un mouvement du servomoteur
				unsigned char duree_depl;
				switch (commande.Etat_Servo){
        	case Servo_H :
						duree_depl = FO_M3((int)commande.Servo_Angle+90 );	//marche pas car durée en float
        		infos.Etat_BUT_Servo = BUT_Servo_H ;
						break;
        	case Servo_V:
        		break;
        	default:
        		break;
        }
				commande.Etat_Servo = BUT_Servo_non ;
			}

	
			
}


//------------------------------------------------------------------------------
//---------------- Fonction transformation commandes pour Serializer------------
//------------------------------------------------------------------------------

INFORMATIONS_SERIALIZER transform_command_Seria( COMMANDES command ){
		COMMANDES_SERIALIZER x ;				//INITIALISE LA VARIABLE
		INFORMATIONS_SERIALIZER y ;
		
		char norme_distance = 0 ;
		int alpha = 0 ;
	
		switch (command.Etat_Mouvement) {
			default : break;	
			case Avancer :
						//parametre Vitesse entre 5 et 100% --> il faut transformer la vitesse en RPM, max RPM = 160
						if ( vitesse_tempo != 0 ){			
								//il faut mettre à la valeur par defaut = 20%
								x.Vitesse_Mot1 = 160*((float)vitesse_tempo/100) ;
								x.Vitesse_Mot2 = 160*((float)vitesse_tempo/100) ;
								vitesse_tempo = 0 ;
						} 
						else {														//sinon on mets à la valeur du param
								x.Vitesse_Mot1 = 160*((float)command.Vitesse/100) ;
								x.Vitesse_Mot2 = 160*((float)command.Vitesse/100) ;
						}
						x.Etat_Commande = mogo_1_2 ;
						y = FO_M2(x) ;
						break;
//-------------------------------------------------------------------------------------------------						
			case Reculer :
						//parametre Vitesse entre 5 et 100% --> il faut transformer la vitesse en RPM ??
						if ( vitesse_tempo != 0 ){			
								//il faut mettre à la valeur par defaut = 20%
								x.Vitesse_Mot1 = -160*((float)vitesse_tempo/100) ;
								x.Vitesse_Mot2 = -160*((float)vitesse_tempo/100) ;
								vitesse_tempo = 0 ;
						} 
						else {														//sinon on mets a la valeur du param
								x.Vitesse_Mot1 = -160*((float)command.Vitesse/100) ;
								x.Vitesse_Mot2 = -160*((float)command.Vitesse/100) ;
						}
						x.Etat_Commande = mogo_1_2 ;
						y = FO_M2(x) ;
						break;
//-------------------------------------------------------------------------------------------------			
			case Stopper :
					x.Etat_Commande = Stop ;
					y = FO_M2(x) ;
					break;
//--------------------------------------------------------------------------			
			case Rot_90G :		//on fait tourner Mot1(droite) en positif et Mot2 en negatif
					//il faut lancer une commade digo sur une distance = 1/4 de cercle = 218mm
					//on a pour notre robot : 3.312ticks/mm  parcouru --> convertir la distance en encodeur ticks --> 218mm = 722,016 ticks
					x.Vitesse_Mot1 = 160*0.05 ;		//on initialise a la vitesse par defaut
					x.Vitesse_Mot2 = 160*0.05 ;		//il faut verifier quel moteur est le droit et quel moteur est le gauche
					x.Ticks_mot1 = abs( ((139*2*PI)/4)*3.312 )*0.7 ;					//on lance la commande digo sur la bonne distance, 139=rayon 
					x.Ticks_mot2 = -abs( ((139*2*PI)/4)*3.312 )*0.7 ;
					x.Etat_Commande = digo_1_2 ;
					y = FO_M2(x) ;
					break ;
			
			case Rot_90D :		//on fait tourner Mot2 en positif et Mot1 en negatif
					x.Vitesse_Mot1 = 160*0.05 ;		
					x.Vitesse_Mot2 = 160*0.05 ;		
					x.Ticks_mot1 = -abs( ((139*2*PI)/4)*3.312 )*0.7 ;					
					x.Ticks_mot2 = abs( ((139*2*PI)/4)*3.312 )*0.7 ;
					x.Etat_Commande = digo_1_2 ;
					y = FO_M2(x) ;
					break ;

			case Rot_180D :		//on fait tourner Mot2 en positif et Mot1 en negatif
					//il faut lancer une commade digo sur une distance = 1/2 de cercle = 436mm
					//on a pour notre robot : 3.312ticks/mm  parcouru --> convertir la distance en encodeur ticks --> 436mm = 1444 ticks
					x.Vitesse_Mot1 = 160*0.05 ;		
					x.Vitesse_Mot2 = 160*0.05 ;		
					x.Ticks_mot1 = -abs( ((139*2*PI)/2)*3.312 )*0.7 ;					
					x.Ticks_mot2 = abs( ((139*2*PI)/2)*3.312 )*0.7 ;
					x.Etat_Commande = digo_1_2 ;
					y = FO_M2(x) ;
					break ;
			case Rot_180G :		//on fait tourner Mot1 en positif et Mot2 en negatif
					x.Vitesse_Mot1 = 160*0.05 ;		
					x.Vitesse_Mot2 = 160*0.05 ;		
					x.Ticks_mot1 = abs( ((139*2*PI)/2)*3.312 )*0.7 ;					
					x.Ticks_mot2 = -abs( ((139*2*PI)/2)*3.312 )*0.7 ;
					x.Etat_Commande = digo_1_2 ;
					y = FO_M2(x) ;
					break ;
			case Rot_AngD :
					x.Vitesse_Mot1 = 160*0.05 ;		
					x.Vitesse_Mot2 = 160*0.05 ;		
					x.Ticks_mot1 = -abs( ((139*2*PI)*(float)command.Angle/360 )*3.312 )*0.7 ;					
					x.Ticks_mot2 = abs( ((139*2*PI)*(float)command.Angle/360 )*3.312 )*0.7 ;
					x.Etat_Commande = digo_1_2 ;
					y = FO_M2(x) ;
					break;
			case RotAngG :
					x.Vitesse_Mot1 = 160*0.05 ;		
					x.Vitesse_Mot2 = 160*0.05 ;		
					x.Ticks_mot1 = abs( (139*2*PI)*( (float)command.Angle/360 )*3.312 )*0.7 ;					
					x.Ticks_mot2 = -abs( (139*2*PI)*( (float)command.Angle/360 )*3.312 )*0.7 ;
					x.Etat_Commande = digo_1_2 ;
					y = FO_M2(x) ;
					break;
//------------------------------------------------------------------------------------
			case Depl_Coord :
//----------- CALCUL DE L'ANGLE DE DEPLACEMENT -----------------------------------
					alpha = (180/PI)*atan2( command.Coord_X, command.Coord_Y );	//calcul de l'angle de triangle de déplacement au format trigo en degré
					//alpha = alpha - Angle_relatif ; //convertion pour avoir l'angle réel de rotation par rapport à l'orientation de la base
					x.Vitesse_Mot1 = 160*0.05 ;			//on initialise la vitesse	
					x.Vitesse_Mot2 = 160*0.05 ;			//qui sera de 5% pour tout le déplacement
					if(alpha < 0 ){		//si alpha négatif donc rotation à droite	
							x.Ticks_mot1 = -abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;					
							x.Ticks_mot2 = abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;
							x.Etat_Commande = digo_1_2 ;
							FO_M2(x) ;
					}
					else{							// si alpha positif rotation a gauche 
							x.Ticks_mot1 = abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;					
							x.Ticks_mot2 = -abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;
							x.Etat_Commande = digo_1_2 ;
							FO_M2(x) ;
					}
					x.Etat_Commande = Pids ;
					while (FO_M2(x).Read_Pids != 0 ){} ;  //on attend la fin du dep
//----------- CALCUL DE LA NORME DE LA DISTANCE A PARCOURIR -----------------------
					norme_distance = (char)sqrt(pow(command.Coord_X,2) + pow(command.Coord_Y,2) ) ;
					x.Ticks_mot1 = abs( (norme_distance*100)*3.312 )*0.85 ;				//on calcul la distance a parcourir	en mm puis en tick		
					x.Ticks_mot2 = abs( (norme_distance*100)*3.312 )*0.85 ;				//on la mets dans la variable Ticks_mot
					Coord_X_absolu += command.Coord_X ;		//mise à jour de la position 
					Coord_Y_absolu += command.Coord_Y ;		//absolue de la base dans le repère
					x.Etat_Commande = digo_1_2 ;
					FO_M2(x) ;		//on parcourt la distance 
					x.Etat_Commande = Pids ;
					while (FO_M2(x).Read_Pids != 0 ){} ;  //on attend la fin du dep
//----------- ORIENTATION FINALE DE LA BASE SELON L'ANGLE DESIRE -------------------- 
					alpha = commande.Angle - alpha ;	//calcul angle de déplacement pour arriver à l'orientation finale demandée
					//Angle_relatif = commande.Angle ; 	//on enregistre l'orientation de la base pour un prochain déplacement
					if(alpha < 0 ){		//si alpha négatif donc rotation à droite	
							x.Ticks_mot1 = -abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;					
							x.Ticks_mot2 = abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;
							x.Etat_Commande = digo_1_2 ;
							y = FO_M2(x) ;
					}
					else{							// si alpha positif rotation a gauche
							x.Ticks_mot1 = abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;					
							x.Ticks_mot2 = -abs( ((139*2*PI)*(float)alpha/360 )*3.312 )*0.7 ;
							x.Etat_Commande = digo_1_2 ;
							y = FO_M2(x) ;
					}
					x.Etat_Commande = Pids ;
					while (FO_M2(x).Read_Pids != 0 ){} ;  //on attend la fin du dep
					break;
	}
	return y ;
}
//------------------------------------------------------------------------------
//---------------- Fonction transformation informations pour PC de controle ----
//------------------------------------------------------------------------------
/*
INFORMATIONS transform_infos( INFORMATIONS_SERIALIZER info_seria ){
		INFORMATIONS x ;
		switch (commande.Etat_Mouvement){
    	default:
    		break;
    }
	
	
	
		return x;
}*/
//------------------------------------------------------------------------------
//---------------- Fonction transition commande Télémetres --------------------
//------------------------------------------------------------------------------
void command_Telemetre( void ){
		char i = 0;
		int temps_dep = 0 ;
		int mesure = 0 ;	//initialise une variable pour stocker la distance de l'obstacle	
		char angle = -90 ;  //angle de départ pour detect par balayage
		nb_mesures = 0 ;
		for(i;i<taille_tab_mesures;i++){
			tab_mesures[i] = 0 ;		//on vide le tableau des mesures précédentes
    }
		
		switch (commande.Etat_DCT_Obst){
			case oui_180:
				if(commande.Type_DCT_Obst == DCT_unique){	
						mesure = FO_M4(1) ;		//on demande une detection avant
						tab_mesures[nb_mesures] = mesure ;	//on stocke la detection
						nb_mesures += 1 ;		//on incrémente le nb de mesures
				}
				if(commande.Type_DCT_Obst == DCT_balayage){
						while (angle < 90){
							temps_dep = FO_M3(angle) ;
							Delay(temps_dep);		//attend que le servo soit en position
							mesure = FO_M4(1) ;		//on demande une detection avant
							tab_mesures[nb_mesures] = mesure ;	//on stocke la detection
							nb_mesures += 1 ;		//on incrémente le nb de mesures
							angle += commande.DCT_Obst_Resolution ;	//on incrémente l'angle	
            }
				}
				if(commande.Type_DCT_Obst == DCT_balayage_plus_proche){
				
				}
						//redirection de la structure infos vers le tableau de valeurs
				infos.Tab_Val_Obst = &tab_mesures ;		//on fait pointer sur l'adresse de notre tableau de mesures
				infos.Nbre_Val_obst = nb_mesures ;		//on set le nbr de mesures effectuées
				infos.Etat_DCT_Obst = DCT_Obst_180_oui ;
				break;
			case oui_360:
				if(commande.Type_DCT_Obst == DCT_unique){	
						mesure = FO_M4(1) ;		//on demande une detection avant
						tab_mesures[nb_mesures] = mesure ;	//on stocke la detection
						nb_mesures += 1 ;		//on incrémente le nb de mesures
					
						mesure = FO_M4(2) ;		//on demande une detection arrière
						tab_mesures[nb_mesures] = mesure ;	//on stocke la detection
						nb_mesures += 1 ;		//on incrémente le nb de mesures
				}
				if(commande.Type_DCT_Obst == DCT_balayage){
						//meme principe que 180 en ajoutant detection arriere
				}
				if(commande.Type_DCT_Obst == DCT_balayage_plus_proche){
				
				}
						//redirection de la structure infos vers le tableau de valeurs
				infos.Tab_Val_Obst = &tab_mesures ;		//on fait pointer sur l'adresse de notre tableau de mesures
				infos.Nbre_Val_obst = nb_mesures ;		//on set le nbr de mesures effectuées
				infos.Etat_DCT_Obst = DCT_Obst_360_oui ;
				break;
			default:
				break;
		}
}


