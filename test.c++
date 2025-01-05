#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <iomanip>
using namespace std;

// Define the pipeline stages
enum PipelineStage { IF, ID, EX, MEM, WB };

// Define the Instruction structure
struct Instruction {
    string opcode;
    int rs, rt, rd; // Registers
    int immediate;  // Immediate value for I-type instructions
    int address;    // Address for branching or memory access
};

class MIPS_Simulator {
private:
    unordered_map<string, int> register_file;
    vector<int> memory;
    queue<Instruction> pipeline[5]; // Simulate the pipeline with stages
    vector<string> clock_output;   // Store clock cycle information

    unordered_map<int, int> forwarding_buffers;
    int clock_cycle;
    bool branch_taken;

public:
    MIPS_Simulator(int memory_size) : memory(memory_size, 0), clock_cycle(0), branch_taken(false) {
        // Initialize registers: $0 to 0, others to 1
        for (int i = 0; i < 32; ++i) {
            register_file["$" + to_string(i)] = (i == 0) ? 0 : 1;
        }
    }

    void load_program(const vector<Instruction>& program) {
        for (const auto& inst : program) {
            pipeline[IF].push(inst);
        }
    }

    void execute_cycle() {
        clock_output.push_back("Cycle " + to_string(++clock_cycle) + "\n");

        // Write Back (WB)
        if (!pipeline[WB].empty()) {
            Instruction inst = pipeline[WB].front();
            pipeline[WB].pop();
            clock_output.back() += " " + inst.opcode + ": WB " + (inst.rd == 0 ? "00" : "11") + "\n";
        }

        // Memory Access (MEM)
        if (!pipeline[MEM].empty()) {
            Instruction inst = pipeline[MEM].front();
            pipeline[MEM].pop();

            bool MemRead = (inst.opcode == "lw");
            bool MemWrite = (inst.opcode == "sw");
            bool RegWrite = (inst.opcode == "lw");

            if (MemRead) {
                forwarding_buffers[inst.rd] = memory[inst.address / 4];
            } else if (MemWrite) {
                memory[inst.address / 4] = register_file["$" + to_string(inst.rt)];
            }

            pipeline[WB].push(inst);
            clock_output.back() += " " + inst.opcode + ": MEM " + (inst.rs == 0 ? "00" : "10") + "\n";
        }

        // Execute (EX)
        if (!pipeline[EX].empty()) {
            Instruction inst = pipeline[EX].front();
            pipeline[EX].pop();

            if (inst.opcode == "add") {
                forwarding_buffers[inst.rd] = register_file["$" + to_string(inst.rs)] + register_file["$" + to_string(inst.rt)];
                clock_output.back() += " " + inst.opcode + ": EX " + "01 " + to_string(inst.rs) + " " + to_string(inst.rt) + "\n";
            } else if (inst.opcode == "sub") {
                forwarding_buffers[inst.rd] = register_file["$" + to_string(inst.rs)] - register_file["$" + to_string(inst.rt)];
            } else if (inst.opcode == "beq") {
                // If $rs == $rt, we take the branch
                if (register_file["$" + to_string(inst.rs)] == register_file["$" + to_string(inst.rt)]) {
                    clock_output.back() += " beq: EX X0 100 0X\n";
                    branch_taken = true;
                    clock_cycle += inst.immediate;  // Jump by the immediate value
                    return; // Skip remaining stages for this cycle
                }
            }

            pipeline[MEM].push(inst);
        }

        // Instruction Decode (ID)
        if (!pipeline[ID].empty()) {
            Instruction inst = pipeline[ID].front();
            pipeline[ID].pop();

            clock_output.back() += " " + inst.opcode + ": ID\n";
            pipeline[EX].push(inst);
        }

        // Instruction Fetch (IF)
        if (!pipeline[IF].empty()) {
            Instruction inst = pipeline[IF].front();
            pipeline[IF].pop();
            clock_output.back() += " " + inst.opcode + ": IF\n";
            pipeline[ID].push(inst);
        }
    }

    void run() {
        while (!pipeline[IF].empty() || !pipeline[ID].empty() || !pipeline[EX].empty() || !pipeline[MEM].empty() || !pipeline[WB].empty()) {
            execute_cycle();
            if (branch_taken) {
                // Clear pipeline after a branch
                while (!pipeline[IF].empty()) pipeline[IF].pop();
                while (!pipeline[ID].empty()) pipeline[ID].pop();
                while (!pipeline[EX].empty()) pipeline[EX].pop();
                while (!pipeline[MEM].empty()) pipeline[MEM].pop();
                while (!pipeline[WB].empty()) pipeline[WB].pop();
                branch_taken = false;
            }
        }
    }

    void print_clock_cycles() {
        for (const auto& cycle : clock_output) {
            cout << cycle;
        }
    }

    void print_registers() {
        cout << "Final Register Values:\n";
        for (int i = 0; i < 32; ++i) {
            cout << "$" << i << ": " << register_file["$" + to_string(i)] << " ";
        }
        cout << endl;
    }

    void print_memory() {
        cout << "Final Memory Values:\n";
        for (size_t i = 0; i < memory.size(); ++i) {
            if (memory[i] != 0) {
                cout << "Memory[" << i * 4 << "]: " << memory[i] << endl;
            }
        }
    }

    int get_total_cycles() const {
        return clock_cycle;
    }
};

int main() {
    // Example program
    vector<Instruction> program = {
        {"lw", 0, 2, 0, 8, 0},  // lw $2, 8($0)
        {"lw", 0, 3, 0, 16, 0}, // lw $3, 16($0)
        {"beq", 2, 3, 0, 1, 0}, // beq $2, $3, 1
        {"add", 2, 3, 4, 0, 0}, // add $4, $2, $3
        {"sw", 4, 0, 0, 24, 0}  // sw $4, 24($0)
    };

    MIPS_Simulator simulator(1024); // Create a simulator with 1024 words of memory
    simulator.load_program(program);
    simulator.run();
    simulator.print_clock_cycles();
    simulator.print_registers();
    simulator.print_memory();

    return 0;
}
