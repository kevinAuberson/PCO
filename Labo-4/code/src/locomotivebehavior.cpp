/*
 ------------------------------------------------------------------------------
 Nom du fichier : locomotivebehavior.cpp
 Auteur(s)      : Alexandre Shyshmarov Kevin Auberson
 Date creation  : 26.11.2023

 Description    : Gestion du parcours d'une locomotive

 Remarque(s)    :
 ------------------------------------------------------------------------------
*/

/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    /* A vous de jouer ! */

    // Vous pouvez appeler les méthodes de la section partagée comme ceci :
    //sharedSection->access(loco);
    //sharedSection->leave(loco);
    //sharedSection->stopAtStation(loco);

    while(true) {

        // Diminution de la vitesse pour l'entrée en gars
        attendre_contact(parcours[0]);
        loco.fixerVitesse(loco.vitesse()/2);

        // Entrée en gare
        attendre_contact(parcours[1]);
        sharedSection->stopAtStation(loco);
        // Attendre que tous les passagés sortent du train
        PcoThread::usleep(5e6);

        loco.demarrer();

        // Entrée section partagée en fonction de la priorité
        attendre_contact(parcours[2]);
        sharedSection->access(loco);

        // Changement d'aiguillage pour faire passer le train
        aiguillage[0].getSwitch();

        // Une fois sortie de gare on augmente la vitesse
        attendre_contact(parcours[3]);
        loco.fixerVitesse(loco.vitesse()*2);

        // Changement d'aiguillage pour sortir de la section partagée
        attendre_contact(19);
        aiguillage[1].getSwitch();
        aiguillage[2].getSwitch();

        // Sortie de la section partagée
        attendre_contact(parcours[4]);
        sharedSection->leave(loco);
    }
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
