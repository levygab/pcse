#include <cpu.h>
#include <inttypes.h>
#include <string.h>
#include "prog3.h"

// on peut s'entrainer a utiliser GDB avec ce code de base
// par exemple afficher les valeurs de x, n et res avec la commande display

// une fonction bien connue

processus tab_pro[N] ;
processus* process_en_cours ;
void masque_IRQ(int num_IRQ, int masque) ;

uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }
    return res;
}

void ecrit_car(uint32_t lig, uint32_t col, char c) ;
void efface_ecran(void) ;
void place_curseur(uint32_t lig, uint32_t col) ;
void traite_car(char c) ;
void aff_ecran(char* c ) ;
void defilement() ;
void freq_horloge() ;
void init_traitant_IT(int32_t num_IT, void (*traitant)(uint32_t*, uint32_t*)) ;
extern void traitant_IT_32(uint32_t* , uint32_t*) ;


void kernel_start(void) {
    efface_ecran() ;

    /*
    //on change la valeur du bit 0 du port 21 : permet d'autoriser les interrutptions de L'IRQ0
    char valeur = inb(0x21) ;
    valeur = valeur & 0xFE ;
    outb(valeur,0x21) ;
    */


    //initialisation des processus
    init_process(tab_pro) ;
    process_en_cours = tab_pro ;



    masque_IRQ(0,0) ;
    freq_horloge() ;
    init_traitant_IT(32, traitant_IT_32) ;

    //sti();

    idle() ;

    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}
