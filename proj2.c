/**
 * @brief Implementace druhe ulohy do predmetu IOS (Faneuil Hall Problem)
 * @file proj2.c
 *
 * @author Alexandr Chalupnik <xchalu15@stud.fit.vutbr.cz>
 * @date 13.4 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

#include "proj2.h"

struct Params par;

int main(int argc, char* argv[]){

    FILE *out;  //!< vystupni soubor

    if(arg_process(argc,argv) == ERR){
        return ERR;
    };

    if ((out = fopen("proj2.out", "w+")) == NULL){
        fprintf(stderr, "Nepodarilo se otevrit soubor!\n");
        return ERR;
    }

    return SUCC;
}

int arg_process(int c, char* v[]){

    if(c < 6 || c > 6) {
        return ERR;
    }
    int x = 0;
    for(int i = 1, val = 0; i < c; i++){

        if((x = sscanf(v[i],"%d",&val)) != 1)
            return ERR;

        if(i == 1){
            if(val < 1)
                return ERR;
            par.PI = val;
        }else if(val > 1){
            if(val < 0 || val > 2000)
                return ERR;

            switch(val){
                case 2: par.IG = val; break;
                case 3: par.JG = val; break;
                case 4: par.IT = val; break;
                case 5: par.JT = val; break;
            }
        }

    }

    return SUCC;
}