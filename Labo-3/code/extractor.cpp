// Fichier              :   extractor.cpp
// Date modification    :   02.11.2023
// Auteurs              :   Auberson Kevin, Maillard Patrick
// Modification         :   implémentation de la fonction trade qui permet
//                          à un autre thread d’essayer d’effectuer un achat
//                          et run mise en place de mutex

#include "extractor.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcomutex.h>

extern bool Run;
PcoMutex mtxExtractorMoney;
PcoMutex mtxExtractorStocks;

WindowInterface* Extractor::interface = nullptr;

Extractor::Extractor(int uniqueId, int fund, ItemType resourceExtracted)
    : Seller(fund, uniqueId), resourceExtracted(resourceExtracted), nbExtracted(0)
{
    assert(resourceExtracted == ItemType::Copper ||
           resourceExtracted == ItemType::Sand ||
           resourceExtracted == ItemType::Petrol);
    interface->consoleAppendText(uniqueId, QString("Mine Created"));
    interface->updateFund(uniqueId, fund);
}

std::map<ItemType, int> Extractor::getItemsForSale() {
    return stocks;
}

/**
 * @brief Extractor::trade
 * @param it type de ressource à coommander
 * @param qty le nombre de ressource
 * @return la commande
 */
int Extractor::trade(ItemType it, int qty) {
    // TODO
    if(getItemsForSale()[it] >= qty && qty > 0){
        stocks[it] -= qty;
        int order = getCostPerUnit(it)*qty;
        interface->consoleAppendText(uniqueId, QString("A trade of %1 of ").arg(qty) % getItemName(it) % QString("was made"));
        money += order;
        return order;
    }
    interface->consoleAppendText(uniqueId, QString("I don't have enough ressources to trade"));
    return 0;
}

void Extractor::run() {
    interface->consoleAppendText(uniqueId, "[START] Mine routine");

    while (Run) {
        /* TODO concurrence */

        int minerCost = getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
        mtxExtractorMoney.lock();
        if (money < minerCost) {
            /* Pas assez d'argent */
            /* Attend des jours meilleurs */
            PcoThread::usleep(1000U);
            mtxExtractorMoney.unlock();
            continue;
        }

        /* On peut payer un mineur */
        money -= minerCost;
        mtxExtractorMoney.unlock();
        /* Temps aléatoire borné qui simule le mineur qui mine */
        PcoThread::usleep((rand() % 100 + 1) * 10000);
        mtxExtractorStocks.lock();
        /* Statistiques */
        nbExtracted++;
        /* Incrément des stocks */
        stocks[resourceExtracted] += 1;
        mtxExtractorStocks.unlock();
        /* Message dans l'interface graphique */
        interface->consoleAppendText(uniqueId, QString("1 ") % getItemName(resourceExtracted) %
                                     " has been mined");
        /* Update de l'interface graphique */
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);      
    }
    interface->consoleAppendText(uniqueId, "[STOP] Mine routine");
}

int Extractor::getMaterialCost() {
    return getCostPerUnit(resourceExtracted);
}

ItemType Extractor::getResourceMined() {
    return resourceExtracted;
}

int Extractor::getAmountPaidToMiners() {
    return nbExtracted * getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
}

void Extractor::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

SandExtractor::SandExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Sand) {}

CopperExtractor::CopperExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Copper) {}

PetrolExtractor::PetrolExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Petrol) {}
