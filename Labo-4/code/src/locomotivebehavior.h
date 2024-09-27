/*
 ------------------------------------------------------------------------------
 Nom du fichier : locomotivebehavior.h
 Auteur(s)      : Alexandre Shyshmarov Kevin Auberson
 Date creation  : 26.11.2023

 Description    : Declaration des fonctions permettant de controler
                  une locomotive

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


#ifndef LOCOMOTIVEBEHAVIOR_H
#define LOCOMOTIVEBEHAVIOR_H

#include "locomotive.h"
#include "launchable.h"
#include "synchrointerface.h"
#include <vector>
#include "trainswitch.h"

/**
 * @brief La classe LocomotiveBehavior représente le comportement d'une locomotive
 */
class LocomotiveBehavior : public Launchable
{
public:
    /*!
     * \brief locomotiveBehavior Constructeur de la classe
     * \param loco la locomotive dont on représente le comportement
     */
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SynchroInterface> sharedSection,
                       std::vector<int> parcours, std::vector<TrainSwitch> aiguillages)
    : loco(loco), sharedSection(sharedSection), parcours(parcours), aiguillage(aiguillages) {}

protected:
    /*!
     * \brief run Fonction lancée par le thread, représente le comportement de la locomotive
     */
    void run() override;

    /*!
     * \brief printStartMessage Message affiché lors du démarrage du thread
     */
    void printStartMessage() override;

    /*!
     * \brief printCompletionMessage Message affiché lorsque le thread a terminé
     */
    void printCompletionMessage() override;

    /**
     * @brief loco La locomotive dont on représente le comportement
     */
    Locomotive& loco;

    /**
     * @brief sharedSection Pointeur sur la section partagée
     */
    std::shared_ptr<SynchroInterface> sharedSection;

    /*
     * Vous êtes libres d'ajouter des méthodes ou attributs
     *
     * Par exemple la priorité ou le parcours
     */

    /**
     * @brief parcours tableau des capteurs à atteindre
     */
    std::vector<int> parcours;

    /**
     * @brief aiguillage tableau des aiguillages à changer
     */
    std::vector<TrainSwitch> aiguillage;
};

#endif // LOCOMOTIVEBEHAVIOR_H
