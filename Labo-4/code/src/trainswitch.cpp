/*
 ------------------------------------------------------------------------------
 Nom du fichier : trainswitch.cpp
 Auteur(s)      : Alexandre Shyshmarov Kevin Auberson
 Date creation  : 26.11.2023

 Description    : Definition de la classe TrainSwitch qui permet de simplifier
                  le changement d'aiguillage

 Remarque(s)    :
 ------------------------------------------------------------------------------
*/

#include "trainswitch.h"
#include "ctrain_handler.h"

TrainSwitch::TrainSwitch(int no,int direction,int temps,void (*func)(int,int,int))
    :no_aiguillage(no), direction(direction),temps_alim(temps),func(func) {}


void TrainSwitch::getSwitch() const{
    func(no_aiguillage,direction,temps_alim);
}
