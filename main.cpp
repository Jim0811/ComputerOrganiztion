#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

struct Instruction
{
    string ins;
    int regRs, regRt, regRd;
    int immediate;
    int cycleIssued;
    int stage[5]; // 記錄每個階段的週期，-1 表示尚未進入該階段
};
int main()
{
    int cycleCount = 1;
    string state[5] = {"IF", "ID", "EX", "MEM", "WB"};
    vector<Instruction> instructions;
    queue<Instruction> pipelineStages[5];
    unordered_map<int, int> registers;
    unordered_map<int, int> memory;

    for (int i = 0; i < 10; ++i)
    {
        registers[i] = 0;
    }

    instructions.push_back({"lw", 0, 0, 2, 8, 0});   // lw $2, 8($0)
    instructions.push_back({"lw", 0, 0, 3, 12, -1}); // lw $3, 12($0)
    instructions.push_back({"add", 2, 3, 4, 0, -2}); // add $4, $2, $3
    instructions.push_back({"sw", 0, 0, 6, 20, -3}); // sw $6, 20($0)

    memory[8] = 100; // Initialize memory for lw instructions
    memory[12] = 200;

    while (!instructions.empty() || !pipelineStages[4].empty())
    {
        cout << "Cycle " << cycleCount << ":" << endl;

        // WB
        if (!pipelineStages[4].empty())
        {
            Instruction instr = pipelineStages[4].front();
            pipelineStages[4].pop();
            if (instr.ins == "add" || instr.ins == "lw")
            {
                registers[instr.regRd] = instr.immediate;
            }
            cout << "Instruction: " << instr.ins << ", Stage: WB" << endl;
        }

        // MEM
        if (!pipelineStages[3].empty())
        {
            Instruction instr = pipelineStages[3].front();
            pipelineStages[3].pop();
            if (instr.ins == "lw")
            {
                instr.immediate = memory[registers[instr.regRs] + instr.immediate];
            }
            else if (instr.ins == "sw")
            {
                memory[registers[instr.regRs] + instr.immediate] = registers[instr.regRt];
            }
            pipelineStages[4].push(instr);
            cout << "Instruction: " << instr.ins << ", Stage: MEM" << endl;
        }

        // EX
        if (!pipelineStages[2].empty())
        {
            Instruction instr = pipelineStages[2].front();
            pipelineStages[2].pop();

            int rsVal = registers[instr.regRs];
            int rtVal = registers[instr.regRt];

            // Forwarding Logic
            if (!pipelineStages[3].empty() && instr.regRs == pipelineStages[3].back().regRd)
            {
                if (pipelineStages[3].back().ins == "lw")
                    rsVal = memory[registers[pipelineStages[3].back().regRs] + pipelineStages[3].back().immediate];
                else
                    rsVal = pipelineStages[3].back().immediate;
            }
            if (!pipelineStages[3].empty() && instr.regRt == pipelineStages[3].back().regRd)
            {
                if (pipelineStages[3].back().ins == "lw")
                    rtVal = memory[registers[pipelineStages[3].back().regRs] + pipelineStages[3].back().immediate];
                else
                    rtVal = pipelineStages[3].back().immediate;
            }
            if (!pipelineStages[2].empty() && instr.regRs == pipelineStages[2].back().regRd)
            {
                rsVal = pipelineStages[2].back().immediate;
            }
            if (!pipelineStages[2].empty() && instr.regRt == pipelineStages[2].back().regRd)
            {
                rtVal = pipelineStages[2].back().immediate;
            }

            if (instr.ins == "add")
            {
                instr.immediate = rsVal + rtVal;
            }
            else if (instr.ins == "sw")
            {
                // No calculation needed in EX for sw
            }
            pipelineStages[3].push(instr);
            cout << "Instruction: " << instr.ins << ", Stage: EX" << endl;
        }

        // ID
        if (!pipelineStages[1].empty())
        {
            Instruction instr = pipelineStages[1].front();
            pipelineStages[1].pop();

            bool stalled = false;

            // Hazard Detection
            if (instr.ins == "add" || instr.ins == "sw")
            {
                if ((!pipelineStages[2].empty() && (instr.regRs == pipelineStages[2].back().regRd || instr.regRt == pipelineStages[2].back().regRd)) ||
                    (!pipelineStages[3].empty() && (instr.regRs == pipelineStages[3].back().regRd || instr.regRt == pipelineStages[3].back().regRd)))
                {
                    stalled = true;
                }
            }
            else if (instr.ins == "lw")
            {
                if (!pipelineStages[2].empty() && instr.regRs == pipelineStages[2].back().regRd)
                {
                    stalled = true;
                }
            }

            if (stalled)
            {
                pipelineStages[1].push(instr);
                cout << "Instruction: " << instr.ins << ", Stage: STALL" << endl;
            }
            else
            {
                pipelineStages[2].push(instr);
                cout << "Instruction: " << instr.ins << ", Stage: ID" << endl;
            }
        }

        // IF
        if (!instructions.empty())
        {
            Instruction instr = instructions.front();

            // Check if IF needs to stall
            bool stallIF = false;
            if (!pipelineStages[1].empty() && pipelineStages[1].back().ins == "add" &&
                (pipelineStages[1].back().regRd == instr.regRs || pipelineStages[1].back().regRd == instr.regRt))
            {
                stallIF = true;
            }

            if (stallIF)
            {
                cout << "Instruction: " << instr.ins << ", Stage: STALL (IF)" << endl;
            }
            else
            {
                instructions.erase(instructions.begin());
                pipelineStages[1].push(instr);
                cout << "Instruction: " << instr.ins << ", Stage: IF" << endl;
            }
        }

        cycleCount++;
    }

    cout << "\nFinal Register Values:" << endl;
    for (int i = 0; i < 10; ++i)
    {
        cout << "$" << i << ": " << registers[i] << " ";
    }
    cout << endl;
    cout << "\nFinal Memory Values:" << endl;
    for (auto it = memory.begin(); it != memory.end(); ++it)
    {
        cout << "mem[" << it->first << "]: " << it->second << " ";
    }
    cout << endl;

    return 0;
}
