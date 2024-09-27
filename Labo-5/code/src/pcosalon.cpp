/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Modifications à faire dans le fichier

#include "pcosalon.h"

#include <pcosynchro/pcothread.h>

#include <iostream>

PcoSalon::PcoSalon(GraphicSalonInterface *interface, unsigned int capacity)
    : _interface(interface), inService(true), MAX_WAITING(capacity),
      MAX_SALON(capacity + 1),nextChair(0) {}


/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
unsigned int PcoSalon::getNbClient()
{    
    size_t nbClient;
    _mutex.lock();

    nbClient = clientInSalon.size();

    _mutex.unlock();

    return nbClient;
}

bool PcoSalon::accessSalon(unsigned clientId)
{   
    _mutex.lock();

    // Si la taille de la file de salon atteint la capacité maximale,
    // le client ne peut pas accéder
    if(clientInSalon.size() == MAX_SALON){
        _mutex.unlock();
        return false;
    }

    unsigned chairPos;

    // Ajoute le client à la file d'attente dans le salon
    clientInSalon.push(clientId);

    // Assigne à chaque client, une chaise d'attente
    if(clientInSalon.front() != clientId){
        // Détermine la position de la chaise pour le client
        chairPos = nextChair++ ;
        nextChair %= MAX_WAITING;
    }

    animationClientAccessEntrance(clientId);

    // Notifie le barbier de l'arrivée d'un client si ce dernier dors si non fait rien
    barberSleep.notifyOne();

    if(clientInSalon.front() != clientId){
        animationClientSitOnChair(clientId,chairPos);
    }

    // Le client attend que son tour arrive dans la file d'attente pour se faire coiffer
    while(clientInSalon.front() != clientId){
        // Le client attend en attendant la notification du barbier
        clientWait.wait(&_mutex);
    }

    _mutex.unlock();
    return true;
}


void PcoSalon::goForHairCut(unsigned clientId)
{
    _mutex.lock();

    animationClientSitOnWorkChair(clientId);
    // Notifie le barbier que le client est prêt sur la chaise de coiffure, si ce dernier est en attente
    clientWorkChair.notifyOne();
    // Attend que le barbier lui coupe les cheuveux
    barberCutting.wait(&_mutex);
    _mutex.unlock();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId)
{
    _mutex.lock();
    animationClientWaitForHairToGrow(clientId);
    _mutex.unlock();
}


void PcoSalon::walkAround(unsigned clientId)
{
    _mutex.lock();
    animationClientWalkAround(clientId);
    _mutex.unlock();
}


void PcoSalon::goHome(unsigned clientId)
{
    _mutex.lock();
    animationClientGoHome(clientId);
    _mutex.unlock();
}


/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
void PcoSalon::goToSleep()
{
    _mutex.lock();
    animationBarberGoToSleep();
    // Le barbier attend en dormant jusqu'à ce qu'un client arrive
    barberSleep.wait(&_mutex);

    animationWakeUpBarber();
    _mutex.unlock();
}


void PcoSalon::pickNextClient()
{
    _mutex.lock();
    clientWait.notifyAll();
    _mutex.unlock();
}

void PcoSalon::waitClientAtChair()
{
    _mutex.lock();
    // Le barbier attend que le client soit prêt sur la chaise de coiffure
    clientWorkChair.wait(&_mutex);
    _mutex.unlock();
}


void PcoSalon::beautifyClient()
{
    _mutex.lock();
    animationBarberCuttingHair();
    // Notifie le client que la coupe de cheveux a commencé
    barberCutting.notifyOne();
    // Le client quitte le salon après la coupe de cheveux
    clientInSalon.pop();
    _mutex.unlock();
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService()
{
    bool rtn;
    _mutex.lock();

    rtn = inService;

    _mutex.unlock();

    return rtn;
}


void PcoSalon::endService()
{
    _mutex.lock();

    inService = false;

    _mutex.unlock();
}

/********************************************
 *   Méthodes privées pour les animations   *
 *******************************************/
void PcoSalon::animationClientAccessEntrance(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientAccessEntrance(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnChair(unsigned clientId, unsigned clientSitNb)
{
    _mutex.unlock();
    _interface->clientSitOnChair(clientId, clientSitNb);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnWorkChair(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientSitOnWorkChair(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientWaitForHairToGrow(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, true);
    _mutex.lock();
}

void PcoSalon::animationClientWalkAround(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWalkAround(clientId);
    _mutex.lock();
}

void PcoSalon::animationBarberGoToSleep()
{
    _mutex.unlock();
    _interface->barberGoToSleep();
    _mutex.lock();
}

void PcoSalon::animationWakeUpBarber()
{
    _mutex.unlock();
    _interface->clientWakeUpBarber();
    _mutex.lock();
}

void PcoSalon::animationBarberCuttingHair()
{
    _mutex.unlock();
    _interface->barberCuttingHair();
    _mutex.lock();
}

void PcoSalon::animationClientGoHome(unsigned clientId){
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, false);
    _mutex.lock();
}
