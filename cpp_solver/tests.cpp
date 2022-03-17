#include "tests.h"


void testRandomInputGeneration() {
    for (int i = 0; i < 5; ++i) {
        Input in(10, i);
        std::cout << in << std::endl;
    }
}

void testRandomInputNaiveSolve(int inputSize, int seed) {
    Input in(inputSize, seed);
    NaiveSolver ns(in);
    Output out = ns.solve(false);

    std::cout << in << "\n\n\n\n\n";
    if (out.isValidFor(in)) {
        std::cout << "valid" << std::endl;
    } else {
        std::cout << "invalid, cropping" << std::endl;
        if (out.trim(in)) {
            std::cout << "crop success!" << std::endl;
            std::cout << "cropped task list: " << std::endl;
        } else {
            std::cout << "crop failed, contains out-of-bounds indices!" << std::endl;
            exit(1);
        }
    }
    std::cout << out << "\n\n";
}

void testSASolveRandomSmallInputs(int inputSize) {
    double delta = 0;
    for (int seed = 0; seed < 1000; ++seed) {
        Input in(inputSize, seed);
        SASolver sas(in);
        NaiveSolver ns(in);
        Output saResult = sas.solve(seed);
        Output nsResult = ns.solve();
        double saProfit = saResult.evaluate(in);
        double nsProfit = nsResult.evaluate(in);
        delta += nsProfit - saProfit;
    }
    std::cout << "Average delta == " << delta / 1000 << std::endl;
}
