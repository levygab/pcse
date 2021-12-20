#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cpu.h"
#include <string.h>

uint32_t ligne = 0 ;
uint32_t colonne = 0 ;
void defilement() ;

uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
    uint16_t* pointeur ;
    pointeur = (uint16_t*) (0xB8000 + 2 * ( lig*80 + col) ); //formule donnee
    return pointeur ;
}

void ecrit_car(uint32_t lig, uint32_t col, char c) {
  uint16_t* pointeur = ptr_mem( lig , col ) ;
  // go ecrire le charactere
  uint16_t valeur ;
  valeur = c ;

  uint16_t format ;
  format =  0x0F ; //ecrit en blanc sur fond noir
  //on decale le caractere sur la partie forte
  format = format << 8 ;
  valeur = valeur | format ;
  *pointeur = valeur ;
}

void efface_ecran(void) {
  int i , j ;
  //uint16_t* pointeur ;
  for(i=0 ; i < 80 ; i++) {
    for (j=0 ; j<25 ; j++) {
      ecrit_car(j,i,' ');
      //pointeur = ptr_mem(j,i) ;
      //*pointeur = 0 ;
    }
  }
}

void place_curseur(uint32_t lig, uint32_t col) {
  uint16_t position = (uint16_t) col + lig * 80 ; //formule donnee

  //partie basse
  outb(0x0F , 0x3D4) ; //envoie la commande sur le port commande
  outb(position,0x3D5) ; //envoie commande sur port donnees

  //partie haute
  position = position >> 8 ; // on decale de 8 pour avoir les 8 bits de la partie haute
  outb(0x0E , 0x3D4) ; //envoie la commande sur le port commande
  outb(position,0x3D5) ;
}

void traite_car(char c) {
  if (127 > c && c >= 32) { //cas le plus simple
    ecrit_car(ligne , colonne , c) ;
    //deplace curseur
    if (colonne < 79) { //cas ou on est pas en bout de ligne
      colonne ++ ;
      place_curseur (ligne , colonne) ;
    }
    else if (ligne < 24 ) { //bout de ligne mais pas a la derniere ligne
      colonne = 0 ; ligne ++  ;
    }
    else { //on est en toute fin
      defilement() ;
      efface_ecran() ;
      ligne = 24 ;
      colonne = 0 ;
      place_curseur(ligne,colonne) ; //toute fin d'ecran
    }
  }
  else {
    if ( c == 8 ) {
      if (colonne != 0) {
        colonne = colonne - 1 ; //deminue colonne de 1
        place_curseur(ligne , colonne ) ; //on place le curseur
      }
      else {
        colonne = 79 ;
        place_curseur(ligne,colonne) ;
      }
    } //cas on recule la colonne
    if ( c == 9 ) {
      if (colonne > 72) {
        colonne = 0 ;
        place_curseur(ligne , colonne ) ;
      }//cas ou on arrive en fin
      else {
        colonne += 8 ;
        place_curseur(ligne , colonne ) ;
      }//sinon on avance colonne
    }
    if ( c == 10 ) {
      colonne = 0 ;
      if (ligne != 24) ligne = ligne + 1 ;
      place_curseur(ligne , colonne) ;
    }
    if ( c == 12 ) {
      efface_ecran () ;
      colonne = 0 ; ligne = 0 ;
      place_curseur(ligne , colonne) ;
    }
    if ( c == 13 ) {
      colonne = 0 ; place_curseur(ligne , colonne ) ;
    }
  }
}

void defilement() {
  //recuperation pointeurs depart et arrivee
  uint16_t* arrive = ptr_mem(0,0) ;
  uint16_t* depart = ptr_mem(1,0) ;

  //utilisation memmove
  memmove ( arrive , depart , 2000 ) ;
}

void console_putbytes(const char *s, int len) {
  int i  ;
  for (i=0 ; i<len ; i++) {
    traite_car(s[i]) ;
  }
}
