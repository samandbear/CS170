#include "naivesolver.h"
#include <algorithm>

NaiveSolver::NaiveSolver() {}

NaiveSolver::NaiveSolver(const Input& in) {
    input = in;
}

Output NaiveSolver::solve(bool verbose) const {
    Output best;
    double maxProfit = 0;
    std::vector<int> schedule;
    for (int i = 0; i < input.size(); ++i) {
        schedule.push_back(i);
    }
    int iter = 0;
    do {
        Output o = Output(schedule);
        double profit = o.evaluate(input);
        if (verbose) {
            std::cout << "iter " << iter << ": schedule == " << o;
        }
        if (profit > maxProfit) {
            if (verbose) {
                std::cout << "New profit == " << profit << " > current max profit == " << maxProfit << ", updating max profit.\n\n";
            }
            best = o;
            maxProfit = profit;
        } else {
            if (verbose) {
                std::cout << "New profit == " << profit << " <= current max profit == " << maxProfit << ", no update needed.\n\n";
            }
        }
    } while (std::next_permutation(schedule.begin(), schedule.end()));
    return best;
}
