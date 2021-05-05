//-----------------------------------------------------------------------------
// 				PROJET TRONC COMMUN 
//				FICHIER SOURCE CARTE MASTER
//-----------------------------------------------------------------------------
// AUTH: Equipe A4 -- CPE LYON -- 2021
// DATE: 24/02/2021
//-----------------------------------------------------------------------------
//				FICHIER SOURCE DU SOUS ENSEMBLE F0-M1
//		
//-----------------------------------------------------------------------------
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------

#include <c8051f020.h>                    
#include <stdio.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>
#include "FOM1.h"


// Structure buffer circulaire
// buffer_start d�but du buffer
// buffer_end fin du buffer
// buffer_in adresse premier �lement
// buffer_out adresse dernier �l�ment

#define RB_CREATE(rb, type) \
   struct { \
     type *rb_start; \	   
     type *rb_end; \	   
     type *rb_in; \
	 type *rb_out; \		
	  } rb
	 
// initialisation des pointeurs du buffer
#define RB_INIT(rb, start, number) \
         ( (rb)->rb_in = (rb)->rb_out= (rb)->rb_start= start, \
           (rb)->rb_end = &(rb)->rb_start[number] )

//buffer ciruculaire
#define RB_SLOT(rb, slot) \
         ( (slot)==(rb)->rb_end? (rb)->rb_start: (slot) )

// buffer vide
#define RB_EMPTY(rb) ( (rb)->rb_in==(rb)->rb_out )

// buffer plein
#define RB_FULL(rb)  ( RB_SLOT(rb, (rb)->rb_in+1)==(rb)->rb_out )

// Incrementation du pointeur du buffer de lecture
#define RB_PUSHADVANCE(rb) ( (rb)->rb_in= RB_SLOT((rb), (rb)->rb_in+1) )
		
// Incrementation du pointeur du buffer d'�criture
#define RB_POPADVANCE(rb)  ( (rb)->rb_out= RB_SLOT((rb), (rb)->rb_out+1) )

// Pointeur pour stocker une valeur dans le buffer
#define RB_PUSHSLOT(rb) ( (rb)->rb_in )

// pointeur pour lire une valeur dans le buffer
#define RB_POPSLOT(rb)  ( (rb)->rb_out )


//*************************************************************************************************

#define       MAX_BUFLEN 80
static char  xdata outbuf[MAX_BUFLEN];     /* memory for ring buffer #1 (TXD) */
static char  xdata inbuf [MAX_BUFLEN];     /* memory for ring buffer #2 (RXD) */
static  bit   TXactive = 0;             /* transmission status flag (off) */



/* define o/p and i/p ring buffer control structures */
static RB_CREATE(out,unsigned char xdata);            /* static struct { ... } out; */
static RB_CREATE(in, unsigned char xdata);            /* static struct { ... } in; */

 char c;

extern char xdata information_encoder[];
extern vitesse_temp ;
// **************************************************************************************************
// MAIN
// **************************************************************************************************

//*************************************************************************************************
//  CONFIGURATION BAS NIVEAU de L'UART0
//*************************************************************************************************

//*****************************************************************************
#define Preload_Timer0 (SYSCLK/(BAUDRATE*16))
#if Preload_Timer0 > 255 
#error "Valeur Preload Timer0 HORS SPECIFICATIONS"
#endif 
//*****************************************************************************	 
//cfg_Clock_UART
//	Utilisation du Timer 1
//*****************************************************************************	 
void cfg_Clock_UART(void)
{
  CKCON |= 0x10;      // T1M: Timer 1 use the system clock.
  TMOD |= 0x20;       //  Timer1 CLK = system clock
	TMOD &= 0x2f;			  // Timer1 configur� en timer 8 bit avec auto-reload	
	TF1 = 0;				  // Flag Timer effac�

	TH1 = -(Preload_Timer0);
	ET1 = 0;				   // Interruption Timer 1 d�valid�e
	TR1 = 1;				   // Timer1 d�marr�
}
 
//*****************************************************************************	 
//CFG_uart0_mode1
//
//*****************************************************************************	 
void cfg_UART0_mode1(void)
{
		RCLK0 = 0;     // Source clock Timer 1
		TCLK0 = 0;
		PCON  |= 0x80; //SMOD0: UART0 Baud Rate Doubler Disabled.
		PCON &= 0xBF;  // SSTAT0=0
		SCON0 = 0x70;   // Mode 1 - Check Stop bit - Reception valid�e
		TI0 = 1;        // Transmission: octet transmis (pr�t � recevoir un char
					          // pour transmettre			
    ES0 = 1;        // interruption UART0 autoris�e	
}


void Init_FOM1(void)
{
	cfg_Clock_UART();
	cfg_UART0_mode1();
	init_Serial_Buffer();
	//EA = 1;
	init_commande();
	serOutstring("\n\rLancement du programme\n\r");
}	


void UART0_ISR(void) interrupt 0x4 {
 
//	static unsigned int cp_tx = 0;
//  static unsigned int cp_rx = 0;
	
  if (TI0==1) // On peut envoyer une nouvelle donn�e sur la liaison s�rie
  { 
  	if(!RB_EMPTY(&out)) {
       SBUF0 = *RB_POPSLOT(&out);      /* start transmission of next byte */
       RB_POPADVANCE(&out);            /* remove the sent byte from buffer */
//			 cp_tx++;
  	}
  	else TXactive = 0;                 /* TX finished, interface inactive */
	TI0 = 0;
  }
  else // RI0 � 1 - Donc une donn�e a �t� re�ue
  {
		if(!RB_FULL(&in)) {                   // si le buffer est plein, la donn�e re�ue est perdue
     	*RB_PUSHSLOT(&in) = SBUF0;        /* store new data in the buffer */
		  RB_PUSHADVANCE(&in);               /* next write location */
//		  cp_rx++;
	 }
   RI0 = 0;
  }
}

