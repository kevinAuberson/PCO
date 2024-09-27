// Fichier              :   threadmanager.cpp
// Date modification    :   17.10.2023
// Auteurs              :   Auberson Kevin, Cosmo Vitória
// Modification         :   Ajustement de la fonction startHacking basé sur
//                          l'implémentation de la fonction workThread
//                          dans mythread.h

#include <QCryptographicHash>
#include <QVector>
#include "mythread.h"
#include "threadmanager.h"

/*
 * Nombre total de possibilité de mot de passe
 * Utilisation : workThread (1 lecture),
 *               ThreadManager::startHacking (1 écriture)
 * Protection  : aucune
 */
long long unsigned int nbToComputeGlobal;
/*
 * Mot de passe clair
 * Utilisation : workThread (1 écriture),
 *               ThreadManager::startHacking (1 écriture et valeur de retour)
 * Protection  : aucune
 */
QString result;

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow (
        long long unsigned int number,
        long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}

ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent)
{}


void ThreadManager::incrementPercentComputed(double percentComputed)
{
    emit sig_incrementPercentComputed(percentComputed);
}

/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        unsigned int nbThreads
)
{
    /*
     * Variable de résultat du hash trouvé par les threads
     */
    result = "";

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars;

    /*
     * Calcul du nombre de hash à générer
     */
    nbToComputeGlobal  = intPow(charset.length(),nbChars);

    /*
     * Calcul du nombre de hash à tester par thread,
     * le plus 1 pour régler le problème de division entière
     */
    long long unsigned int nbToComputePerThread = (nbToComputeGlobal/nbThreads)
            +1;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    nbValidChars       = charset.length();


    QVector<PcoThread*> threads;

    for (unsigned int i = 0; i < nbThreads; i++){
        threads.push_back(new PcoThread(
                              workThread,
                              this,
                              i,
                              nbThreads,
                              nbToComputePerThread,
                              std::ref(hash),
                              std::ref(salt),
                              nbValidChars,
                              std::ref(charset),
                              nbChars
                              )
                          );
    }

    for (PcoThread* thread : threads){
        thread->join();
        delete thread;
    }
    /* renvoie le bon hash ou est vide si aucun trouver */
    return result;
}
