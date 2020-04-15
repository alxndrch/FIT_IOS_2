/**
 * @brief Implementace druhe ulohy do predmetu IOS (Faneuil Hall Problem)
 * @file proj2.c
 *
 * @author Alexandr Chalupnik <xchalu15@stud.fit.vutbr.cz>
 * @date 13.4 2020
 */

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "proj2.h"

int shm_fd = 0;  //!< fd sdilene pameti
struct Shared_data* data = NULL;  //!< sdilena data
struct Params par;  //!< parametry z prikazove radky

int main(int argc, char* argv[]){

    printf("MAIN process => PID=%d\n", getpid());

    sem_unlink("/proj2_sem_judge");
    sem_unlink("/proj2_sem_check");
    sem_unlink("/proj2_sem_confirm");
    sem_unlink("/proj2_sem_print_row");
    shm_unlink("/proj2_shared_mem");

    // zpracovani argumentu
    if(arg_process(argc,argv) == ERR){
        fprintf(stderr, "Chybne zadane argumenty\n");
        return ERR;
    };

    // nastaveni sdilene pameti
    if((set_sh_mem(&shm_fd,&data)) == ERR)
        return ERR;

    // nastaveni semaforu
    if(set_semaphores() == ERR)
        return ERR;

    // inicializace sdilene pameti
    if((init_sh_data(data, shm_fd)) == ERR )
        return ERR;

    // vytvoreni novych procesu
    pid_t pid=fork();
    if(pid == 0){  // child; pomocny proces pro generovani imigrantu

        generate_immigrants();
        exit(SUCC);

    }else if(pid == -1){
        fprintf(stderr,"Nastala chyba pri vytvareni procesu\n");
        clean_sh_mem(data, shm_fd);
        return ERR;
    }else{  // parent; proces pro soudce

        generate_judge();

    }

    wait(NULL);  // cekani na dokonceni vsech vytvorenych procesu

    fclose(data->out);  // zavreni souboru pro vypis
    clean_semaphores();  // zruseni vsech semaforu
    clean_sh_mem(data,shm_fd);  // zruseni sdilene pameti

    return SUCC;
}

int arg_process(int c, char* v[]){

    if(c < 6 || c > 6) {
        return ERR;
    }

    for(int i = 1, val = 0; i < c; i++){

        if(sscanf(v[i],"%d",&val) != 1)
            return ERR;

        if(i == 1){
            if(val < 1)
                return ERR;
            par.PI = val;
        }else if(i > 1){
            if(val < 0 || val > 2000)
                return ERR;

            switch(i){
                case 2: par.IG = val; break;
                case 3: par.JG = val; break;
                case 4: par.IT = val; break;
                case 5: par.JT = val; break;
            }
        }

    }

    return SUCC;
}

void print_to_file(FILE* f, char * str, ...){

    sem_wait(data->print_row);

    va_list args;
    va_start(args, str);
    vfprintf(f, str, args);
    va_end(args);

    sem_post(data->print_row);

}

