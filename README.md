# CS 170 Project Fall 2021

Tested on `gcc version 7.3.0 (x86_64-posix-seh-rev0, Built by MinGW-W64 project)` on Windows 10 and `gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)` on hive machines.

Compilation: inside `cpp_solver`, run:

```
g++ -pthread -o solve *.cpp
```

Make sure that `inputs` and `outputs` folders created with proper subfolder structures, and are under the same directory as `cpp_solver`. Particularly, the folder `outputs` as well as `large`, `medium`, `small`, and `log` folders inside `outputs` must be created before the program is run. 

Then, to start solving, run the following command inside `cpp_solver`:

```
./solve
```

The parameters are set to reproduce the outputs submitted (but only to a certain degree of accuracy see **Note** below for detail), and may take days to run. To speed up the process at an expense of optimality, change settings in main.cpp by decreasing alpha, maxRejections, epochSizeFactor, and initAccRate; maxRestarts is set to 0 by default as restarting gives minimal profit gain.

**Note on the implementation for SASolver::getInitTemperature**

There was a bug in the version that we used to generate most of the outputs. The function was written as follows.

```
double SASolver::getInitTemperature(Output &output, std::mt19937_64 &gen, double initAccRate) {
    int n = input.size();
    int L = INIT_TEMP_SAMPLE_SIZE_FACTOR * n * n;
    int posCount = 0;
    int negCount = 0;
    double posDelta = 0.0;
    double negDelta = 0.0;
    int index1, index2;
    double currProfit = output.evaluate(input);
    for (int i = 0; i < L; ++i) {
        double newProfit = perturb(output, index1, index2, gen);
        if (newProfit > currProfit) {
            ++posCount;
            posDelta += newProfit - currProfit;
        } else {
            ++negCount;
            negDelta += currProfit - newProfit;
        }
        output.swapTasks(index1, index2);
    }
    if (negCount > 10 * posCount) {
        return negDelta / negCount / std::log(1 / initAccRate);
    } else {
        return posDelta / posCount / std::log(1 / initAccRate);
    }
}
```

In an attempt to cover an edge case in which the starting sequence happens to be a local minimum in terms of profit. The function should use the average increase in profit to calculate the initial temperature, so the function should instead end with

```
    if (posCount > 10 * negCount) {
        return posDelta / posCount / std::log(1 / initAccRate);
    } else {
        return negDelta / negCount / std::log(1 / initAccRate);
    }
```

The bug was not discovered until most of the computation was done over a couple of days and there was not enough time to regenerate new outputs. Luckily, it turned out that the bug did not negatively affect the performance by a huge amount.

To reproduce the output that are submitted, replace the implementation in "sasolver.cpp" with this erroneous implementation and rebuild.















