
#include "c8051F020.h"

// Prototypes de Fonctions
void Lumiere(unsigned char Intensite, unsigned char Lum_ON, unsigned char Lum_OFF,unsigned char Lum_Nbre);
void Software_Delay_1ms(void);
void config_clock (void);
void config_timer4(void);
void init_FFS1(void);
void allumage_MLI (unsigned int);




