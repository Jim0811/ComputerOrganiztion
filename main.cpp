#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
ifstream cinp3("../inputs/test3.txt");
ofstream coup3("../outputs/test3.txt");
int oid, a, b, c, maxcount = 20;
void fcinp(int q, int w, int e, int r, int t, ifstream& cinp) {
    cinp.ignore(q);
    cinp >> a;
    cinp.ignore(w);
    cinp >> b;
    cinp.ignore(e);
    cinp >> c;
    cinp.ignore(r);
    oid = t;
}
void every_thing(ofstream& coup, ifstream& cinp) {
    int position = 0, prenum = 0, counter = 0, mem[32], reg[32], cycle = 4;
    vector<vector<string>> str2;
    vector<string> str = { "if ", "id ", "ex ", "me ", "wb " }, strs;
    vector<string> strmap = { "lw ", "sw ", "add", "sub", "beq" };
    vector<vector<int>> inst;
    string prestr = "";
    fill(mem, mem + 32, 1);
    fill(reg, reg + 32, 1);
    reg[0] = 0;

    for (string operation; cinp >> operation;) {
        if (operation == "lw") {
            fcinp(2, 2, 2, 1, 0, cinp);
        }
        else if (operation == "sw") {
            fcinp(2, 2, 2, 1, 1, cinp);
        }
        else if (operation == "add") {
            fcinp(2, 3, 3, 0, 2, cinp);
        }
        else if (operation == "sub") {
            fcinp(2, 3, 3, 0, 3, cinp);
        }
        else {
            fcinp(2, 3, 1, 0, 4, cinp);
        }
        inst.push_back({ oid, a, b, c });
    }
    
    for (int i = 0; i < maxcount; i++) {
        int size = str2[i].size();
        while (size < cycle) {
            str2[i].push_back("   ");
            size = str2[i].size();
        }
    }
    coup << "\n\n    1   2   3   4   5   6   7   8   9   10  11  12  13  14  "
        "15  16  17  18  19  20\n";
    for (int i = 0; i < counter; i++) {
        coup << strs[i] << " ";
        for (string s : str2[i]) {
            coup << s << " ";
        }
        coup << endl;
    }
    coup << "\n" << cycle << " cycles\n";
    coup << "\n$1  $2  $3  $4  $5  $6  $7  $8  $9  $10 $11 $12 $13 $14 $15 $16 "
        "$17 $18 $19 $20 $21 $22 $23 $24 $25 $26 $27 $28 $29 $30 $31 $32\n";
    for (int i : mem)
        coup << i << "   ";
    coup << "\nw1  w2  w3  w4  w5  w6  w7  w8  w9  w10 w11 w12 w13 w14 w15 w16 "
        "w17 w18 w19 w20 w21 w22 w23 w24 w25 w26 w27 w28 w29 w30 w31 w32\n";
    for (int i : reg)
        coup << i << "   ";
}
int main() {
    every_thing(coup3, cinp3);
    return 0;
}
