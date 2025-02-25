#include <cassert>

#include "EStore.h"
#include <iostream>

using namespace std;

Item::
Item() : valid(false)
{ }

Item::
~Item()
{ }


EStore::
EStore(bool enableFineMode)
    : fineMode(enableFineMode)
{
    // TODO: Your code here.
    discount=0;
    shippingCost=3;
    if(enableFineMode) {
        for(int i=0;i<INVENTORY_SIZE;i++) {
            smutex_t mut;
            smutex_init(&mut);
            inv_mut.push_back(mut);
        }
    }
    for(int i=0;i<INVENTORY_SIZE;i++) {
        scond_t cond;
        scond_init(&cond);
        inv_cond.push_back(cond);
    }
    //we keep this for when we want to change the shared store vars
    smutex_init(&esm);
    scond_init(&esc_pr);
    scond_init(&esc_inv);
}

EStore::
~EStore()
{
    // TODO: Your code here.
    if(fineModeEnabled()) {
        for(int i=0;i<INVENTORY_SIZE;i++) {
            smutex_destroy(&inv_mut[i]);
        }
    }
    for(int i=0;i<INVENTORY_SIZE;i++) {
        scond_destroy(&inv_cond[i]);
    }
    smutex_destroy(&esm);
    scond_destroy(&esc_pr);
    scond_destroy(&esc_inv);
}



