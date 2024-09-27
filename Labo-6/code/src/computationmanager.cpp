//     ____  __________     ___   ____ ___  _____ //
//    / __ \/ ____/ __ \   |__ \ / __ \__ \|__  / //
//   / /_/ / /   / / / /   __/ // / / /_/ / /_ <  //
//  / ____/ /___/ /_/ /   / __// /_/ / __/___/ /  //
// /_/    \____/\____/   /____/\____/____/____/   //
// Auteurs : Kevin Auberson, Alexandre Shyshmarov


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include "computationmanager.h"
#include <algorithm>

using namespace std;


ComputationManager::ComputationManager(int maxQueueSize):
        MAX_TOLERATED_QUEUE_SIZE(maxQueueSize),
        buffer(3),
        computationResult(3),
        full(3),
        empty(3)
{}

int ComputationManager::requestComputation(Computation c) {

    auto computeType = (size_t)c.computationType;

    monitorIn();

    int id = nextId++;

    // S'il n'y a pas d'arret et si le buffer est plein pour une des requetes
    if(!stopped and buffer[computeType].size() >= MAX_TOLERATED_QUEUE_SIZE){
        wait(full[computeType]);
    }

    // On arret le programme si l'utilisateur le demande
    if(stopped) {

        // Reveil en cascade de tous les thread qui attendent
        signal(full.at((size_t)ComputationType::A));
        signal(full.at((size_t)ComputationType::B));
        signal(full.at((size_t)ComputationType::C));

        monitorOut();
        throwStopException();
    }

    // On transmet la requet dans le buffer
    buffer.at(computeType).emplace(c,id);
    // enregistre le type de computation pour le calcul
    computationResult.at(computeType).emplace_back(id);

    // On signal aux thread du calculateurs d'un certains type qu'une requete est disponible
    signal(empty[computeType]);

    monitorOut();
    return id;
}

void ComputationManager::abortComputation(int id) {
    monitorIn();

    abortedId.push_back(id);
    //On vérifie si la computation annulée est dans les résultats existant
    auto it = find_if(resultBuffer.begin(),resultBuffer.end(),[id](const Result& result) {return result.getId() == id;});

    if( it != resultBuffer.end()){
        resultBuffer.erase(it);
    }

    // libére la file d'attente correspondante au calcul annulé
    for(unsigned i = 0; i < computationResult.size(); ++i){
        for(unsigned j = 0; j < computationResult.at(i).size(); ++j){
            if(computationResult.at(i).at(j) == id){
                signal(full[i]);
                break;
            }
        }
    }

    signal(resultBufferEmpty);

    monitorOut();
}

Result ComputationManager::getNextResult() {

    int actualresultat;

    monitorIn();

    actualresultat = actualId++;

    // On met le thread en attente si sont resulat n'est pas encore disponible
    while(!stopped and (resultBuffer.empty() or resultBuffer.front().getId() != actualresultat)){
            wait(resultBufferEmpty);
    }

    // On arret le programme si l'utilisateur le demande
    if(stopped) {
        signal(resultBufferEmpty);
        monitorOut();
        throwStopException();
    }

    // On prend le premier resultat et on le supprime du buffer
    Result rtn = resultBuffer.front();
    resultBuffer.erase(resultBuffer.begin());

    monitorOut();
    return rtn;
}

Request ComputationManager::getWork(ComputationType computationType) {

    Request rtn;

    auto computeType = (size_t)computationType;

    monitorIn();

    while (true) {
        // Si le buffer pour un type de requete, on met le thread d'un type en attente
        if(!stopped and buffer.at(computeType).empty()){
            wait(empty.at(computeType));
        }
        // On vérifie si la requete est annuler, si  oui on la supprimer des calculs à effectuer
        if(find(abortedId.begin(), abortedId.end(), buffer.at(computeType).front().getId()) != abortedId.end()){
            buffer.at(computeType).pop();
        }else{
            break;
        }
    }


    // On arret le programme si l'utilisateur le demande
    if(stopped) {
        // Reveil en cascade de tous les thread qui attendent
        signal(empty.at((size_t)ComputationType::A));
        signal(empty.at((size_t)ComputationType::B));
        signal(empty.at((size_t)ComputationType::C));

        monitorOut();
        throwStopException();
    }

    // On prend la requete et on la supprime du buffer
    rtn = buffer.at(computeType).front();
    buffer.at(computeType).pop();

    // On signal aux threads en attente qu'une place s'est libérée pour un Type donnée
    signal(full[computeType]);

    monitorOut();
    return rtn;
}

bool ComputationManager::continueWork(int id) {
    bool isAborted;
    monitorIn();

    // vérifie si le calcul est annulé
    auto it = find(abortedId.begin(), abortedId.end(),id);

    if(it != abortedId.end() || stopped){
        isAborted = false;
    }else{
        isAborted = true;
    }

    monitorOut();
    return isAborted;
}

void ComputationManager::provideResult(Result result) {
    monitorIn();

    resultBuffer.push_back(result);

    // triage apres chaque insertion
    std::sort(resultBuffer.begin(),resultBuffer.end(),
              [](Result a,Result b) ->bool { return a.getId() < b.getId();});


    signal(resultBufferEmpty);

    monitorOut();
}

void ComputationManager::stop() {
    monitorIn();

    stopped = true;

    // boucle for sur get work pour libere les thread qui attend du taff
    signal(empty.at((size_t)ComputationType::A));
    // boucle sur le thread qui attend un resultat
    signal(resultBufferEmpty);
    // boucle sur les thread qui veulent donner un calcule
    signal(full.at((size_t)ComputationType::A));

    monitorOut();
}
