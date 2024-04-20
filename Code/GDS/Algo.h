#ifndef MLGDENSEST_ALGO_H
#define MLGDENSEST_ALGO_H


#include "../Header.h"

typedef unsigned int NODE_TYPE;
typedef unsigned int EDGE_TYPE;
typedef unsigned int LAYER_TYPE;

NODE_TYPE findTopK(const NODE_TYPE *degreeList, LAYER_TYPE layerNum, LAYER_TYPE k);

#endif //MLGDENSEST_ALGO_H
