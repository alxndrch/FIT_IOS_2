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
    int PI;  //!< pocet procesu vygenerovany v kategorii pristehovalcu
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
};

/**
 * @brief zpracovani argumentu pri spusteni programu
 *
 * @param c pocet argumentu
 * @param v argumenty z prikazove radky
 * @return SUCC pri uspechu, jinak ERR
 */
int arg_process(int c, char* v[]);

#endif //FIT_IOS_2_PROJ2_H
