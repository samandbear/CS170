#ifndef NAIVESOLVER_H
#define NAIVESOLVER_H
#include "input.h"
#include "output.h"

class NaiveSolver
{
private:
    Input input;

public:
    NaiveSolver();
    NaiveSolver(const Input& in);

    Output solve(bool verbose = false) const;
};

#endif // NAIVESOLVER_H
