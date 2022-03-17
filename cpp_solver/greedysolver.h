#ifndef GREEDYSOLVER_H
#define GREEDYSOLVER_H
#include "input.h"
#include "output.h"

class GreedySolver
{
private:
    Input input;

public:
    GreedySolver();
    GreedySolver(const Input& in);

    Output solve();
    Output solveLeastOverdue();
    Output solveMostProfitable();
    Output solveDeadline();
    Output solveProfit();
    Output solveProfitRate();
    Output solveDuration();

private:
    template<class T>
    struct Comparator {
        const std::vector<T> & value_vector;
        Comparator(const std::vector<T>& val_vec): value_vector(val_vec) {}
        bool operator()(T i1, T i2) {
            return value_vector[i1] < value_vector[i2];
        }
    };

    int maxProfitIndex(int time) const;
    int leastOverdueIndex(int time) const;
    void resetTaken();
};

#endif // GREEDYSOLVER_H
