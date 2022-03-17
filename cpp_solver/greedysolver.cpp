#include "greedysolver.h"
#include <cmath>
#include <numeric>
#include <algorithm>

GreedySolver::GreedySolver() {}

GreedySolver::GreedySolver(const Input &in) {
    input = in;
}

///
/// \brief Solves using all possible greedy algorithms below.
/// \return Schedule with max profit.
///
Output GreedySolver::solve() {
    double maxProfit = 0.0;
    Output best;

    Output out = solveDeadline();
    double currProfit = out.evaluate(input);
    if (currProfit > maxProfit) {
        maxProfit = currProfit;
        best = out;
    }

    out = solveDuration();
    currProfit = out.evaluate(input);
    if (currProfit > maxProfit) {
        maxProfit = currProfit;
        best = out;
    }

    out = solveLeastOverdue();
    currProfit = out.evaluate(input);
    if (currProfit > maxProfit) {
        maxProfit = currProfit;
        best = out;
    }

    out = solveMostProfitable();
    currProfit = out.evaluate(input);
    if (currProfit > maxProfit) {
        maxProfit = currProfit;
        best = out;
    }

    out = solveProfit();
    currProfit = out.evaluate(input);
    if (currProfit > maxProfit) {
        maxProfit = currProfit;
        best = out;
    }

    out = solveProfitRate();
    currProfit = out.evaluate(input);
    if (currProfit > maxProfit) {
        maxProfit = currProfit;
        best = out;
    }
    return best;
}

Output GreedySolver::solveLeastOverdue() {
    resetTaken();
    // keep picking the task that causes least number of tasks to become overdue
    int time = 0;
    std::vector<int> sequence(input.size());
    for (int i = 0; i < input.size(); ++i) {
        sequence[i] = leastOverdueIndex(time);
        time += input.getDuration(sequence[i]);
        input.setTaken(sequence[i]);
    }
    return Output(sequence);
}

Output GreedySolver::solveMostProfitable() {
    resetTaken();
    std::vector<int> sequence;
    int time = 0;
    for (int i = 0; i < input.size(); ++i) {
        int bestTaskIndex = maxProfitIndex(time);
        sequence.push_back(bestTaskIndex);
        input.setTaken(bestTaskIndex);
        time += input.getDuration(bestTaskIndex);
    }
    return Output(sequence);
}

Output GreedySolver::solveDeadline() {
    // sort the tasks by ascending deadline order
    std::vector<int> sequence(input.size());
    std::iota(sequence.begin(), sequence.end(), 0);
    std::sort(sequence.begin(), sequence.end(), Comparator<int>(input.getDeadlines()));
    return Output(sequence);
}

Output GreedySolver::solveProfit() {
    // sort the tasks by descending profit order
    std::vector<int> sequence(input.size());
    std::iota(sequence.begin(), sequence.end(), 0);
    std::sort(sequence.rbegin(), sequence.rend(), Comparator<double>(input.getProfits()));
    return Output(sequence);
}

Output GreedySolver::solveProfitRate() {
    // sort the tasks by descending profit rate order, disregarding deadlines
    std::vector<double> profitRates(input.size());
    std::vector<int> sequence(input.size());
    for (int i = 0; i < input.size(); ++i) {
        profitRates[i] = input.getProfit(i) / input.getDuration(i);
    }
    std::iota(sequence.begin(), sequence.end(), 0);
    std::sort(sequence.rbegin(), sequence.rend(), Comparator<double>(profitRates));
    return Output(sequence);
}

Output GreedySolver::solveDuration() {
    // sort the tasks by ascending duration order
    std::vector<int> sequence(input.size());
    std::iota(sequence.begin(), sequence.end(), 0);
    std::sort(sequence.begin(), sequence.end(), Comparator<int>(input.getDurations()));
    return Output(sequence);
}

int GreedySolver::maxProfitIndex(int time) const {
    double maxProfit = -INFINITY;
    double profit;
    int index = -1;
    for (int i = 0; i < input.size(); ++i) {
        if (input.isTaken(i)) {
            continue;
        }
        int overtime = time + input.getDuration(i) - input.getDeadline(i);
        if (overtime > 0) {
            profit = input.getProfit(i) * exp(-0.017 * overtime);
        } else {
            profit = input.getProfit(i);
        }
        if (profit > maxProfit) {
            maxProfit = profit;
            index = i;
        }
    }
    return index;
}

int GreedySolver::leastOverdueIndex(int time) const {
    int overdueCount;
    int minOverdueCount = -1;
    int res = -1;
    for (int i = 0; i < input.size(); ++i) {
        overdueCount = 0;
        if (input.isTaken(i)) {
            continue;
        }
        int finishTime = time + input.getDuration(i);
        for (int j = 0; j < input.size(); ++j) {
            if (i == j || input.isTaken(j)) {
                continue;
            } else if (finishTime > input.getDeadline(j)) {
                ++overdueCount;
            }
        }
        if (overdueCount < minOverdueCount || minOverdueCount == -1) {
            minOverdueCount = overdueCount;
            res = i;
        }
    }
    return res;
}

void GreedySolver::resetTaken() {
    for (int i = 0; i < input.size(); ++i) {
        input.setTaken(i, false);
    }
}











