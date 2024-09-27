/*
 ------------------------------------------------------------------------------
 Nom du fichier : trainswitch.h
 Auteur(s)      : Alexandre Shyshmarov Kevin Auberson
 Date creation  : 26.11.2023

 Description    : Declaration de la classe TrainSwitch qui permet de simplifier
                  le changement d'aiguillage

 Remarque(s)    :
 ------------------------------------------------------------------------------
*/

#ifndef TRAINSWITCH_H
#define TRAINSWITCH_H

#include "ctrain_handler.h"


/**
 * @brief La classe TrainSwitch permet de gerer l'aiguillage des trains et l'appel à la fonction
 * d'aiguillage. Elle simplifie la tache en ayant déjà tout les parametres.
 */
class TrainSwitch
{
public:
    /**
     * @brief Constructeur pour chaque aiguillage. Il y a une fonction par defaut qui peut être changer
     * si la logique du changement d'aiguillage change.
     */
    TrainSwitch(int no,int direction,int temps,void (*func)(int,int,int) = &diriger_aiguillage);


    /**
     * @brief Permet d'effectué le changement d'aiguillage
     */
    void getSwitch() const;

private:
    int no_aiguillage;
    int direction;
    int temps_alim;
    void (*func)(int,int,int);
};

#endif // TRAINSWITCH_H
