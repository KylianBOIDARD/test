//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER SOURCE CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER SOURCE DU SOUS ENSEMBLE F0-M2
//		CE PROGRAMME CONFIGURE L'UART 1 ET SE CHARGE DE TRANSMETTRE LES COMMANDES
//		A LA CARTE SERIALIZER DANS SON FORMAT DE COMMUNICATION ET DE RECUPERER SES REPONSES
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include "FO-M2.h"

//-----------------------------------------------------------------------------
//---------------- Variables Globales -----------------------------------------
//-----------------------------------------------------------------------------
INFORMATIONS_SERIALIZER Reponse_Serial ;			//variable à renvoyer à FO-M6
INFORMATIONS_SERIALIZER empty = {0};
char command_to_transmit[40] = {0};						//variable contenant la commande a transmettre à Serializer
char reponse[40] ;														//variable contenant la reponse de Serializer
char Flag_TX = 0 ;														//flag de fin de transmission
char Reponse_Recue = 1 ;											//flag pour savoir si la reponse complete de Seria est recue
char Flag_RX = 0 ;														//flag de fin de reception

//-----------------------------------------------------------------------------
//---------- FONCTION INITIALISAION -----------------------------------
void Init_FOM2(void){
		//Init_Timer1();
		Init_UART1();
}
//---------- FONCTION PRINCIPALE DU SOUS ENSEMBLE -----------------------------
INFORMATIONS_SERIALIZER FO_M2(COMMANDES_SERIALIZER x){	

		Reponse_Serial = empty ; 
		create_command(x);									//on crée la commande qui correspond à celle demandée en parametre
		vider_chaine(reponse);							//on s'assure d'avoir une variable reponse vide pour acceuillir la nouvelle reponse à la commande envoyée
		send_command(command_to_transmit);	//on envoie la commande à Serializer
		wait_for_answer();									//on attend la reponse de Serializer si nécessaire
		create_infos() ;										//on transforme la reponse reçue dans le bon format
	
		return Reponse_Serial ;								//on retourne la variable de type INFORMATIONS_SERIALIZER à FO-M6
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//---------- Fonction de Config Timer1 ---------------------------------------
/*void Init_Timer1(void){
	
		TR1 = 1 ; 					//enable timer 1
		TMOD |= (1<<5) ;		//select mode 2 : timer 1 8bits auto reload
		TH1 = 0xFA ;				//set the reload value : 0xFF - 0xFA = 0x05 --> value for BAUD=19200
		TL1 = 0x05 ;
}*/
//----------------------------------------------------------------------
//---------- Config UART1 vers serializer -----------------------------
void Init_UART1(void){
		PCON |= (1<<4) ; 		//disable baude rate divide by two
		SCON1 = 0x50 ; 			//config uart1 in mode 1 : 8bit, and reception enable
		EIE2 |= (1<<6) ; 					//enable uart1 interrupt 
}
//-----------------------------------------------------------------------
//---------- Fonction Interrupt UART1 ----------------------------------
void Uart1_int(void) interrupt 20 {
		if ( SCON1 & (1<<0) ){				//on regarde s'il s'agit d'une reception
				recup_infos();						//on appelle la fonction pour recuperer le caractere reçu
				Flag_RX = 1 ;							// Flag de fin de reception d'un caractere
		}
		else{
			Flag_TX = 1 ;									//Flag de fin de transmission d'un caractere 
		}
		SCON1 &= 0xFC ;								//remise a zero du flag de transmission et de reception de l'uart1
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//--------- Création d'une commande au format Serializer --------------------------
void create_command(COMMANDES_SERIALIZER x){
		char i = 0 ;
	  char var[10] = {0} ;
		vider_chaine(command_to_transmit) ;
		Reponse_Serial.Etat_Response = Reponse_non ;		//on initialise la var qui permet de savoir si on attend une rep

		switch (x.Etat_Commande) {
				case Commande_non : ;
//---------- COMMANDE RESET -------------------------------------------------------------------------				
				case Reset :																		
						strcat(command_to_transmit,"reset\r");
						break;
//----------- COMMANDES GET ENCODEUR ------------------------------------------------------------------------				
				case Getenc_1 : 																
						strcat(command_to_transmit,"getenc 1\r");
						Reponse_Serial.Etat_Response = Rep_getenc ;
						break;
				case Getenc_2 : 
						strcat(command_to_transmit,"getenc 2\r");
						Reponse_Serial.Etat_Response = Rep_getenc ;
						break;
				case Getenc_1_2 : 
						strcat(command_to_transmit,"getenc 1 2\r");
						Reponse_Serial.Etat_Response = Rep_getenc ;
						break;
//------------ COMMANDES CLEAR ENCODEUR -----------------------------------------------------------------------				
				case Clrenc_1 : 
						strcat(command_to_transmit,"clrenc 1\r");
						break;
				case Clrenc_2 : 
						strcat(command_to_transmit,"clrenc 2\r");
						break;
				case Clrenc_1_2 : 
						strcat(command_to_transmit,"clrenc 1 2\r");
						break;
//------------- COMMANDES MOGO ----------------------------------------------------------------------				
				case mogo_1 : 				//ROUE DROITE 
						strcat(command_to_transmit,"mogo 1:");
						if (x.Vitesse_Mot1 < 0) {						//on regarde le signe + ou - 
								strcat(command_to_transmit,"-");															//si c'est - on le met devant la vitesse
								x.Vitesse_Mot1 = -x.Vitesse_Mot1 ;
						}
						sprintf(var, "%d", (int)x.Vitesse_Mot1 );
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,"\r");																	//ajout retour chariot 
						break;
				case mogo_2 : 
						strcat(command_to_transmit,"mogo 2:");
						if (x.Vitesse_Mot2 < 0 ) {																		//on regarde le signe + ou - 
								strcat(command_to_transmit,"-");															//si c'est - on le met devant la vitesse
								x.Vitesse_Mot2 = -x.Vitesse_Mot2 ;
						}
						sprintf(var, "%d", (int)x.Vitesse_Mot2 );
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,"\r");																	//ajout retour chariot 
						break;
				case mogo_1_2 : 
						strcat(command_to_transmit,"mogo 1:");														//On rentre la commande du moteur 1
						if (x.Vitesse_Mot1 < 0 ) {																		
								strcat(command_to_transmit,"-");															
								x.Vitesse_Mot1 = -x.Vitesse_Mot1 ;
						}
						sprintf(var, "%d", (int)x.Vitesse_Mot1 );
						strcat(command_to_transmit, var);
						strcat(command_to_transmit," 2:");														//On repete les operations pour le moteur 2
						if (x.Vitesse_Mot2 < 0 ) {																		
								strcat(command_to_transmit,"-");															
								x.Vitesse_Mot2 = -x.Vitesse_Mot2 ;
						}
						sprintf(var, "%d", (int)x.Vitesse_Mot2 );
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,"\r");																	
						break;
//------------ COMMANDES VPID -----------------------------------------------------------------------				
				case Vpid_set : 
						strcat(command_to_transmit,"vpid ") ;
						sprintf(var, "%d", x.Set_P) ;						//recup en chaine de car le param P
						strcat(command_to_transmit, var) ;			//on l'ajoute a la commande
						strcat(command_to_transmit, ":") ;
						sprintf(var, "%d", x.Set_I) ;						//recup en chaine de car le param I
						strcat(command_to_transmit, var) ;			//on l'ajoute a la commande
						strcat(command_to_transmit, ":") ;
						sprintf(var, "%d", x.Set_D) ;						//recup en chaine de car le param D
						strcat(command_to_transmit, var) ;			//on l'ajoute a la commande
						strcat(command_to_transmit, ":") ;
						sprintf(var, "%d", x.Set_L_A) ;						//recup en chaine de car le param L
						strcat(command_to_transmit, var) ;
						strcat(command_to_transmit, "\r") ;
						break;
				case Vpid_read : 
						strcat(command_to_transmit,"vpid\r");
						Reponse_Serial.Etat_Response = Rep_vpid ;
						break;
//------------- COMMANDES DIGO ----------------------------------------------------------------------				
				case digo_1 : 
						strcat(command_to_transmit,"digo 1 : ");
						sprintf(var, "%d", x.Ticks_mot1);
						strcat(command_to_transmit, var);
						strcat(command_to_transmit," : ");
						if (x.Vitesse_Mot1 < 0 ) {														//on regarde le signe + ou - 
								strcat(command_to_transmit,"-");															//si c'est - on le met devant la vitesse
								x.Vitesse_Mot1 = -x.Vitesse_Mot1 ;
						}
						sprintf(var, "%d", (int)x.Vitesse_Mot1 );
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,"\r");
						break;
				case digo_2 : 
						strcat(command_to_transmit,"digo 2 : ");
						sprintf(var, "%d", x.Ticks_mot2);
						strcat(command_to_transmit, var);
						strcat(command_to_transmit," : ");
						if (x.Vitesse_Mot2 < 0 ) {														//on regarde le signe + ou - 
								strcat(command_to_transmit,"-");															//si c'est - on le met devant la vitesse
								x.Vitesse_Mot2 = -x.Vitesse_Mot2 ;
						}
						sprintf(var, "%d", (int)x.Vitesse_Mot1 );
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,"\r");
						break;
				case digo_1_2 :
						strcat(command_to_transmit,"digo 1:");
						if (x.Ticks_mot1 < 0) {												//on regarde le signe + ou - 
								strcat(command_to_transmit,"-");															//si c'est - on le met devant la vitesse
								x.Ticks_mot1 = -x.Ticks_mot1 ;
						}
						sprintf(var, "%d", x.Ticks_mot1);
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,":");
						sprintf(var, "%d", (int)x.Vitesse_Mot1 );
						strcat(command_to_transmit, var);
				
						strcat(command_to_transmit," 2:");
						if (x.Ticks_mot2 < 0) {												//on regarde le signe + ou - 
								strcat(command_to_transmit,"-");															//si c'est - on le met devant la vitesse
								x.Ticks_mot2 = -x.Ticks_mot2 ;
						}
						sprintf(var, "%d", x.Ticks_mot2);
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,":");
						sprintf(var, "%d", (int)x.Vitesse_Mot2 );
						strcat(command_to_transmit, var);
						strcat(command_to_transmit,"\r");
						break;
