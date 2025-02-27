#include "RequestHandlers.h"
//why no provided includes :(
/*
 * ------------------------------------------------------------------
 * add_item_handler --
 *
 *      Handle an AddItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */


void 
add_item_handler(void *args)
{
    // TODO: Your code here.
    struct AddItemReq *req=(AddItemReq*)args;
    printf("Handling AddItemReq: item_id - %d, quantity - %d, price - $%0.2lf, discount - %0.2lf\n",req->item_id,
                req->quantity,req->price,req->discount);
    req->store->addItem(req->item_id,req->quantity,req->price,req->discount);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * remove_item_handler --
 *
 *      Handle a RemoveItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
remove_item_handler(void *args)
{
    // TODO: Your code here.
    struct RemoveItemReq *req=(RemoveItemReq*)args;
    printf("Handling RemoveItemReq: item_id - %d\n",req->item_id);
    req->store->removeItem(req->item_id);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * add_stock_handler --
 *
 *      Handle an AddStockReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
add_stock_handler(void *args)
{
    // TODO: Your code here.
    struct AddStockReq* req=(AddStockReq*)args;
    printf("Handling AddStockReq: item_id - %d, additional_stock - %d\n",req->item_id,req->additional_stock);
    req->store->addStock(req->item_id,req->additional_stock);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * change_item_price_handler --
 *
 *      Handle a ChangeItemPriceReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
change_item_price_handler(void *args)
{
    // TODO: Your code here.
    struct ChangeItemPriceReq *req=(ChangeItemPriceReq*)args;
    printf("Handling ChangeItemPriceReq: item_id - %d, new_price - $%0.2lf\n",req->item_id,req->new_price);
    req->store->priceItem(req->item_id,req->new_price);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * change_item_discount_handler --
 *
 *      Handle a ChangeItemDiscountReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
change_item_discount_handler(void *args)
{
    // TODO: Your code here.
    struct ChangeItemDiscountReq *req=(ChangeItemDiscountReq*)args;
    printf("Handling ChangeItemDiscountReq: item_id - %d, new_discount - %0.2lf\n",req->item_id,req->new_discount);
    req->store->discountItem(req->item_id,req->new_discount);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * set_shipping_cost_handler --
 *
 *      Handle a SetShippingCostReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
set_shipping_cost_handler(void *args)
{
    // TODO: Your code here.
    struct SetShippingCostReq *req=(SetShippingCostReq*)args;
    printf("Handling SetShippingCostReq: new_cost - $%0.2lf\n",req->new_cost);
    req->store->setShippingCost(req->new_cost);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * set_store_discount_handler --
 *
 *      Handle a SetStoreDiscountReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
set_store_discount_handler(void *args)
{
    // TODO: Your code here.
    struct SetStoreDiscountReq *req=(SetStoreDiscountReq*)args;
    printf("Handling setStoreDiscount: new_discount - %0.2lf\n",req->new_discount);
    req->store->setStoreDiscount(req->new_discount);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * buy_item_handler --
 *
 *      Handle a BuyItemReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
buy_item_handler(void *args)
{
    struct BuyItemReq *req=(BuyItemReq*)args;
    printf("Handling BuyItemReq: item_id - %d, budget $%0.2lf\n",req->item_id,req->budget);
    req->store->buyItem(req->item_id,req->budget);
    delete req;
    return;
    // TODO: Your code here.
}

/*
 * ------------------------------------------------------------------
 * buy_many_items_handler --
 *
 *      Handle a BuyManyItemsReq.
 *
 *      Delete the request object when done.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void
buy_many_items_handler(void *args)
{
    // TODO: Your code here.
    struct BuyManyItemsReq* req=(BuyManyItemsReq*)args;
    printf("Handling BuyManyItemsReq: ");
    for(int i=0;i<(int)req->item_ids.size();i++) {
        printf("%d ",req->item_ids[i]);
    }
    printf("\n");
    req->store->buyManyItems(&req->item_ids,req->budget);
    delete req;
    return;
}

/*
 * ------------------------------------------------------------------
 * stop_handler --
 *
 *      The thread should exit.
 *
 * Results:
 *      None.
 *
 * ------------------------------------------------------------------
 */
void 
stop_handler(void* args)
{
    // TODO: Your code here.
    printf("Handling StopHandlerReq: QUITTING\n");
    sthread_exit();  
}

