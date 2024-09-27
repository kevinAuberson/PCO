/*
 ------------------------------------------------------------------------------
 Nom du fichier : synchro.h
 Auteur(s)      : Alexandre Shyshmarov Kevin Auberson
 Date creation  : 26.11.2023

 Description    : Implementation de la classe permettant de gérer des sections
                  partagée

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


#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "synchrointerface.h"

/**
 * @brief La classe Synchro implémente l'interface SynchroInterface qui
 * propose les méthodes liées à la section partagée.
 */
class Synchro final : public SynchroInterface
{
public:

    /**
     * @brief Synchro Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    Synchro():mutex(1),waiting(0), atStation(0) ,nbTrain(0), trainAtStation(false), nbWaiting(0),NB_MAX_TRAIN(1) {}

    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    void access(Locomotive &loco) override {

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));

        mutex.acquire();
        // Entrée du premier train dans la section partagée
        if(nbTrain < NB_MAX_TRAIN && loco.priority == 1){
            ++nbTrain;
            loco.priority = 0;
            mutex.release();
        }
        else{ // Entrée du second train dans la section partagée

            // Attente de la sortie d'un train de la section partagée
            loco.arreter();
            ++nbWaiting;
            mutex.release();
            waiting.acquire();

            // Entrée du second train dans la section partagée
            mutex.acquire();
            ++nbTrain;
            mutex.release();
            loco.demarrer();
        }
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section partagée
     *
     * Réveille les threads des locomotives potentiellement en attente.
     *
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive& loco) override {

        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));

        // Sortie de la section partagée
        mutex.acquire();
        --nbTrain;
        // S'il y a des trains en attente, on en libere un
        if(nbWaiting > 0){
            --nbWaiting;
            waiting.release();
        }
        mutex.release();
    }

    /**
     * @brief stopAtStation Méthode à appeler quand la locomotive doit attendre à la gare
     *
     * Implémentez toute la logique que vous avez besoin pour que les locomotives
     * s'attendent correctement.
     *
     * @param loco La locomotive qui doit attendre à la gare
     */
    void stopAtStation(Locomotive& loco) override {

        afficher_message(qPrintable(QString("The engine no. %1 arrives at the station.").arg(loco.numero())));

        // Entrée en gare
        loco.arreter();
        mutex.acquire();

        if (trainAtStation) {
            // Second train entrant dans la gare
            loco.priority = 1;
            trainAtStation = false;
            mutex.release();

            // Liberer le premier train de l'attente
            atStation.release(); 
        } else {
            // Premier train entrant dans la gare
            trainAtStation = true;
            mutex.release();
            // Attente du second train
            atStation.acquire();
        }
    }

private:
    PcoSemaphore mutex;
    PcoSemaphore waiting;
    PcoSemaphore atStation;
    unsigned int nbTrain;
    bool trainAtStation;
    unsigned int nbWaiting;
    const unsigned int NB_MAX_TRAIN;
};


#endif // SYNCHRO_H
