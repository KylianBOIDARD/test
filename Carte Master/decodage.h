#ifndef __DECODAGE_H__
#define __DECODAGE_H__

void init_commande();
void decoupage_caractere_message(char *);
int recherche_indice(char *);
int traitement_commande();
void traitement_information(void);
void clear_commande();
int cTOi(char *);

#endif