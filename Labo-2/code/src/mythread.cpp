// Fichier              :   mythread.cpp
// Date modification    :   17.10.2023
// Auteurs              :   Auberson Kevin, Cosmo Vitória
// Modification         :   implémentation de la fonction de travail utilisée
//                          pour rechercher la préimage d'un hachage MD5 via
//                          une approche de force brute avec plusieurs threads.

#include "mythread.h"

/*
 * Flag permettant d'arrêter les threads
 * Utilisation : workThread (1 écriture et des lectures)
 * Protection  : aucune
 */
bool passwordCracked = false;


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
        )
{
    /*
     * Flag pour l'incrémetation du caractère suivant
     */
    bool changePosition = false;
    /*
     * Object QCryptographicHash servant à générer des md5
     */
    QCryptographicHash md5(QCryptographicHash::Md5);
    /*
     * Hash à vérifier
     */
    QString currentHash;
    /*
     * Nombre de mot de passee testé
     */
    unsigned long long int nbComputed = 0;
    /*
     * Mot de passe à tester courant
     */
    QString currentPasswordString;
    /*
     * Tableau contenant les index dans la chaine charset des caractères de
     * currentPasswordString
     */
    QVector<unsigned int> currentPasswordArray;

    /*
     * On initialise le premier mot de passe à tester courant en choisissant
     * comme première lettre du charset  correspondant à threadID puis
     * remplissant de nbChars - 1 fois du premier caractère de charset
     */
    currentPasswordString.fill(charset.at(0),nbChars - 1);
    currentPasswordString = charset.at(threadID) + currentPasswordString;
    currentPasswordArray.fill(0,nbChars);
    currentPasswordArray[0] = threadID;

    while (nbComputed < nbToCompute)
    {
        /* permet d'arreter les autres threads */
        if(passwordCracked)
            return;

        /* On vide les données déjà ajoutées au générateur */
        md5.reset();
        /* On préfixe le mot de passe avec le sel */
        md5.addData(salt.toLatin1());
        md5.addData(currentPasswordString.toLatin1());
        /* On calcul le hash */
        currentHash = md5.result().toHex();

        /*
         * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
         * puis on met le flag à true et on termine le thread
         */
        if (currentHash == hash)
        {
            result = currentPasswordString;
            passwordCracked = true;
            return;
        }
        /*
         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
         * de l'état de notre avancement (pour la barre de progression)
         */
        if ((nbComputed % 1000) == 0) {
            manager->incrementPercentComputed((double)1000/nbToComputeGlobal);
        }

        /*
         * On récupère le mot de pass à tester suivant.
         *
         * L'opération se résume à incrémenter currentPasswordArray comme si
         * chaque élément de ce vecteur représentait un digit d'un nombre en
         * base nbValidChars.
         *
         * Le digit de poids faible étant en position 0
         */

        unsigned int i = 0;

        while (i < (unsigned int)currentPasswordArray.size())
        {
            /* permet d'incrémenté la lettre à l'emplacement suivant */
            if(changePosition){
                currentPasswordArray[i]++;
                changePosition = false;
            }else {
                /* incrémente les lettres de façon à ce que chaque
                 * thread test une possiblité différente */
                currentPasswordArray[i] = currentPasswordArray[i] + nbThreads;
            }

            if (currentPasswordArray[i] >= nbValidChars)
            {
                /* met en place le décalage des
                 * différentes lettres entre threads */
                currentPasswordArray[i] = threadID;
                i++;
                changePosition = true;
            }
            else
            {
                break;
            }
        }

        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i=0;i<nbChars;i++)
            currentPasswordString[i]  = charset.at(currentPasswordArray.at(i));

        nbComputed++;
    }
    return;
}
