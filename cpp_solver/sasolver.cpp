#include "sasolver.h"
#include <algorithm>
#include <thread>

SASolver::SASolver() {}

///
/// \brief Initializes a solver instance using the Input.
/// \param in: Problem is specified by this Input.
///
SASolver::SASolver(const Input& in) {
    input = in;
    uniformTaskNumDist = std::uniform_int_distribution<int>(0, in.size() - 1);
    uniformRealDist = std::uniform_real_distribution<double>(0.0, 1.0);
}

///
/// \brief Solves the problem the the seed and settings specified.
/// \param seed: Seed for pseudo-random number generator.
/// \param s: settings for the solver.
/// \return The best task sequence found by the algorithm
///         specified by an untrimmed Output.
///
Output SASolver::solve(int seed, Settings s) {
    if (s.verbose) {
        std::cout << "seed == " << seed << '\n';
    }
    std::vector<std::mt19937_64> gens(NUM_THREADS);
    std::vector<std::thread> threads;
    std::vector<Output> bestSequences(NUM_THREADS);
    for (int tid = 0; tid < NUM_THREADS; ++tid) {
        gens[tid].seed(seed + tid);
        threads.emplace_back(std::thread(&SASolver::solveThread,
                                         *this,
                                         std::ref(bestSequences[tid]),
                                         std::ref(gens[tid]),
                                         s));
    }
    for (int tid = 0; tid < NUM_THREADS; ++tid) {
        threads[tid].join();
    }
    return bestSequence(bestSequences);
}

///
/// \brief Solves a single thread using restart and assigns the result to bestSequence.
/// \param bestSequence: Assigned to be the result upon completion.
/// \param gen: Random generator.
/// \param s: Settings for the solver.
///
void SASolver::solveThread(Output &bestSequence, std::mt19937_64& gen, const Settings& s) {
    Output currSequence = generateRandomSequence(gen);
    bestSequence = currSequence;
    double maxProfit = currSequence.evaluate(input);
    for (int restart = -1; restart < s.maxRestarts; ++restart) {
        solveInstance(currSequence, gen, s);
        double currProfit = currSequence.evaluate(input);
        if (currProfit > maxProfit) {
            maxProfit = currProfit;
            bestSequence = currSequence;
        }
    }
}

///
/// \brief Solves an instance without restarting.
/// \param sequence: Assigned to be the result upon completion.
/// \param gen: Random generator.
/// \param s: Settings for the solver.
///
void SASolver::solveInstance(Output& sequence, std::mt19937_64& gen, Settings s) {
    if (s.verbose) {
        std::cout << "---------------- SIMULATED ANNEALING SOLVE BEGIN ----------------\n";
    }
    int n = input.size();               // Number of tasks
    int L = s.epochSizeFactor * n * n;  // Size of (number of perturbations in) each epoch

    double currProfit = sequence.evaluate(input);
    double temperature = getInitTemperature(sequence, gen, s.initAccRate);
    int index1, index2;
    int epoch = 0;
    double lastEpochProfit = currProfit;
    int rejectionCount = 0;

    if (s.verbose) {
        std::cout << "\nSequence ramdomly initialized to...\n";
        std::cout << sequence << '\n';
        std::cout << "Printing Parameters... \n";
        std::cout << s;
        std::cout << "Input size == " << input.size() << '\n';
        std::cout << "Profit gain threshold == " << PROFIT_GAIN_THRESH << '\n';
        std::cout << "with current profit == " << currProfit << '\n';
        std::cout << "Initial temperature == " << temperature << '\n';
        std::cout << "Epoch size == " << L << '\n';
        std::cout << "BEGIN COOLING...\n";
    }

    // While the system is not frozen
    while (rejectionCount < s.maxRejections) {
        for (int i = 0; i < L; ++i) {
            double newProfit = perturb(sequence, index1, index2, gen);  // Perturb the system to get a random neiboring state
            double acceptanceProb = accProb(-currProfit, -newProfit, temperature);
            if (acceptanceProb < uniformRealDist(gen)) {
                sequence.swapTasks(index1, index2);
            } else {
                currProfit = newProfit;
            }
        }
        if (s.verbose && epoch % s.epochPrintPeriod == 0) {
            std::cout << "\nEpoch " << epoch << " done.\n";
            std::cout << "Current profit == " << currProfit << '\n';
            std::cout << "Profit from last epoch == " << lastEpochProfit << '\n';
        }
        temperature *= s.alpha; // Decrease temperature after each epoch
        if (currProfit - lastEpochProfit < PROFIT_GAIN_THRESH) {    // Increment rejection count if this epoch is rejected
            ++rejectionCount;
            if (s.verbose && epoch % s.epochPrintPeriod == 0) {
                std::cout << "Profit gain smaller than profitGainThresh, epoch rejected.\n";
                std::cout << "Current rejection count == " << rejectionCount << '\n';
            }
        } else {                                                    // Reset rejection count otherwise
            rejectionCount = 0;
            if (s.verbose && epoch % s.epochPrintPeriod == 0) {
                std::cout << "Profit gain detected, epoch accepted. Rejection count reset to 0.\n";
            }
        }
        lastEpochProfit = currProfit;
        ++epoch;
    }
    if (s.verbose) {
        std::cout << "SYSTEM FROZEN, COOLING PROCESS DONE.\n";
        std::cout << "Printing current task sequence...\n" << sequence << '\n';
        std::cout << "Final profit == " << currProfit << '\n';
        std::cout << "---------------- SIMULATED ANNEALING SOLVE DONE ----------------\n";
    }
}

