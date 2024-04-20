#include "Algo.h"

inline void swap(NODE_TYPE a[], LAYER_TYPE i, LAYER_TYPE j) {
    NODE_TYPE temp = a[i];
    a[i] = a[j];
    a[j] = temp;
}

inline void refreshHeapAfterInsert(NODE_TYPE *heap, LAYER_TYPE heapSize) {
    LAYER_TYPE father = 0;
    LAYER_TYPE selectedChild, leftChild, rightChild;
    while (father < heapSize) {
        leftChild = 2 * father + 1;
        rightChild = 2 * father + 2;
        if (leftChild >= heapSize) {
            break;
        } else {
            if (rightChild >= heapSize) {
                if (heap[father] > heap[leftChild]) {
                    swap(heap, father, leftChild);
                }
                break;
            } else {
                selectedChild = heap[leftChild] > heap[rightChild] ? rightChild : leftChild;
                if (heap[father] > heap[selectedChild]) {
                    swap(heap, father, selectedChild);
                    father = selectedChild;
                } else {
                    break;
                }
            }
        }
    }
}

inline NODE_TYPE findByHeap(const NODE_TYPE *degreeList, LAYER_TYPE layerNum, LAYER_TYPE k) {
    NODE_TYPE i = 0;
    NODE_TYPE tempLargestHeap[k];
    for (i = 0; i < k; i++) {
        tempLargestHeap[i] = 0;
    }
    for (i = 0; i < layerNum; i++) {
        if (degreeList[i] > tempLargestHeap[0]) {
            tempLargestHeap[0] = degreeList[i];
            refreshHeapAfterInsert(tempLargestHeap, k);
        }
    }
    return tempLargestHeap[0];
}

NODE_TYPE findTopK(const NODE_TYPE *degreeList, LAYER_TYPE layerNum, LAYER_TYPE k) {
    return findByHeap(degreeList, layerNum, k);
}

