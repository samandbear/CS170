#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include "sasolver.h"
#include "greedysolver.h"

using namespace std;

const string INPUT_DIR = "../inputs/";
const string LARGE_DIR = "large/";
const string MEDIUM_DIR = "medium/";
const string SMALL_DIR = "small/";
const string OUTPUT_DIR = "../outputs/";
const string LOG_DIR = "../outputs/log/";
const string LARGE_PREFIX = "large-";
const string MEDIUM_PREFIX = "medium-";
const string SMALL_PREFIX = "small-";
const string INPUT_POSTFIX = ".in";
const string OUTPUT_POSTFIX = ".out";
const string LOG_POSTFIX = ".log";

// Change settings here for different performance.
/* Usage: SASolver::Settings(int maxRestarts,
 *                           double alpha,
 *                           int maxRejections,
 *                           double epochSizeFactor,
 *                           double initAccRate,
 *                           bool verbose,
 *                           int epochPrintPeriod)
 *
 * maxRestarts: number of restarts to perform after system frozen
 * alpha: temperature decay rate, must be a fraction between 0.0 and 1.0 (exclusive)
 * maxRejections: number of epochs allowed with no profit gain
 * epochSizeFactor: Size of each epoch == epochSizeFactor * n * n
 * initAccRate: target initial acceptance rate for downhill movements
 */
const SASolver::Settings settings(0, 0.999, 50, 1.0, 0.8);

void solveBatch(const string& inDir, const string& outDir, const string& logDir, const string& prefix);
void solveAll();
void fixBatch(const string& inDir, const string& outDir, const string& logDir, const string& prefix);
void fixAll();
double greedyProfit(const Input& in, Output& out);


//-----------------------------------------------------------------------------

int main() {
    solveAll();
    fixAll();
    return 0;
}

//----------------------------------------------------------------------------

void solveBatch(const string& inDir, const string& outDir, const string& logDir, const string& prefix) {
    fstream fs;
    Input in;
    Output out;
    std::time_t tt;

    for (int i = 1; i <= 300; ++i) {
        string inputFileName = inDir + prefix + to_string(i) + INPUT_POSTFIX;
        string outputFileName = outDir + prefix + to_string(i) + OUTPUT_POSTFIX;
        string logFileName = logDir + prefix + to_string(i) + LOG_POSTFIX;

        // skip if log file already exists
        fs.open(logFileName, fstream::in);
        if (fs.good()) {
            fs.close();
            continue;
        }

        // Read input
        auto start = chrono::system_clock::now();
        in = Input(inputFileName);

        // skip if input file does not exist
        if (in.failed()) {
            continue;
        }

        // Begin solving
        SASolver sas = SASolver(in);
        out = sas.solve(0, settings);
        auto stop = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(stop - start);

        // write log file
        fs.open(logFileName, fstream::out);
        fs << out.evaluate(in) << endl;
        fs << out << endl;
        fs << settings << endl;
        tt = std::chrono::system_clock::to_time_t(start);
        fs << "Start time == " << ctime(&tt);
        tt = std::chrono::system_clock::to_time_t(stop);
        fs << "Stop time == " << ctime(&tt);
        fs << "Elapsed time == " << duration.count() << " seconds\n";
        fs.close();

        // write output file
        out.trim(in);
        out.writeFile(outputFileName);
    }
}

void solveAll() {
    solveBatch(INPUT_DIR + LARGE_DIR, OUTPUT_DIR + LARGE_DIR, LOG_DIR, LARGE_PREFIX);
    solveBatch(INPUT_DIR + MEDIUM_DIR, OUTPUT_DIR + MEDIUM_DIR, LOG_DIR, MEDIUM_PREFIX);
    solveBatch(INPUT_DIR + SMALL_DIR, OUTPUT_DIR + SMALL_DIR, LOG_DIR, SMALL_PREFIX);
}

void fixBatch(const string& inDir, const string& outDir, const string& logDir, const string& prefix) {
    fstream fs;
    Input in;
    Output out;

    for (int i = 1; i <= 300; ++i) {
        string inputFileName = inDir + prefix + to_string(i) + INPUT_POSTFIX;
        string outputFileName = outDir + prefix + to_string(i) + OUTPUT_POSTFIX;
        string logFileName = logDir + prefix + to_string(i) + ".fix" + LOG_POSTFIX;

        // skip if input file does not exist
        in = Input(inputFileName);
        if (in.failed()) {
            continue;
        }

        // evaluate current profit
        out = Output(outputFileName);
        double saProfit = out.evaluate(in);
        GreedySolver gs(in);
        out = gs.solve();
        double gsProfit = out.evaluate(in);
        if (gsProfit > saProfit) {
            cout << "GS better than SA on " << prefix << to_string(i) << ":" << endl;
            cout << out << endl;

            // Overwrite original output

            // write log file
            fs.open(logFileName, fstream::out);
            fs << out.evaluate(in) << endl;
            fs << out << endl;
            fs.close();

            // write output file
            out.trim(in);
            out.writeFile(outputFileName);
        }
    }
}

void fixAll() {
    fixBatch(INPUT_DIR + LARGE_DIR, OUTPUT_DIR + LARGE_DIR, LOG_DIR, LARGE_PREFIX);
    fixBatch(INPUT_DIR + MEDIUM_DIR, OUTPUT_DIR + MEDIUM_DIR, LOG_DIR, MEDIUM_PREFIX);
    fixBatch(INPUT_DIR + SMALL_DIR, OUTPUT_DIR + SMALL_DIR, LOG_DIR, SMALL_PREFIX);
}








