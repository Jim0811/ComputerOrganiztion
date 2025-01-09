#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
ifstream cinp3("../inputs/test3.txt");
ifstream cinp4("../inputs/test4.txt");
ifstream cinp5("../inputs/test5.txt");
ifstream cinp6("../inputs/test6.txt");
ofstream coup3("../outputs/test3.txt");
ofstream coup4("../outputs/test4.txt");
ofstream coup5("../outputs/test5.txt");
ofstream coup6("../outputs/test6.txt");
int oid, a, b, c, maxcount = 20;
void fcinp(int q, int w, int e, int r, int t, ifstream &cinp) {
    cinp.ignore(q);
    cinp >> a;
    cinp.ignore(w);
    cinp >> b;
    cinp.ignore(e);
    cinp >> c;
    cinp.ignore(r);
    oid = t;
}
void every_thing(ofstream &coup, ifstream &cinp) {
    int position = 0, prenum = 0, counter = 0, mem[32], reg[32], cycle = 4, ppnum = 0;
    vector<vector<string>> str2;
    vector<string> str = {"if ", "id ", "ex ", "me ", "wb "}, strs;
    vector<string> strmap = {"lw ", "sw ", "add", "sub", "beq"};
    vector<vector<int>> inst;
    string prestr = "", ppstr = "";
    fill(mem, mem + 32, 1);
    fill(reg, reg + 32, 1);
    reg[0] = 0;
    int count20 = maxcount;
    while (count20--) {
        str2.push_back(str);
        str.insert(str.begin(), "   ");
    }
    for (string operation; cinp >> operation;) {
        if (operation == "lw") {
            fcinp(2, 2, 2, 1, 0, cinp);
        } else if (operation == "sw") {
            fcinp(2, 2, 2, 1, 1, cinp);
        } else if (operation == "add") {
            fcinp(2, 3, 3, 0, 2, cinp);
        } else if (operation == "sub") {
            fcinp(2, 3, 3, 0, 3, cinp);
        } else {
            fcinp(2, 3, 1, 0, 4, cinp);
        }
        inst.push_back({oid, a, b, c});
    }
    int instruction = inst.size();
    for (int i = 0; i < instruction; ++i) {
        if (inst[i][0] == 0) {
            strs.push_back("lw ");
            reg[inst[i][1]] = mem[reg[inst[i][3]] + inst[i][2] / 4];
            ppstr = prestr;
            ppnum = prenum;
            prestr = "lw";
            prenum = inst[i][1];
        } else if (inst[i][0] == 1) {
            strs.push_back("sw ");
            mem[reg[inst[i][3]] + inst[i][2] / 4] = reg[inst[i][1]];
            ppstr = prestr;
            ppnum = prenum;
            prestr = "";
        } else if (inst[i][0] == 2) {
            strs.push_back("add");
            if (prestr == "lw" &&
                (prenum == inst[i][2] || prenum == inst[i][3])) {
                cycle++;
                for (int k = counter; k < maxcount; k++) {
                    str2[k].insert(str2[k].begin() + position + 2,
                                   str2[k][position + 1]);
                }
                position++;
            }
            reg[inst[i][1]] = reg[inst[i][2]] + reg[inst[i][3]];
            ppstr = prestr;
            ppnum = prenum;
            prestr = "r";
            prenum = inst[i][1];
        } else if (inst[i][0] == 3) {
            strs.push_back("sub");
            if (prestr == "lw" &&
                (prenum == inst[i][2] || prenum == inst[i][3])) {
                cycle++;
                for (int k = counter; k < maxcount; k++) {
                    str2[k].insert(str2[k].begin() + position + 2,
                                   str2[k][position + 1]);
                }
                position++;
            }
            reg[inst[i][1]] = reg[inst[i][2]] - reg[inst[i][3]];
            ppstr = prestr;
            ppnum = prenum;
            prestr = "r";
            prenum = inst[i][1];
        } else {
            strs.push_back("beq");
            if (prenum == inst[i][1] || prenum == inst[i][2]) {
                if (prestr == "lw") {
                    cycle += 2;
                    for (int k = counter; k < maxcount; k++) {
                        str2[k].insert(str2[k].begin() + position + 2,
                                       str2[k][position + 1]);
                    }
                    position++;
                    for (int k = counter; k < maxcount; k++) {
                        str2[k].insert(str2[k].begin() + position + 2,
                                       str2[k][position + 1]);
                    }
                    position++;
                } else if (prestr == "r") {
                    for (int k = counter; k < maxcount; k++) {
                        str2[k].insert(str2[k].begin() + position + 2,
                                       str2[k][position + 1]);
                    }
                    position++;
                    cycle++;
                }
            }else if((ppnum == inst[i][1] || ppnum == inst[i][2]) && ppstr == "lw"){
                for (int k = counter; k < maxcount; k++) {
                        str2[k].insert(str2[k].begin() + position + 2,
                                       str2[k][position + 1]);
                    }
                    position++;
                    cycle++;
            }
            if (reg[inst[i][1]] == reg[inst[i][2]]) {
                cycle++;
                position++;
                counter++;
                str2[counter].pop_back();
                str2[counter].pop_back();
                str2[counter].pop_back();
                str2[counter].pop_back();
                strs.push_back(strmap[inst[i + 1][0]]);
                i += inst[i][3];
            }
            ppstr = prestr;
            ppnum = prenum;
            prestr = "";
        }
        cycle++;
        counter++;
        position++;
    }
    for (int i = 0; i < maxcount; i++) {
        int size = str2[i].size();
        while (size < cycle) {
            str2[i].push_back("   ");
            size = str2[i].size();
        }
    }
    for (int i = 0; i < cycle; i++) {
        coup << "cycle " << i + 1;
        for (int j = 0; j < counter; j++) {
            if (str2[j][i] != "   ") {
                coup << "\n    " << strs[j] << ": " << str2[j][i];
                if (strs[j] == "lw ") {
                    if (str2[j][i] == "ex ") {
                        coup << " 01 010 11";
                    } else if (str2[j][i] == "me ") {
                        coup << " 010 11";
                    } else if (str2[j][i] == "wb ") {
                        coup << " 11";
                    }
                } else if (strs[j] == "sw ") {
                    if (str2[j][i] == "ex ") {
                        coup << " x1 001 0x";
                    } else if (str2[j][i] == "me ") {
                        coup << " 001 0x";
                    } else if (str2[j][i] == "wb ") {
                        coup << " 0x";
                    }
                } else if (strs[j] == "add" || strs[j] == "sub") {
                    if (str2[j][i] == "ex ") {
                        coup << " 10 000 10";
                    } else if (str2[j][i] == "me ") {
                        coup << " 000 10";
                    } else if (str2[j][i] == "wb ") {
                        coup << " 10";
                    }
                } else {
                    if (str2[j][i] == "ex ") {
                        coup << " x0 100 0x";
                    } else if (str2[j][i] == "me ") {
                        coup << " 100 0x";
                    } else if (str2[j][i] == "wb ") {
                        coup << " 0x";
                    }
                }
            }
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
        coup << "\n\n    1   2   3   4   5   6   7   8   9   10  11  12  13  14  "
            "15  16  17  18  19  20\n";
    for (int i = 0; i < counter; i++) {
        coup << strs[i] << " ";
        for (string s : str2[i]) {
            coup << s << " ";
        }
        coup << endl;
    }
}
int main() {
    every_thing(coup3, cinp3);
    every_thing(coup4, cinp4);
    every_thing(coup5, cinp5);
    every_thing(coup6, cinp6);
}
/*

*/