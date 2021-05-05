//-----------------------------------------------------------------------------
// TP4_Main.c
//-----------------------------------------------------------------------------
// AUTH: Martin - Mouna
// DATE: 31/03/2021
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include <C8051F020.h>
#include "c8051F020_SFR16.h"
#include <string.h>
#include <math.h>
//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
#define PWM_START 0x4000 // starting value for the PWM
// high time
sbit PWM_OUT = P0^0; // define PWM output port pin

// Prototypes de Fonctions
void configPCA (void);
void PCA_ISR (void); // PCA Interrupt Service Routine
unsigned char CDE_Servo_V (char Angle);
void Init_FF_S3();
char FF_S3(char Angle);

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
unsigned PWM = PWM_START; // Number of PCA clocks for waveform to be high
// duty cycle = PWM / 65536 Note: this is a 16-bit value
float AncienAngle = 0;
float AngleConvert=10;	
float temps_deplacement;
float temps_rotation;
float dutycycle;

//----- FONCTION INITIALISATION -----------------------------------------------
void Init_FF_S3(void){
	configPCA();
	PWM_OUT=0;
}
//-----------------------------------------------------------------------------

//---- FONCTION PRINCIPALE DE FF-S3 -------------------------------------------
char FF_S3 (char Angle){
		//Recoit l'angle en ASCII, retourne le temps mis par le servomoteur pour effectuer la rotation
		return CDE_Servo_V(Angle);
}

//-----------------------------------------------------------------------------
// CDE_Servo_H
//-----------------------------------------------------------------------------

unsigned char CDE_Servo_V (char Angle){ //Angle ira de -90° à 90°
	dutycycle = ( (Angle*0.01)+0.6 )/35.4 ;	// HAUT/Periode
	PWM=(dutycycle)*65536;

	temps_deplacement = 0.21 * ( sqrt(pow(Angle,2)) - AncienAngle ) / 60; //En secondes, 0.21 = 0.21 sec/60° du servomoteur HS-422
	temps_deplacement = temps_deplacement*1000 ;		//convertion en ms
	AncienAngle = Angle;
	
	return (char)temps_deplacement;//On retourne le temps que va mettre le servomoteur à se déplacer
}
//-----------------------------------------------------------------------------

//---- configPCA --------------------------------------------------------------
void configPCA (void) {
//Gestion crossbar
XBR0 = 0x08; // enable CEX0 at P0.0

// configuration du PCA
PCA0MD = 0x00; // System clock divided by 12
	
// PCA time base = SYSCLK / 12 donc clk externe, période 35.5ms
PCA0CPL0 = (0xFF & (PWM<<8)); //initialize PCA compare value
PCA0CPH0 = (0xFF & (PWM>>8));
PCA0CPM0 = 0x4D; // CCM0 in High Speed output mode

//Gestion interruptions
EIE1 |= 0x08; // enable PCA interrupts
PCA0CN = 0x40; // enable PCA counter

}

// pull-ups
//P0MDOUT |= 0x01; // set P0.0 output state to push-pull
//P1MDOUT |= 0x20; // set P1.6 output to push-pull (LED)

//-----------------------------------------------------------------------------
// PCA_ISR
//-----------------------------------------------------------------------------
//
// This ISR is called when the PCA CCM0 obtains a match
// PWM_OUT is the CEX0 port pin that holds the state of the current edge:
// 1 = rising edge; 0 = falling edge
// On the rising edge, the compare registers are loaded with PWM_HIGH.
// On the falling edge, the compare registers are loaded with zero.
//
void PCA_ISR (void) interrupt 9
{

if (CCF0) {
	CCF0 = 0; // clear compare indicator
	if (PWM_OUT) { // process rising edge
		PCA0CPL0 = (0xFF & (PWM<<8)); //set next match to PWM
		PCA0CPH0 = (0xFF & (PWM>>8));
		PWM_OUT=1;
	} else { // process falling edge
		PCA0CPL0 = 0; // set next match to zero
		PCA0CPH0 = 0;
		PWM_OUT=0;
  }
 //handle other PCA interruption
 
} else if (CCF1) { // handle other PCA interruption sources
CCF1 = 0; 
} else if (CCF2) {
CCF2 = 0;
} else if (CCF3) {
CCF3 = 0;
} else if (CCF4) {
CCF4 = 0;
} else if (CF) {
CF = 0;
}
}
