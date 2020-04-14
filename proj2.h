/**
 * @brief Implementace druhe ulohy do predmetu IOS (Faneuil Hall Problem)
 * @file proj2.h
 *
 * @author Alexandr Chalupnik <xchalu15@stud.fit.vutbr.cz>
 * @date 13.4 2020
 */

#ifndef FIT_IOS_2_PROJ2_H
#define FIT_IOS_2_PROJ2_H

#define SUCC 0
#define ERR 1

/**
 * parametry z prikazove radky
 */
struct Params{
    int PI;  //!< pocet procesu vygenerovanych v kategorii pristehovalcu
    int IG;  //!< maximalni hodnota doby v ms, po ktere je generovan novy proces immigrant
    int JG;  //!< maximalni hodnota doby v ms, po ktere soudce opet vsoupti do budovy
    int IT;  //!< maximalni hodnota doby v ms, ktera simuluje trvani vyzvedavani certifikatu pristehovalcem
    int JT;  //!< maximalni hodnota doby v ms, ktera simuluj trvani vydavani rozhodnuti soudcem
};

/**
 * sdilena data
 */
struct Shared_data{
    int cnt;  //!< citac akci
    FILE* out;  //!< vystupni soubor
    int NE;  //!< aktualni pocet imigrantu, kteri vstoupily do budouvy; nebylo rozhodnuto
    int NC;  //!< aktualni pocet imigrantu, kteri se zaregistrovali; nebylo rozhodnuto
    int NB;  //!< aktualni pocet imigrantu, kteri jsou v budouve
};

sem_t *judge;
sem_t *check;
sem_t *confirm;


/**
 * @brief zpracovani argumentu pri spusteni programu
 *
 * @param c pocet argumentu
 * @param v argumenty z prikazove radky
 * @return SUCC pri uspechu, jinak ERR
 */
int arg_process(int c, char* v[]);

/**
 * @brief nastaveni sdilene pameti
 *
 * @param shm_fd pipisovac sdilene pameti
 * @param data
 * @return SUCC pokud nenastane chyba, jinak ERR
 */
int set_sh_mem(int* shm_fd, struct Shared_data** data);

/**
 * @brief uvolneni zdroju pro sdilenou pamet
 *
 * @param data
 * @param shm_fd
 */
void clean_sh_mem(struct Shared_data* data, int shm_fd);

/**
 * @brief inicializace sdilenych dat
 *
 * @param data sdilena data
 * @return SUCC pokud nenastala chyba pri otevirani souboru, jinak ERR
 */
int init_sh_data(struct Shared_data* data, int shm_fd);

/**
 * @brief vytvoreni semaforu
 *
 * @return SUCC pokud byly vytvoreny semafory, jinak ERR
 */
int set_semaphores();

/**
 * @brief uvolneni zdroju vsech semaforu
 */
void clean_semaphores();

/**
 * @brief proces soudce
 *
 * @param data sdilena data
 * @return SUCC pri uspesnem behu jinak ERR
 */
int process_judge(struct Shared_data* data);

/**
 * @brief proces soudce
 *
 * @param data sdilena data
 * @param proc_num cislo procesu
 * @return SUCC pri uspesnem behu jinak ERR
 */
int process_immigrant(struct Shared_data* data, int proc_num);

/**
 * @brief generovani imigrantu
 *
 * @return
 */
int generate_immigrants();

/**
 * @brief generovani nahodnych cisel
 *
 * @param max maximalni hodnota intervalu
 * @return nahodne cislo
 */
int generate_random(int max);

#endif //FIT_IOS_2_PROJ2_H
