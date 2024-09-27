// Fichier              :   wholesale.cpp
// Date modification    :   02.11.2023
// Auteurs              :   Auberson Kevin, Maillard Patrick
// Modification         :   implémentation de la fonction trade qui permet
//                          aux usines d’acheter des objets aux grossistes
//                          et buyResources qui est la routine d’achat de
//                          ressources produites par les extracteurs et
//                          les usines

#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcomutex.h>

WindowInterface* Wholesale::interface = nullptr;
extern bool Run;
PcoMutex mtxWholesale;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

void Wholesale::setSellers(std::vector<Seller*> sellers) {
    this->sellers = sellers;

    for(Seller* seller: sellers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}
/**
 * @brief Wholesale::buyResources
 */
void Wholesale::buyResources() {
    auto s = Seller::chooseRandomSeller(sellers);
    auto m = s->getItemsForSale();
    auto i = Seller::chooseRandomItem(m);

    if (i == ItemType::Nothing) {
        /* Nothing to buy... */
        return;
    }

    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(i);

    interface->consoleAppendText(
                uniqueId,
                QString("I would like to buy %1 of ").arg(qty) %getItemName(i)%
                QString(" which would cost me %1").arg(price));

    mtxWholesale.lock();
    if(m[i] >= qty && price <= money){
        s->trade(i, qty);
        money -= price;
        stocks[i] += qty;
    }
    mtxWholesale.unlock();
}

void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr << "You have to give factories and mines to a \
            wholeseler before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (Run) {
        buyResources();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);

        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 100000);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");
}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

/**
 * @brief Wholesale::trade
 * @param it type de ressource à coommander
 * @param qty le nombre de ressource
 * @return la commande
 */
int Wholesale::trade(ItemType it, int qty) {
    if(getItemsForSale()[it]>= qty && qty > 0){
        stocks[it] -= qty;
        int order = getCostPerUnit(it)*qty;
        money += order;
        return order;
    }
    return 0;
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