//------------- COMMANDES DPID ----------------------------------------------------------------------				
				case Dpid_set : 
						strcat(command_to_transmit,"dpid ") ;
						sprintf(var, "%d", x.Set_P) ;						//recup en chaine de car le param P
						strcat(command_to_transmit, var) ;			//on l'ajoute a la commande
						strcat(command_to_transmit, ":") ;
						sprintf(var, "%d", x.Set_I) ;						//recup en chaine de car le param I
						strcat(command_to_transmit, var) ;			//on l'ajoute a la commande
						strcat(command_to_transmit, ":") ;
						sprintf(var, "%d", x.Set_D) ;						//recup en chaine de car le param D
						strcat(command_to_transmit, var) ;			//on l'ajoute a la commande
						strcat(command_to_transmit, ":") ;
						sprintf(var, "%d", x.Set_L_A) ;						//recup en chaine de car le param A
						strcat(command_to_transmit, var) ;
						strcat(command_to_transmit, "\r") ;
						break;
				case Dpid_read : 
						strcat(command_to_transmit,"dpid\r") ;
						Reponse_Serial.Etat_Response = Rep_dpid ;
						break;
//------------- COMMANDE RPID STINGER MODE ----------------------------------------------------------------------				
				case Rpid_Stinger : 
						strcat(command_to_transmit,"rpid s\r") ;
						break;
