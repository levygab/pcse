#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cpu.h"
#include "segment.h"

uint32_t sec = 0 ;
uint16_t sec_tot = 0 ;
uint32_t min = 0 ;
uint32_t heu = 0;
uint32_t compteur = 0 ;
uint32_t QUARK = 0x1234DD ;
uint32_t CLOCKFREQ = 50 ;

void ecrit_car(uint32_t lig, uint32_t col, char c) ;

void aff_ecran(char* c ) {
  int i ;
  uint32_t ligne = 0 , colonne = 80 - 8 ;

  for (i=0 ; i < 8 ; i++ ) {
    ecrit_car(ligne , colonne , c[i]) ; //on ecrit a l'emplacement
    colonne ++ ; //puis on avance la colonne de 1
  }
}

void tic_PIT() {
  //on commence par prevenir le controlleur d'interruption
  outb(0x20,0x20) ;
  compteur ++ ;

  if (compteur == 50) {
    compteur = 0 ;
    sec ++ ;
    sec_tot ++ ;

    if (sec == 60) {
      sec = 0 ;
      min ++ ;

      if (min == 60) {
        min = 0 ;
        heu ++ ;
      }

    }

  }

  char charactere[9] ;

  sprintf(charactere, "%02d:%02d:%02d",heu,min,sec) ;

  aff_ecran(charactere) ;

}

void init_traitant_IT(int32_t num_IT, void (*traitant)(uint32_t*, uint32_t*)) {
  /*
  On initialise le traitant num_IT avec l'adresse dans l'autre variable farfelue
  */

  uint32_t* p = (uint32_t*) 0x1000 ; //on se place sur la table
  //on va se deplacer jusqu'a l'adresse voulu
  p = p + 2* num_IT ; //chaque case est sur 64 octets
  //on est au debut de la case numero num_IT


  //on doit ecrire kernel dans les 16 bits de poids fort
  *p = KERNEL_CS << 16 ;

  uint32_t adresse = (uint32_t) traitant ; //contient l'@ de traitant

  uint32_t ecrit = adresse & 0x0000FFFF ; //contient les 16 bits de poids faibles

  //on ecrit maintenant cette valeur
  *p = *p & 0xFFFF0000 ; //on efface les 16 derniers bits (ne modifie pas les 16 premiers)
  *p = *p | ecrit ; //on ecrit sur les 16 derniers bits (ne modifie pas les 16 premiers)


  //on va maintenant ecrire l'autre partie
  p = p+1 ; //on se deplace de 32 bits

  // on ecrit ce qu'il faut ecrire dans les bits faibles
  *p = 0x8E00 ;

  ecrit = adresse & 0xFFFF0000 ; //contient les 16 bits de poids forts
  *p = *p & 0x0000FFFF ; //on efface les 16 premeirs bits (ne modifie pas les 16 derniers)
  *p = *p | ecrit ; //on ecrit sur les 16 premiers bits (ne modifie pas les 16 derniers)

}

void freq_horloge() {

  //fct qui regle la frequence d'horloge a 50 Hz
  outb(0x34, 0x43) ; //premiere etape, on previent qu'on va changer la freq d'horloge
  //printf("QUARK/CLOCKFREQ)mod256 est %d\n",(QUARK/CLOCKFREQ)%256 );
  //printf("QUARK/CLOCKFREQ)/256 est %ld\n",(long)(QUARK/CLOCKFREQ)/256 );
  outb((QUARK/CLOCKFREQ)%256, 0x40) ; //les 8 bits de poids faible
  outb((QUARK/CLOCKFREQ)>>8, 0x40) ; //les 8 bits de poids forts
}

void masque_IRQ(int num_IRQ, int masque) {

  char valeur = inb(0x21) ;
  //int i ;
  //char changement ;

  if (num_IRQ == 0) {
    if (masque == 0) valeur = valeur & 0xFE ; //on met le bit 0 a 0
    else valeur = valeur | 0x01 ; //on met le bit 0 a 1
  }
  if (num_IRQ == 1) {
    if (masque == 0) valeur = valeur & 0xFD ; //on met le bit 1 a 0
    else valeur = valeur | 0x02 ; //on met le bit 1 a 1
  }
  if (num_IRQ == 2) {
    if (masque == 0) valeur = valeur & 0xFB ; //on met le bit 2 a 0
    else valeur = valeur | 0x04 ; //on met le bit 2 a 1
  }
  if (num_IRQ == 3) {
    if (masque == 0) valeur = valeur & 0xF7 ;
    else valeur = valeur | 0x08 ;
  }
  if (num_IRQ == 4) {
    if (masque == 0) valeur = valeur & 0xEF ;
    else valeur = valeur | 0x10 ;
  }
  if (num_IRQ == 5) {
    if (masque == 0) valeur = valeur & 0xDF ;
    else valeur = valeur | 0x20 ;
  }
  if (num_IRQ == 6) {
    if (masque == 0) valeur = valeur & 0xBF ;
    else valeur = valeur | 0x40 ;
  }
  if (num_IRQ == 7) {
    if (masque == 0) valeur = valeur & 0x7F ;
    else valeur = valeur | 0x80 ;
  }
  outb(valeur,0x21) ;
}
