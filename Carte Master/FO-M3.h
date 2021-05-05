


#include <C8051F020.h>
#include "c8051F020_SFR16.h"
#include <string.h>

// Prototypes de Fonctions
void configPCA (void);
void PCA_ISR (void); // PCA Interrupt Service Routine
unsigned int CDE_Servo_H (int );
void Init_FO_M3();
unsigned int F0_M3(int );




