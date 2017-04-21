/*
 * CyclicBatch.h
 *
 *  Created on: Apr 15, 2017
 *      Author: innakha
 */

#ifndef UTILFUNCTIONS_CYCLICBATCH_CYCLICBATCH_H_
#define UTILFUNCTIONS_CYCLICBATCH_CYCLICBATCH_H_

#include <stdint.h>
#include <stdbool.h>


#define MAX_BATCH_SIZE      (5)


typedef struct {
    uint32_t    batchSize;
    uint32_t    currentNumElements;
    uint32_t    oldestElementIndex;
    int32_t*    elementsArray;

} CyclicBatchStruct;

void CyclicBatchConstruct(   CyclicBatchStruct* batch,
                             uint32_t batchSize,
                             int32_t* elementsArray);

// Adds the new element to the elementArray
// If the array is full, new element overrides the oldest.
// Otherwise, adds the new element in the next available array slot
void CyclicBatchAddElement( CyclicBatchStruct* batch, int32_t newElement );

bool IsBatchFull( CyclicBatchStruct* batch );

#endif /* UTILFUNCTIONS_CYCLICBATCH_CYCLICBATCH_H_ */
