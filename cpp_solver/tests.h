#ifndef TESTS_H
#define TESTS_H
#include "naivesolver.h"
#include "sasolver.h"
#include "greedysolver.h"

void testRandomInputGeneration();
void testRandomInputNaiveSolve(int inputSize, int seed = 0);
void testSASolveRandomSmallInputs(int inputSize);

#endif // TESTS_H
