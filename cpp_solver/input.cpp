#include "input.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Input::Input() {}

///
/// \brief Generates a random Input with the number of tasks specified.
/// \param numTasks: number of tasks to include.
/// \param seed: seed for the pseudo-random number generator.
///
Input::Input(int numTasks, int seed) {
    if (numTasks <= 0) {
        return;
    }
    n = numTasks;
    gen.seed(seed);
    for (int i = 0; i < n; ++i) {
        deadlines.push_back(static_cast<int>(uniformRealDist(gen) * std::min(n * 20, MAX_TIME) + 1));
        durations.push_back(static_cast<int>(uniformRealDist(gen) * DURATION_MAX + 1));
        profits.push_back(round(uniformRealDist(gen) * PROFIT_MAX_EXCLUSIVE * 1000.0) / 1000.0);
    }
    taken = std::vector<bool>(n, false);
}

///
/// \brief Reads an Input from a file.
/// \param fileName: Directory to the file.
/// \param preset: Task number preset. If specified to 100, 150, or 200,
///        the constructor will also check for validity of the input.
///        If not specified, the Input is by default valid.
///
Input::Input(const std::string& fileName, int preset) {
    std::fstream fs(fileName, std::fstream::in);

    // Terminate if file does not exist.
    if (!fs.good()) {
        fail = true;
        return;
    }
    fs >> n;
    deadlines = std::vector<int>(n, 0);
    durations = std::vector<int>(n, 0);
    profits = std::vector<double>(n, 0);
    taken = std::vector<bool>(n, false);
    valid = true;
    if (preset > 0) {
        valid = validateTaskNumber(n, preset);
    }
    int taskIndex;
    for (int i = 0; i < n; ++i) {
        fs >> taskIndex >> deadlines[i] >> durations[i] >> profits[i];
        valid = taskIndex == i + 1 && validateTask(deadlines[i], durations[i], profits[i]);
    }
    fs.close();
}

bool Input::failed() const {
    return fail;
}

///
/// \brief Returns if the Input is valid.
/// \return true if the Input is valid, false otherwise.
///
bool Input::isValid() const {
    return valid;
}

///
/// \brief Returns the size of the Input.
/// \return size of the Input.
///
int Input::size() const {
    return n;
}

///
/// \brief Returns the deadline of task i.
/// \param i: index of a task.
/// \return Deadline of task i.
///
int Input::getDeadline(int i) const {
    return deadlines[i];
}

///
/// \brief Returns the duration of task i.
/// \param i: index of a task.
/// \return Duration of task i.
///
int Input::getDuration(int i) const {
    return durations[i];
}

///
/// \brief Returns the profit of task i.
/// \param i: index of a task.
/// \return Profit of task i.
///
double Input::getProfit(int i) const {
    return profits[i];
}

///
/// \brief Returns whether the task has been taken.
/// \param i: index of a task.
/// \return True if task has been taken, false otherwise.
///
bool Input::isTaken(int i) const {
    return taken[i];
}

///
/// \brief Set task i to be taken/untaken.
/// \param i: index of the task
/// \param val: Set taken to be this boolean value.
///
void Input::setTaken(int i, bool val) {
    taken[i] = val;
}

///
/// \brief Get a constant reference to the deadlines vector.
/// \return A constant reference to the deadlines vector.
///
const std::vector<int> &Input::getDeadlines() const {
    return deadlines;
}

///
/// \brief Get a constant reference to the durations vector.
/// \return A constant reference to the durations vector.
///
const std::vector<int> &Input::getDurations() const {
    return durations;
}

///
/// \brief Get a constant reference to the profits vector.
/// \return A constant reference to the profits vector.
///
const std::vector<double> &Input::getProfits() const {
    return profits;
}

///
/// \brief Writes the Input to a file specified by fileName.
/// \param fileName: directory to the output file. If the file
///        already exists, it will be overwritten.
///
void Input::writeFile(const std::string &fileName) const {
    std::fstream fs;
    fs.open(fileName, std::fstream::out);
    fs << n << '\n';
    for (int i = 0; i < n; ++i) {
        fs << i + 1 << ' ' << deadlines[i] << ' ' << durations[i] << ' ' << profits[i] << '\n';
    }
    fs.close();
}

