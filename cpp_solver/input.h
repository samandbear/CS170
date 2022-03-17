#ifndef INPUT_H
#define INPUT_H
#include <string>
#include <vector>
#include <random>

// Ranges specified by project. See "project_spec.pdf".
const int TASKS_100_MIN = 76;
const int TASKS_100_MAX = 100;
const int TASKS_150_MIN = 101;
const int TASKS_150_MAX = 150;
const int TASKS_200_MIN = 151;
const int TASKS_200_MAX = 200;
const int MAX_TIME = 1440;
const int DEADLINE_MIN = 1;
const int DEADLINE_MAX = 1440;
const int DURATION_MIN = 1;
const int DURATION_MAX = 60;
const double PROFIT_MIN_EXCLUSIVE = 0.0;
const double PROFIT_MAX_EXCLUSIVE = 100.0;

///
/// \brief An Input object is a problem instance specified by number of tasks together with deadline,
///  duration, and profit of each task. An Input can be read from a file on disk, or randomly generated
///  by specifying the number of tasks.
///
class Input
{
private:
    int n;
    std::vector<int> deadlines;
    std::vector<int> durations;
    std::vector<double> profits;
    std::vector<bool> taken;
    bool valid;
    bool fail = false;

    std::mt19937_64 gen;
    std::uniform_real_distribution<> uniformRealDist = std::uniform_real_distribution<double>(0.0, 1.0);

public:
    Input();
    Input(int numTasks, int seed = 0);
    Input(const std::string &fileName, int preset = 0);

    bool failed() const;
    bool isValid() const;

    int size() const;
    int getDeadline(int i) const;
    int getDuration(int i) const;
    double getProfit(int i) const;
    bool isTaken(int i) const;
    void setTaken(int i, bool val = true);

    const std::vector<int>& getDeadlines() const;
    const std::vector<int>& getDurations() const;
    const std::vector<double>& getProfits() const;

    void writeFile(const std::string& fileName) const;
    friend std::ostream& operator <<(std::ostream& o, const Input& in);

private:
    bool validateTask(int deadline, int duration, double profit) const;
    bool validateTaskNumber(int n, int preset) const;
};

#endif // INPUT_H