/*
 * ------------------------------------------------------------------
 * buyItem --
 *
 *      Attempt to buy the item from the store.
 *
 *      An item can be bought if:
 *          - The store carries it.
 *          - The item is in stock.
 *          - The cost of the item plus the cost of shipping is no
 *            more than the budget.
 *
 *      If the store *does not* carry this item, simply return and
 *      do nothing. Do not attempt to buy the item.
 *
 *      If the store *does* carry the item, but it is not in stock
 *      or its cost is over budget, block until both conditions are
 *      met (at which point the item should be bought) or the store
 *      removes the item from sale (at which point this method
 *      returns).
 *
 *      The overall cost of a purchase for a single item is defined
 *      as the current cost of the item times 1 - the store
 *      discount, plus the flat overall store shipping fee.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
buyItem(int item_id, double budget)
{
    assert(!fineModeEnabled());
    scond_t *cond=&inv_cond[item_id];
    smutex_lock(&esm);
    if(item_id<0||item_id>INVENTORY_SIZE||!inventory[item_id].valid) {
        smutex_unlock(&esm);
        return;
    }
    while(inventory[item_id].quantity==0 || 
            (inventory[item_id].price*(1-inventory[item_id].discount))*(1-discount)+shippingCost>budget) {
        scond_wait(cond,&esm);
    }
    //we are able to buy
    inventory[item_id].quantity--;
    smutex_unlock(&esm);
    return;
    // TODO: Your code here.
}

/*
 * ------------------------------------------------------------------
 * buyManyItem --
 *
 *      Attempt to buy all of the specified items at once. If the
 *      order cannot be bought, give up and return without buying
 *      anything. Otherwise buy the entire order at once.
 *
 *      The entire order can be bought if:
 *          - The store carries all items.
 *          - All items are in stock.
 *          - The cost of the entire order (cost of items plus
 *            shipping for each item) is no more than the budget.
 *
 *      If multiple customers are attempting to buy at the same
 *      time and their orders are mutually exclusive (i.e., the
 *      two customers are not trying to buy any of the same items),
 *      then their orders must be processed at the same time.
 *
 *      For the purposes of this lab, it is OK for the store
 *      discount and shipping cost to change while an order is being
 *      processed.
 *
 *      The cost of a purchase of many items is the sum of the
 *      costs of purchasing each item individually. The purchase
 *      cost of an individual item is covered above in the
 *      description of buyItem.
 *
 *      Challenge: For bonus points, implement a version of this
 *      method that will wait until the order can be fulfilled
 *      instead of giving up. The implementation should be efficient
 *      in that it should not wake up threads unecessarily. For
 *      instance, if an item decreases in price, only threads that
 *      are waiting to buy an order that includes that item should be
 *      signaled (though all such threads should be signaled).
 *
 *      Challenge: For bonus points, ensure that the shipping cost
 *      and store discount does not change while processing an
 *      order.
 *      READ For this just add another mutex for each, lock when entering this method. We must acquire
 *      the shipping and discount mutex when entering this method, acquire global and respective mutex in set methods
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
buyManyItems(vector<int>* item_ids, double budget)
{
    assert(fineModeEnabled());
    std::vector<int> items=*item_ids;
    for(int i=0;i<(int)items.size();i++) {
        smutex_lock(&inv_mut[items[i]]);
    }
    double total=0;
    bool all_in_stock=true;
    for(int i=0;i<(int)items.size();i++) {
        int item_id=items[i];
        if(inventory[item_id].quantity==0 || !inventory[item_id].valid) {
            all_in_stock=false;
            break;
        }
        total+=(inventory[item_id].price*(1-inventory[item_id].discount))*(1-discount)+shippingCost;
    }
    for(int i=(int)items.size()-1;i>=0;i--) {
        if(all_in_stock&&total<=budget)
            inventory[items[i]].quantity--;
        smutex_unlock(&inv_mut[items[i]]);
    }
    return;
    // TODO: Your code here.
}


/*
 * ------------------------------------------------------------------
 * addItem --
 *
 *      Add the item to the store with the specified quantity,
 *      price, and discount. If the store already carries an item
 *      with the specified id, do nothing.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
addItem(int item_id, int quantity, double price, double discount)
{
    // TODO: Your code here.
    smutex_t *local;
    if(fineModeEnabled()) {
        local=&inv_mut[item_id];
    }
    else 
        local=&esm;
    smutex_lock(local);
    if(inventory[item_id].valid){ //already carried
        smutex_unlock(local);
        return;
    }
    inventory[item_id].quantity=quantity;
    inventory[item_id].price=price;
    inventory[item_id].discount=discount;
    inventory[item_id].valid=true;
    smutex_unlock(local);
    return;
}


/*
 * ------------------------------------------------------------------
 * removeItem --
 *
 *      Remove the item from the store. The store no longer carries
 *      this item. If the store is not carrying this item, do
 *      nothing.
 *
 *      Wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
removeItem(int item_id)
{
    // TODO: Your code here.
    smutex_t *local;
    if(fineModeEnabled()) 
        local=&inv_mut[item_id];
    else
        local=&esm;
    smutex_lock(local);
    if(!inventory[item_id].valid) {
        smutex_unlock(local);
        return;
    }
    inventory[item_id].valid=false;
    smutex_unlock(local);
    return;
}

/*
 * ------------------------------------------------------------------
 * addStock --
 *
 *      Increase the stock of the specified item by count. If the
 *      store does not carry the item, do nothing. Wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
addStock(int item_id, int count)
{
    // TODO: Your code here.
    smutex_t *local;
    scond_t *cond=&inv_cond[item_id];
    if(fineModeEnabled())
        local=&inv_mut[item_id];
    else 
        local=&esm;
    smutex_lock(local);
    if(!inventory[item_id].valid) {
        smutex_unlock(local);
        return;
    }
    inventory[item_id].quantity+=count;
    scond_broadcast(cond,local);
    smutex_unlock(local);
    return;
}

/*
 * ------------------------------------------------------------------
 * priceItem --
 *
 *      Change the price on the item. If the store does not carry
 *      the item, do nothing.
 *
 *      If the item price decreased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
priceItem(int item_id, double price)
{
    // TODO: Your code here.
    smutex_t *local;
    scond_t *cond=&inv_cond[item_id];
    if(fineModeEnabled())
        local=&inv_mut[item_id];
    else 
        local=&esm;
    smutex_lock(local);
    if(!inventory[item_id].valid) {
        smutex_unlock(local);
        return;
    }
    double prev_price=inventory[item_id].price;
    inventory[item_id].price=price;
    if(price<prev_price) {
        scond_broadcast(cond,local);
    }
    smutex_unlock(local);
    return;
}

/*
 * ------------------------------------------------------------------
 * discountItem --
 *
 *      Change the discount on the item. If the store does not carry
 *      the item, do nothing.
 *
 *      If the item discount increased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
discountItem(int item_id, double discount)
{
    // TODO: Your code here.
    smutex_t *local;
    scond_t *cond=&inv_cond[item_id];
    if(fineModeEnabled())
        local=&inv_mut[item_id];
    else 
        local=&esm;
    smutex_lock(local);
    if(!inventory[item_id].valid) {
        smutex_unlock(local);
        return;
    }
    double prev_discount=inventory[item_id].discount;
    inventory[item_id].discount=discount;
    if(discount>prev_discount) {
        scond_broadcast(cond,local);
    }
    smutex_unlock(local);
    return;
}

/*
 * ------------------------------------------------------------------
 * setShippingCost --
 *
 *      Set the per-item shipping cost. If the shipping cost
 *      decreased, wake any waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
setShippingCost(double cost)
{
    // TODO: Your code here.
    smutex_lock(&esm);
    shippingCost=cost;
    smutex_unlock(&esm);
    return;
}

/*
 * ------------------------------------------------------------------
 * setStoreDiscount --
 *
 *      Set the store discount. If the discount increased, wake any
 *      waiters.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void EStore::
setStoreDiscount(double discount)
{
    // TODO: Your code here.
    smutex_lock(&esm);
    discount=discount;
    smutex_unlock(&esm);
    return;
}