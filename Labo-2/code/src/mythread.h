// Fichier              :   mythread.h
// Date modification    :   17.10.2023
// Auteurs              :   Auberson Kevin, Cosmo Vitória
// Modification         :   ajout de la fonction workThread qui effectue
//                          une recherche par force brute du hachage MD5
//                          d'un mot de passe.

#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pcosynchro/pcothread.h>
#include <QString>
#include <QCryptographicHash>
#include <QVector>
#include <iostream>
#include "threadmanager.h"

extern QString result;
extern long long unsigned int nbToComputeGlobal;

/*
 * fonction algorithme de cassage de mot de passe manager
 * manager     :  pointeur du threadmanager pour appelerla fonction
 *                d'incrémentation de la barre de progression
 * threadID    :  numéro d'index du thread du vecteur de threads
 * nbThreads   :  nombre de thread exécuter
 * nbToCompute :  nombre de possibilité de mot de passe pour un thread
 * hash        :  hash à trouver
 * salt        :  sel
 * nbValidChars:  nombre total de caractère à tester
 * charset     :  set de caractères utilisables
 * nbChars     :  nombre de caractère dans le mot de passe
 */
void workThread(
        ThreadManager* manager,
        int threadID,
        unsigned int nbThreads,
        long long unsigned int nbToCompute,
        const QString& hash,
        const QString& salt,
        unsigned int nbValidChars,
        const QString& charset,
        unsigned int nbChars
        );

#endif // MYTHREAD_H