// **************************************************************************************************
// init_Serial_Buffer: Initialisation des structuresde gestion des buffers transmission et reception
// *************************************************************************************************
//**************************************************************************************************
void init_Serial_Buffer(void) {

    RB_INIT(&out, outbuf, MAX_BUFLEN-1);           /* set up TX ring buffer */
    RB_INIT(&in, inbuf,MAX_BUFLEN-1);             /* set up RX ring buffer */

}
// **************************************************************************************************
// SerOutchar: envoi d'un caract�re dans le buffer de transmission de la liaison s�rie
// *************************************************************************************************
unsigned char serOutchar(char c) {

  if(!RB_FULL(&out))  // si le buffer n'est pas plein, on place l'octet dans le buffer
  {                 
  	*RB_PUSHSLOT(&out) = c;               /* store data in the buffer */
  	RB_PUSHADVANCE(&out);                 /* adjust write position */

  	if(!TXactive) {
		TXactive = 1;                      /* indicate ongoing transmission */
 	  TI0 = 1;//   Placer le bit TI � 1 pour provoquer le d�clenchement de l'interruption
  	}
	return 0;  // op�ration correctement r�alis�e 
  }
   else return 1; // op�ration �chou�e - Typiquement Buffer plein
}
// ************************************************************************************************
//  serInchar: 	lecture d'un caract�re dans le buffer de r�ception de la liaison s�rie
//  Fonction adapt�e pour la r�ception de codes ASCII - La r�ception de la valeur binaire 0
//  n'est pas possible (conflit avec le code 0 retourn� si il n'y a pas de caract�re re�u)
// ************************************************************************************************
char serInchar(void) {
char c;

  if (!RB_EMPTY(&in))
  {                 /* wait for data */

  	c = *RB_POPSLOT(&in);                 /* get character off the buffer */
 	  RB_POPADVANCE(&in);                   /* adjust read position */
  	return c;
  }
		else return 0;
}
// ************************************************************************************************
//  serInchar_Bin: 	lecture d'un caract�re dans le buffer de r�ception de la liaison s�rie
//  Fonction adapt�e pour la r�ception de codes Binaires - Cette fonction retourne un entier. 
//  L'octet de poids faible correspond au caract�re re�u, l'octet de poids fort, s'il est nul indique 
//  qu'aucun caract�re n'a �t� re�u.
//  
// ************************************************************************************************
/*unsigned int serInchar_Bin(void) {
char c;
unsigned int return_code = 0;
	 
  if (!RB_EMPTY(&in))
  {                
    // un caract�re au moins est dans le buffer de r�ception
  	c = *RB_POPSLOT(&in);                 // get character off the buffer 
 	  RB_POPADVANCE(&in);                   // adjust read position 
  	return 0xFF00+c;
  }
	// pas de caract�re dans le buffer de r�ception.
  else return return_code;
}*/
// *************************************************************************************************
// serOutstring:  Envoi d'une chaine de caract�re dans le buffer de transmission
// ************************************************************************************************
unsigned char serOutstring(char *buf) {
unsigned char len,code_err=0;

  for(len = 0; len < strlen(buf); len++)
     code_err +=serOutchar(buf[len]);
  return code_err;
}


	char commande_recu[30]; 
	int i = 0;


void FOM1(void){
	int j = 0;
	char c;
	c = serInchar();
	if (c != 0x00 && c!=0x0D && c!=0x0A)
	{
				serOutchar(c);

			commande_recu[i] = c;
			i++;
	}
	
	if (strlen(commande_recu) != 0 && c== 0x0D)
	{
			commande_recu[i] = '\0';
			//serOutchar(strlen(commande_recu));
			serOutstring("\n\rCommande recu: \n\r");
			serOutstring(commande_recu);
			serOutstring("\n\r");
		//serOutstring("\n\r Traitement \n\r");
			decoupage_caractere_message(commande_recu);
			if (traitement_commande()){
				traitement_information();
				FO_M6();
				serOutstring("\r\n>");
				//EA = 0;
				// Envoie de l'ordre au serializer � ajouter
				//EA = 1;
				//serOutstring(information_encoder);
			}
			else {
				
				serOutstring("\r\n#");
			}
			
			clear_commande();
			 i = 0;
			strcpy(commande_recu,"/0");
	}
}





/*
void main (void) {
	
		char test [] = "\n\rLancement du programme\n\r";

	  Init_FOM1();  // Appel des configurations globales
	  init_commande();
		init_struct_information();
	
		serOutstring(\n\rLancement du programme\n\r);
		//Putchar('a');
// Fin Code phase D�marrage	***************************************************
	//Send_String ("G X:5 Y:8 A:121");
	//decoupage_caractere_message("E");
	//traitement_commande();
	//clear_commande();
	//decoupage_caractere_message("G X:5 Y:8 A:121");
	//traitement_commande();
	
	//traitement_commande();
	      while(1)
        {
					FOM1();
        }
					
}*/