///
/// \brief Get an approximation of initial temperature that achieves the initial acceptance rate
///        for downhill movements.
/// \param output: Specifies the current task sequence.
/// \param gen: Random genreator.
/// \param initAccRate: Target acceptance rate.
/// \return Initial temperature.
///
double SASolver::getInitTemperature(Output &output, std::mt19937_64 &gen, double initAccRate) {
    int n = input.size();
    int L = INIT_TEMP_SAMPLE_SIZE_FACTOR * n * n;
    int count = 0;
    double delta = 0.0;
    int index1, index2;
    double currProfit = output.evaluate(input);

    // Randomly perturb the current state L times to find the average decrease in profit.
    for (int i = 0; i < L; ++i) {
        double newProfit = perturb(output, index1, index2, gen);
        if (newProfit < currProfit) {
            ++count;
            delta += currProfit - newProfit;
        }
        output.swapTasks(index1, index2);
    }
    return delta / count / std::log(1 / initAccRate);
}

///
/// \brief Returns the probability of accepting the new state given the
///        energies. If the energy of the new state is lower than the
///        old state, the probability is 1.0 (always accepted).
///        Otherwise, the probability is calculated based on the
///        temperature.
/// \param eOld: Energy of the old state.
/// \param eNew: Energy of the new state.
/// \param t: Current temperature.
/// \return Probability of accepting the new state.
///
double SASolver::accProb(double eOld, double eNew, double t) const {
    return eNew < eOld ? 1.0 : exp((eOld - eNew) / t);
}

///
/// \brief Perturb the current task sequence by swaping two random indices.
/// \param currOutput: Current task sequence.
/// \param index1: Assigned to the first chosen index in Output sequence being swapped.
/// \param index2: Assigned to the second chosen index in Output sequence being swapped.
/// \param gen: Random generator.
/// \return Profit of the new task sequence after perturbation.
///
double SASolver::perturb(Output &currOutput, int &index1, int &index2, std::mt19937_64 &gen) {
    index1 = uniformTaskNumDist(gen);
    index2 = uniformTaskNumDist(gen);
    while (index2 == index1) {
        index2 = uniformTaskNumDist(gen);
    }
    currOutput.swapTasks(index1, index2);
    return currOutput.evaluate(input);
}

///
/// \brief Generates a random task sequence for the Input.
/// \param gen: Random generator.
/// \return Random task sequence specified by an Output.
///
Output SASolver::generateRandomSequence(std::mt19937_64 &gen) {
    std::vector<int> taskSequence;
    for (int i = 0; i < input.size(); ++i) {
        taskSequence.push_back(i);
    }
    std::shuffle(taskSequence.begin(), taskSequence.end(), gen);
    return Output(taskSequence);
}

///
/// \brief Returns the sequence with maximum profit in a std::vector of sequences.
/// \param sequences: A vector of candidate sequences.
/// \return Sequence with max profit in vector.
///
Output SASolver::bestSequence(const std::vector<Output> &sequences) const {
    double maxProfit = -1;
    Output res;
    for (const Output& seq : sequences) {
        double profit = seq.evaluate(input);
        if (profit > maxProfit) {
            maxProfit = profit;
            res = seq;
        }
    }
    return res;
}



