#ifndef OUTPUT_H
#define OUTPUT_H
#include <iostream>
#include <vector>
#include "input.h"

///
/// \brief An output is a solution to the problem specified by a sequence
///        of task indices. The project spec does not allow tasks to be
///        assigned after the global deadline (1440 by default). However,
///        tasks are allowed to exist in this class even if they exceeds
///        the above time limit since the Output does not know anything
///        about Input. When evaluating the profit of an Output, an Input
///        is specified and all tasks assigned after the global deadline
///        receives zero profit. One can also trim the Output to fit the
///        the global deadline as a final step before writing the Output
///        to a file.
///
class Output
{
private:
    std::vector<int> taskSchedule;

public:
    Output();
    Output(const std::vector<int>& schedule);
    Output(const std::string& fileName);

    bool swapTasks(size_t index1, size_t index2);
    bool trim(const Input& input);

    bool isValidFor(const Input& input) const;
    double evaluate(const Input& input) const;

    void writeFile(const std::string& fileName);
    friend std::ostream& operator <<(std::ostream& o, const Output& output);
};

#endif // OUTPUT_H