//------------- COMMANDE PIDS ----------------------------------------------------------------------
				case Pids : 
						strcat(command_to_transmit,"pids\r");
						Reponse_Serial.Etat_Response = Rep_pids ;
						break;
//------------ COMMANDE STOP -----------------------------------------------------------------------				
				case Stop : 
						strcat(command_to_transmit,"stop\r");
						break;
//------------- COMMANDE VELOCITY ----------------------------------------------------------------------				
				case Vel : 
						strcat(command_to_transmit,"vel\r");
						Reponse_Serial.Etat_Response = Rep_vel ;
						break;
//------------ COMMANDE rESTORE -----------------------------------------------------------------------				
				case Restore : 
						strcat(command_to_transmit,"restore\r");
						break;
		}
}
//----------------------------------------------------------------------------------------------				
//---------- Fonction d'envoi de la commande à la carte Serializer -----------------------------
void send_command(char *x) {
		char i = 0 ;
		while (*(x+i) != '\0') {			//on parcourt la chaine tant que le caractère reçu n'est pas le caractere de fin \0 
				SBUF1 = *(x+i) ;					//on charge le buffer de l'UART avec le caractere
				i++ ;	
				while ( Flag_TX == 0){}		//on attends que le flag de transmission soit set par l'uart
				Flag_TX = 0 ;
		}
}
//---------------------------------------------------------------------------------------
//---------------- Fonction d'attente de reception de la reponse de Serializer --------------
void wait_for_answer(void){				//on verifie maintenant si on attend une reponse ou non pour la commande envoyée
		//if ( Reponse_Serial.Etat_Response != Reponse_non ){			//on attend une reponse
				while( Reponse_Recue != 0 ){}						//la boucle permet de laisser le temps aux interruptions d'intervenir
				Reponse_Recue = 1 ;
		//}
}
//-------------------------------------------------------------------------------------------------
//----------- Fonction récupération informations Serializer ---------------------------------------------				
void recup_infos(void){
		if ( SBUF1 == '\r' && strlen(reponse) != 0 ){
					//Reponse_Recue = 0 ;
		}
		else if ( SBUF1 != '\r' && SBUF1 != '\n' ){
					reponse[strlen(reponse)] = SBUF1 ;									
		}
		if (SBUF1 == '>'){
				Reponse_Recue = 0 ;
		}
}
//---------------------------------------------------------------------------------
//--------- Fonction de creation de la variable information pour FO-M6 -----------
void create_infos(void){
	char i = 0 , j = 0 ;
	char var[6] = {0};

	switch (Reponse_Serial.Etat_Response) {
			  default : 
						break ;
//----------------------------------------------------------------------			
				case Rep_pids :
						*reponse -= 0x30 ;
						Reponse_Serial.Read_Pids = *reponse ;
						break;
//------------------------------------------------------------------------
				case Rep_vel :
						while (reponse[i] != ' '){		//lecture et recuperation de la vitesse du moteur 1
							var[j] = reponse[i] ;
							i++; j++;
						}
						Reponse_Serial.Read_Vitesse_mot1 = atoi(var);		//on met la valeur dans le parametre reponse 
						vider_chaine(var); j = 0 ;
						//i est a la position du dernier chiffre de la vitesse
						while (i < strlen(reponse)){
							var[j] = reponse[i] ;
							i++; j++;
						}
						Reponse_Serial.Read_Vitesse_mot2 = atoi(var);		//on met la valeur dans le parametre reponse 
						break;
//--------------------------------------------------------------------------
				case Rep_dpid :	
						while ( i < strlen(reponse) ){		//on veut parcourir toute la chaine
							if( reponse[i] == 'P' ){								//si on tombe sur 'P'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != ' ' ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_P = atoi(var) ;
									vider_chaine(var); j=0;
							}
							if( reponse[i] == 'I' ){										//si on tombe sur 'I'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != ' ' ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_I = atoi(var) ;
									vider_chaine(var); j=0;
							}
							if( reponse[i] == 'D' ){										//si on tombe sur 'D'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != ' ' ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_D = atoi(var) ;
									vider_chaine(var); j=0;
							}
							if( reponse[i] == 'A' ){										//si on tombe sur 'A'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != 0 ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_L_A = atoi(var) ;
									vider_chaine(var); j=0;
							}
						i++;
						}
						break ;
//--------------------------------------------------------------------------
				case Rep_vpid :
						while ( i < strlen(reponse) ){		//on veut parcourir toute la chaine
							if( reponse[i] == 'P' ){								//si on tombe sur 'P'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != ' ' ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_P = atoi(var) ;
									vider_chaine(var); j=0;
							}
							if( reponse[i] == 'I' ){										//si on tombe sur 'I'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != ' ' ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_I = atoi(var) ;
									vider_chaine(var); j=0;
							}
							if( reponse[i] == 'D' ){										//si on tombe sur 'D'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != ' ' ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_D = atoi(var) ;
									vider_chaine(var); j=0;
							}
							if( reponse[i] == 'L' ){										//si on tombe sur 'L'
									i+=2 ;		//on elimine le  ':'
									while ( reponse[i] != 0 ){
											var[j] = reponse[i] ;
											i++; j++;
									}
									Reponse_Serial.Read_L_A = atoi(var) ;
									vider_chaine(var); j=0;
							}
						i++;
						}
						break ;
//--------------------------------------------------------------------------
				case Rep_getenc :
						while (reponse[i] != ' '){		//lecture et recuperation de la valeur de l'encodeur 1
							var[j] = reponse[i] ;
							i++; j++;
						}
						Reponse_Serial.Read_Val_enc_1 = atoi(var);		//on met la valeur dans le parametre reponse 
						vider_chaine(var); j = 0 ;
						//i est a la position du dernier chiffre de la valeur
						while (i < strlen(reponse)){
							var[j] = reponse[i] ;
							i++; j++;
						}
						Reponse_Serial.Read_Val_enc_2 = atoi(var);		//on met la valeur dans le parametre reponse 
						break;		
		}
}
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------