int set_sh_mem(int* shm_fd, struct Shared_data** data){

    if((*shm_fd = shm_open("/proj2_shared_mem", O_CREAT | O_RDWR | O_EXCL, 0666)) == -1){
        // nepodarilo se otevrit sdilenou pamet
        return ERR;
    }

    if((ftruncate(*shm_fd, sizeof(struct Shared_data)) == -1)){
        // nepodarilo se nastavit velikost pameti
        shm_unlink("/proj2_shared_mem");
        close(*shm_fd);
        return ERR;
    }

    if((*data = mmap(0,sizeof(struct Shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_fd, 0)) == MAP_FAILED){
        // nepodarilo se namapovat
        shm_unlink("/proj2_shared_mem");
        close(*shm_fd);
        return ERR;
    }

    return SUCC;
}

void clean_sh_mem(struct Shared_data* data, int shm_fd){

    munmap(data,sizeof(struct Shared_data));
    shm_unlink("/proj2_shared_mem");
    close(shm_fd);  // zavreni fd alokovaneho v shm_open

}

int init_sh_data(struct Shared_data* data, int shm_fd){

    // otevreni souboru pro vypis
    if ((data->out = fopen("proj2.out", "w+")) == NULL){
        clean_sh_mem(data,shm_fd);
        fprintf(stderr, "Nepodarilo se otevrit soubor\n");
        return ERR;
    }
    setbuf(data->out,NULL);

    data->cnt = data->NE = data->NC = data->NB = 0;

    return SUCC;

}

int set_semaphores(){

    if((data->judge = sem_open("/proj2_sem_judge", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
        return ERR;
    }

    if((data->check = sem_open("/proj2_sem_check", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        sem_close(data->judge);
        sem_unlink("/proj2_sem_judge");
        return ERR;
    }

    if((data->confirm = sem_open("/proj2_sem_confirm", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
        sem_close(data->judge);
        sem_close(data->check);
        sem_unlink("/proj2_sem_judge");
        sem_unlink("/proj2_sem_check");
        return ERR;
    }

    if((data->print_row = sem_open("/proj2_sem_print_row", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
        sem_close(data->judge);
        sem_close(data->check);
        sem_close(data->confirm);
        sem_unlink("/proj2_sem_judge");
        sem_unlink("/proj2_sem_check");
        sem_unlink("/proj2_sem_confirm");
        return ERR;
    }

    return SUCC;
}

void clean_semaphores(){

    sem_close(data->judge);
    sem_close(data->check);
    sem_close(data->confirm);
    sem_close(data->print_row);

    sem_unlink("/proj2_sem_judge");
    sem_unlink("/proj2_sem_check");
    sem_unlink("/proj2_sem_confirm");
    sem_unlink("/proj2_sem_print_row");

}

void process_judge(){

    printf("JUDGE process => PPID=%d, PID=%d\n", getppid(), getpid());

    do{
        usleep(generate_random(par.JG));
        print_to_file(data->out, "%d: JUDGE: wants to enter\n", ++data->cnt);

        sem_wait(data->judge);

        print_to_file(data->out, "%d: JUDGE: enters: %d: %d: %d\n", ++data->cnt, data->NE, data->NC, data->NB);

        if(data->NE != data->NC){
            printf("NE: %d, NC: %d\n",data->NE,data->NC);
            print_to_file(data->out, "%d: JUDGE: waits for imm: %d: %d: %d\n", ++data->cnt, data->NE, data->NC, data->NB);
            sem_post(data->check);

        }else{
//            fprintf(data->out, "%d: JUDGE: starts confirmation: %d: %d: %d\n", data->cnt, data->NE, data->NC, data->NB);
//            usleep(generate_random(par.JT));
//            fprintf(data->out, "%d: JUDGE: ends onfirmation: %d: %d: %d\n", data->cnt, data->NE--, data->NC--, data->NB);
//            sem_post(data->confirm);
        }

        usleep(generate_random(par.JT));
        print_to_file(data->out, "%d: JUDGE: leaves: %d: %d: %d\n", ++data->cnt, data->NE, data->NC, data->NB);

        sem_post(data->judge);

    }while(par.PI != data->NC);

    print_to_file(data->out, "%d: JUDGE: finishes\n", ++data->cnt);

    sem_post(data->check);
    sem_post(data->judge);

}

void process_immigrant(int proc_num){

    printf("%d: IMMIGRANT process => PPID=%d, PID=%d\n", proc_num, getppid(), getpid());

    print_to_file(data->out, "%d: IMM %d: starts\n", ++data->cnt, proc_num);

    // pokousi se dostat do budovy
    // pokud je soudce v budove, ceka az odejde
    sem_wait(data->judge);
    data->NE++;
    data->NB++;

    print_to_file(data->out, "%d: IMM %d: enters: %d: %d: %d\n", ++data->cnt, proc_num, data->NE, data->NC, data->NB);

    sem_post(data->judge);

    // registrace
    // kazdy proces imigrant se registruje samostatne, poradi odpovida vstupu
    sem_wait(data->check);
    data->NC++;

    print_to_file(data->out, "%d: IMM %d: checks: %d: %d: %d\n", ++data->cnt, proc_num, data->NE, data->NC, data->NB);

    sem_post(data->check);

//    sem_wait(data->confirm);
//    // proces ceka na vydani rozhodnuti soudce
//    // po vydani rozhodnuti si vyzvedava certifikat
//    fprintf(data->out, "%d: IMM %d: wants certificate: %d: %d: %d\n", data->cnt, proc_num, data->NE, data->NC, data->NB);
//    usleep(generate_random(par.IT));
//    fprintf(data->out, "%d: IMM %d: got certificate: %d: %d: %d\n", data->cnt, proc_num, data->NE, data->NC, data->NB);

    // odchod z budouvy
    // pokud je soudce v budove ceka na jeho odchod
    // pokud neni v budouve tiskne:
    sem_wait(data->judge);
    print_to_file(data->out, "%d: IMM %d: leaves: %d: %d: %d\n", ++data->cnt, proc_num, data->NE, data->NC, data->NB);
    sem_post(data->judge);

}

void generate_judge(){

    pid_t pid = fork();
    if(pid == 0) {

        process_judge();
        exit(SUCC);
    }
    else if(pid > 0) {
        wait(NULL);
    }
    else {
        printf("Unable to create child process.\n");
    }

}

void generate_immigrants(){

    printf("GENERATOR IMMIGRANT process => PPID=%d, PID=%d\n", getppid(), getpid());

    for(int i = 1; i <= par.PI; i++) {

        usleep(generate_random(par.IG));

        pid_t pid = fork();
        if(pid == 0) {
            process_immigrant(i);
            exit(0);
        }
        else if(pid > 0)  {
            wait(NULL);
        }
    }

}

int generate_random(int max){
    srand(time(0));
    return (rand() % (max + 1)) * 1000;  // prevod na mikrosekundy
}