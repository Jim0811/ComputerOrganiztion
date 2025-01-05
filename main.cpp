#include <iostream>
#include <vector>
using namespace std;

struct instruction
{
    string ins;
    int stageNum;
};

int main()
{
    string state[5] = {"IF", "ID", "EX", "MEM", "WB"};
    vector<instruction> instructions;
    instructions.push_back({"lw", 0});
    instructions.push_back({"add", -1});
    instructions.push_back({"sub", -2});
    instructions.push_back({"beq", -3});
    instructions.push_back({"sw", -4});

    while (!instructions.empty())
    {
        for (auto it = instructions.begin(); it != instructions.end();)
        {
            if (it->stageNum >= 0 && it->stageNum < 5)
            {
                cout << "Instruction: " << it->ins << ", Stage: " << state[it->stageNum] << endl;
                it->stageNum++;
                ++it;
            }
            else if (it->stageNum >= 5)
            {
                it = instructions.erase(it);
            }
        }
    }
    return 0;
}
