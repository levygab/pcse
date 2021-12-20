#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "cpu.h"
#define N 4

#ifndef _PROG3_H
#define _PROG3_H

void ctx_sw(int* , int* ) ; //declaration de la fct assembleur

typedef struct processus {
  int pid ;
  char nom[10] ;
  int etat ; //vaut 0 si activable, 1 si elu
  int sauv_reg[5] ;
  int pile_exec[512] ;
  int endormi  ; //vaut 1 si la fct est endormie
  uint32_t sec_reveil ;
} processus ;

extern processus tab_pro[N] ;
extern processus* process_en_cours ;
extern uint16_t sec_tot ;

void init_process(processus* tab_pro) ;
void idle(void) ;
void proc1(void)  ;
void proc2(void)  ;
void proc3(void)  ;
void ordonnance(void) ;
int32_t mon_pid(void) ;
char *mon_nom(void) ;
int32_t cree_processus(void (*code)(void), char *nom) ;
void dors(uint32_t nbr_secs) ;

#endif
