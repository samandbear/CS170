#ifndef SASOLVER_H
#define SASOLVER_H
#include "input.h"
#include "output.h"
#include <random>

///
/// \brief Simulated annealing solver. Settings can be specified using
///        Settings struct defined inside the class.
/// \note Restarting does not work well in this implementation and the number of restarts
///       should be ignored and always set to zero (0).
///
class SASolver
{
public:
    static const int NUM_THREADS = 8;
    struct Settings {
        int maxRestarts;        // Maximum number of restarts upon completion of each annealing process.
        double alpha;           // Rate of temperature decay.
        int maxRejections;      // Maximum number of epochs to be rejected before the system is deemed frozen.
        double epochSizeFactor; // Number of perturbations in each epoch == epochSizeFactor * numTasks * numTasks.
        double initAccRate;     // Approximate initial tempreature to achieve this initial acceptance rate for downhill movements.
        bool verbose;           // Prints all details if set to true. May not work properly if NUM_THREADS is more than 1.
        int epochPrintPeriod;   // Print epoch summary every this number of epochs.

        Settings(int maxRestarts = 0,
                 double alpha = 0.99,
                 int maxRejections = 50,
                 double epochSizeFactor = 1.0,
                 double initAccRate = 0.8,
                 bool verbose = false,
                 int epochPrintPeriod = 1) {
            this->maxRestarts = maxRestarts;
            this->alpha = alpha;
            this->maxRejections = maxRejections;
            this->epochSizeFactor = epochSizeFactor;
            this->initAccRate = initAccRate;
            this->verbose = verbose;
            this->epochPrintPeriod = epochPrintPeriod;
        }

        friend std::ostream& operator <<(std::ostream& out, const Settings& s) {
            out << "GLOBAL NUM_THREADS == " << SASolver::NUM_THREADS << '\n';
            out << "Max restarts == " << s.maxRestarts << '\n';
            out << "Temperature decay factor == " << s.alpha << '\n';
            out << "Initial acceptance rate == " << s.initAccRate << '\n';
            out << "Epoch size factor == " << s.epochSizeFactor << '\n';
            out << "Max epochs with no profit gain == " << s.maxRejections << '\n';
            return out;
        }
    };

private:
    Input input;
    const double INIT_TEMP_SAMPLE_SIZE_FACTOR = 2.0;    // Number of perturbations to try when determining
                                                        // initial temperature == INIT_TEMP_SAMPLE_SIZE_FACTOR * numTasks * numTasks.
    const double PROFIT_GAIN_THRESH = 1e-3;             // No profit is considered gained if less than this value.

    std::uniform_int_distribution<int> uniformTaskNumDist;
    std::uniform_real_distribution<double> uniformRealDist;

public:
    SASolver();
    SASolver(const Input& in);

    Output solve(int seed = 0, Settings s = Settings());

private:
    void solveThread(Output& bestSequence, std::mt19937_64& gen, const Settings& s);
    void solveInstance(Output& sequence, std::mt19937_64 &gen, Settings s);

    double getInitTemperature(Output &output, std::mt19937_64& gen, double initAccRate = 0.8);
    double accProb(double eOld, double eNew, double t) const;
    double perturb(Output& currOutput, int& index1, int& index2, std::mt19937_64& gen);
    Output generateRandomSequence(std::mt19937_64& gen);
    Output bestSequence(const std::vector<Output>& sequences) const;
};

#endif // SASOLVER_H
