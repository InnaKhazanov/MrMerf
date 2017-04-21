/*
 * CyclicBatch.c
 *
 *  Created on: Apr 15, 2017
 *      Author: innakha
 */

#include "CyclicBatch.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/****************************/
/* Local Definitions        */
/****************************/

/****************************/
/* Local Functions          */
/****************************/

/****************************/
/* Local Variables          */
/****************************/

/****************************/
/* Function Implementation  */
/****************************/

void CyclicBatchConstruct(CyclicBatchStruct* batch, uint32_t batchSize, int32_t* elementsArray)
{
    if( (NULL == batch) || (NULL == elementsArray) || (MAX_BATCH_SIZE < batchSize))
    {
        return;
    }

    batch->batchSize = batchSize;
    batch->currentNumElements = 0;
    batch->oldestElementIndex = 0;
    batch->elementsArray = elementsArray;

}


// Adds the new element to the elementArray
// If the array is full, new element overrides the oldest.
// Otherwise, adds the new element in the next available array slot
void CyclicBatchAddElement(CyclicBatchStruct* batch, int32_t newElement)
{
    if( (NULL == batch) || (NULL == newElement) )
    {
        return;
    }

    if( batch->currentNumElements < batch->batchSize )
    {
        batch->elementsArray[batch->currentNumElements] = newElement;
        batch->currentNumElements++;
    }

    // batch is full, override the oldest element
    else
    {
        batch->elementsArray[batch->oldestElementIndex] = newElement;
        batch->oldestElementIndex = (batch->oldestElementIndex + 1) % (batch->batchSize);
    }
}


bool IsBatchFull( CyclicBatchStruct* batch )
{
    return (batch->currentNumElements == batch->batchSize ) ? true : false;
}
