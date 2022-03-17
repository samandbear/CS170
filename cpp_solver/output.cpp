#include "output.h"
#include <iostream>
#include <fstream>
#include <cmath>

Output::Output() {}

///
/// \brief Initializes a Output using a vector of task indices.
/// \param schedule: A std::vector containing the sequence of tasks.
///
Output::Output(const std::vector<int> &schedule) {
    taskSchedule = schedule;
}

///
/// \brief Reads an Output from a file.
/// \param fileName: Directory to the Output file.
///
Output::Output(const std::string &fileName) {
    std::fstream file(fileName, std::fstream::in);
    int task;
    while (!file.eof()) {
        file >> task;
        taskSchedule.push_back(task - 1); // -1 to convert to 0-indexed
    }
    file.close();
}

///
/// \brief Swaps two tasks in the task sequence.
/// \param index1: index of first task.
/// \param index2: index of second task/
/// \return True if the indices are valid, false otherwise.
///
bool Output::swapTasks(size_t index1, size_t index2) {
    if (index1 >= taskSchedule.size() || index2 >= taskSchedule.size()) {
        return false;
    }
    int temp = taskSchedule[index1];
    taskSchedule[index1] = taskSchedule[index2];
    taskSchedule[index2] = temp;
    return true;
}

///
/// \brief Trims the Output to fit the gloabl deadline specified by project.
///        An Input must be specified for durations of each task.
/// \param input: Problem is specified by this Input.
/// \return True if the Output is valid for the Input, false otherwise.
///
bool Output::trim(const Input &input) {
    int time = 0;
    for (size_t i = 0; i < taskSchedule.size(); ++i) {
        if (taskSchedule[i] >= input.size()) {
            return false;
        }
        time += input.getDuration(taskSchedule[i]);
        if (time > 1440) {  // tasks from index i to the end are invalid
            taskSchedule.resize(i);
            return true;
        }
    }
    return true;
}

///
/// \brief Checks if the current Output is valid for the given Input,
///        and does not exceed global deadline.
/// \param input: Problem is specified by this Input.
/// \return True if valid, false otherwise.
///
bool Output::isValidFor(const Input& input) const {
    int time = 0;
    for (int i : taskSchedule) {
        if (i >= input.size()) {
            return false;
        }
        time += input.getDuration(i);
        if (time > 1440) {
            return false;
        }
    }
    return true;
}

///
/// \brief Returns the profit of the current task schedule for
///        the given Input.
/// \param input: Problem is specified by this Input.
/// \return Profit of current task schedule.
///
double Output::evaluate(const Input &input) const {
    double res = 0;
    int time = 0;
    int minutesLate;
    for (int i : taskSchedule) {
        time += input.getDuration(i);
        if (time > 1440) {
            return res;
        }
        minutesLate = time - input.getDeadline(i);
        if (minutesLate > 0) {
            res += input.getProfit(i) * exp(-0.017 * minutesLate);
        } else {
            res += input.getProfit(i);
        }
    }
    return res;
}

///
/// \brief Writes the Output to a file.
/// \param fileName: Directory to the Output file. Overwrites
///        if file already exists.
///
void Output::writeFile(const std::string &fileName) {
    std::fstream fout;
    fout.open(fileName, std::fstream::out);
    for (int i : taskSchedule) {
        fout << i + 1 << '\n';
    }
    fout.close();
}

///
/// \brief Sends the string representation of an Output sequence
///        to the output stream.
/// \param o: output stream.
/// \param output: Output to print.
/// \return R/W reference to the output stream.
///
std::ostream& operator <<(std::ostream& o, const Output& output) {
    o << "Task Sequence (1-indexed): ";
    for (int i : output.taskSchedule) {
        o << i + 1 << ' ';
    }
    o << '\n';
    return o;
}
