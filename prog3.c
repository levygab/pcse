#include "prog3.h"
uint16_t nb_pro = 0 ;

void idle(void) {
  for (int i=0 ; i<3 ; i++ ) {
    printf("[%s] pid = %i\n", mon_nom(), mon_pid());
    ordonnance();
    sti() ;
    hlt() ;
    cli() ;
  }
}


void proc1(void) {
  for (int i = 0 ; i<3 ; i++) {
    printf("[%s] pid = %i\n", mon_nom(), mon_pid());
    dors(3) ;
    sti() ;
    hlt() ;
    cli() ;
  }
}

void proc2(void) {
  for (int i = 0 ; i<3 ; i++) {
    printf("[%s] pid = %i\n", mon_nom(), mon_pid());
    dors(4) ;
    sti() ;
    hlt() ;
    cli() ;
  }
}

void proc3(void) {
  for (int i = 0 ; i<3 ; i++) {
    printf("[%s] pid = %i\n", mon_nom(), mon_pid());
    dors(5) ;
    sti() ;
    hlt() ;
    cli() ;
  }
}

int32_t cree_processus(void (*code)(void), char *nom) {
  nb_pro ++ ; //on incremente le nb de processus cree
  if (nb_pro > N) return -1 ; //on dit que trop de processus ont ete cree

  //on s'occupe maintenant de rentrer code dand tab_pro
  tab_pro[nb_pro].pid = nb_pro ;
  strcpy(tab_pro[nb_pro].nom, nom) ;
  tab_pro[nb_pro].etat = 0 ;
  tab_pro[nb_pro].endormi = 0 ;
  tab_pro[nb_pro].sauv_reg[1] = (int) ((tab_pro[nb_pro].pile_exec)+511) ;
  tab_pro[nb_pro].pile_exec[511] = (int) code ;

  return nb_pro ;
}

void init_process(processus* tab_pro) {

  int num_pid ; //recupere le numero du pid

  //ici le tableau aura une taille de  N
  tab_pro[0].pid = 0 ;
  strcpy(tab_pro[0].nom,"idle") ;
  tab_pro[0].etat = 1 ;
  tab_pro[0].endormi = 0 ;

  num_pid = cree_processus(proc1, "proc1") ; //on  cree le processus avec la fonction donnee
  //on fait une verif
  if (num_pid == -1) printf("erreur dans cree_processus de 2 \n" );

  num_pid = cree_processus(proc2, "proc2") ; //on  cree le processus avec la fonction donnee
  //on fait une verif
  if (num_pid == -1) printf("erreur dans cree_processus de 3 \n" );

  num_pid = cree_processus(proc3, "proc3") ; //on  cree le processus avec la fonction donnee
  //on fait une verif
  if (num_pid == -1) printf("erreur dans cree_processus de 4 \n");

  /*
  //on a plus besoin d'initialiser le deuxieme processus
  //init du deuxieme processus
  tab_pro[1].pid = 1 ;
  strcpy(tab_pro[1].nom,"proc1") ;
  tab_pro[1].etat = 0 ;

  tab_pro[1].sauv_reg[1] = (int) ((tab_pro[1].pile_exec)+511) ;
  tab_pro[1].pile_exec[511] = (int) proc1 ;
  */

}


 void ordonnance(void) {
   /*
   if (process_en_cours -> pid == 0 ) {  //on est sur idle
     tab_pro[0].etat = 0 ;
     tab_pro[1].etat = 1 ;
     process_en_cours = tab_pro+1 ;
     ctx_sw(tab_pro[0].sauv_reg, tab_pro[1].sauv_reg);
   }

   else if (process_en_cours -> pid == 1 ) {  //on est sur proc1
     tab_pro[1].etat = 0 ;
     tab_pro[2].etat = 1 ;
     process_en_cours = tab_pro+2 ;
     ctx_sw(tab_pro[1].sauv_reg , tab_pro[0].sauv_reg);
   }
   */

   //on traite les processus endormi
   for (int i = 0 ; i < N ; i++ ) {
     if (tab_pro[i].endormi == 1) {
       if (tab_pro[i].sec_reveil < sec_tot) {
         tab_pro[i].endormi = 0 ;
       }
     }
   }

   int num_pid = process_en_cours -> pid ; //on recupere le processus en cours

   //on va chercher le numero du futur processus : on aura un idle entre lui et les autre procesus
   int nv_pid ;
   int find = 0 ; //varaible servant a dire si on a trouve un nouveua pid
   for ( int i = num_pid +1  ; i < 4 ; i ++ ) {
     if (tab_pro[i].endormi == 0) {
       nv_pid = i ; //le futur pid peut etre la valeur i donc on lui donne
       i = 4 ; //pour mettre fin a la boucle
       find = 1 ; //on a bien trouve un nouveau pid
     }
    }

    if (find == 0) nv_pid = 0 ; //si on a rien trouve, le prochain process est idle

   tab_pro[num_pid].etat = 0 ; //on met le process en cours a 0
   tab_pro[nv_pid].etat = 1 ; //on met le futur process a 1
   process_en_cours = tab_pro + nv_pid ;

   ctx_sw(tab_pro[num_pid].sauv_reg , tab_pro[nv_pid].sauv_reg);


 }

int32_t mon_pid(void) {
  return process_en_cours -> pid ;
}

char *mon_nom(void) {
  return process_en_cours -> nom ;
}

void dors(uint32_t nbr_secs) {
  process_en_cours -> endormi = 1 ; //on met dors a 1 pour endormir la fct
  process_en_cours -> sec_reveil = sec_tot + nbr_secs ;  // on met le moment du reveil
  ordonnance() ; //on fait ordonnance pour appeler un autre processus
}