///
/// \brief Validates a task by checking the validity of its deadline, duration,
///        and profit against the project spec.
/// \param deadline: Deadline of the task
/// \param duration: Duration of the task
/// \param profit: Profit of the task.
/// \return True if the parameters are valid, false otherwise.
///
bool Input::validateTask(int deadline, int duration, double profit) const {
    if (deadline < DEADLINE_MIN) {
        std::cout << "Input::validateTask(): invalid deadline " << deadline
                  << " is smaller than " << DEADLINE_MIN << "!\n";
        return false;
    } else if (deadline > DEADLINE_MAX) {
        std::cout << "Input::validateTask(): invalid deadline " << deadline
                  << " is greater than " << DEADLINE_MAX << "!\n";
        return false;
    } else if (duration < DURATION_MIN) {
        std::cout << "Input::validateTask(): invalid duration " << duration
                  << " is smaller than " << DURATION_MIN << "!\n";
        return false;
    } else if (duration > DURATION_MAX) {
        std::cout << "Input::validateTask(): invalid duration " << duration
                  << " is greater than " << DURATION_MAX << "!\n";
        return false;
    } else if (profit <= PROFIT_MIN_EXCLUSIVE) {
        std::cout << "Input::validateTask(): invalid profit " << profit
                  << " is smaller than " << PROFIT_MIN_EXCLUSIVE << "!\n";
        return false;
    } else if (profit >= PROFIT_MAX_EXCLUSIVE) {
        std::cout << "Input::validateTask(): invalid profit " << profit
                  << " is greater than " << PROFIT_MAX_EXCLUSIVE << "!\n";
        return false;
    } else {
        return true;
    }
}

///
/// \brief Checks the validity of the number of tasks.
/// \param n: Number of tasks.
/// \param preset: 100, 150, or 200, as specified by project.
/// \return True if valid, false otherwise.
///
bool Input::validateTaskNumber(int n, int preset) const {
    switch (preset) {
    case 100:
        if (n < TASKS_100_MIN) {
            std::cout << "Input::validateTaskNumber(): invalid task number " << n
                      << " is smaller than " << TASKS_100_MIN << "!\n";
            return false;
        } else if (n > TASKS_100_MAX) {
            std::cout << "Input::validateTaskNumber(): invalid task number " << n
                      << " is greater than " << TASKS_100_MAX << "!\n";
            return false;
        } else {
            return true;
        }
        break;

    case 150:
        if (n < TASKS_150_MIN) {
            std::cout << "Input::validateTaskNumber(): invalid task number " << n
                      << " is smaller than " << TASKS_150_MIN << "!\n";
            return false;
        } else if (n > TASKS_150_MAX) {
            std::cout << "Input::validateTaskNumber(): invalid task number " << n
                      << " is greater than " << TASKS_150_MAX << "!\n";
            return false;
        } else {
            return true;
        }
        break;

    case 200:
        if (n < TASKS_200_MIN) {
            std::cout << "Input::validateTaskNumber(): invalid task number " << n
                      << " is smaller than " << TASKS_200_MIN << "!\n";
            return false;
        } else if (n > TASKS_200_MAX) {
            std::cout << "Input::validateTaskNumber(): invalid task number " << n
                      << " is greater than " << TASKS_200_MAX << "!\n";
            return false;
        } else {
            return true;
        }
        break;

    default:
        std::cout << "Input::validateTaskNumber(): invalid preset == " << preset << '\n';
        return false;
        break;
    }
}

///
/// \brief Sends a string representation of an Input object to the std::ostream.
/// \param o: output stream.
/// \param in: Input object to print.
/// \return R/W reference to the output stream.
///
std::ostream& operator <<(std::ostream &o, const Input& in) {
    int n = in.size();
    o << "-------------- BEGIN INPUT TASK LIST (1-INDEXED) --------------\n\n";
    for (int i = 0; i < n; ++i) {
        o << "index == " << i + 1
          << ": DDL == " << in.getDeadline(i)
          << ", duration == " << in.getDuration(i)
          << ", profit == " << in.getProfit(i)
          << '\n';
    }
    o << "\n--------------------- END INPUT TASK LIST ---------------------\n";
    return o;
}

