#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>

using namespace std;

/*
  Assembly Project 2
  Yaseen Ahmed
  Karim Sherif
  Farah Kabesh
  Submitted on: April 20, 2022
*/

map <string, int> registers; // 32 registers for risc v
map<string, int> labels; // carries label and which address its at
map<int, string> memory; // memory contains instruction
map<int, int> variables; //to store any variables with their existing values in the memory
int base_pc = 0; // program counter
string working_file_path = ""; // program file that will run
bool halt = 0;



void init_registers();
void run_program();
bool validate_file(string); //to validate file path
void assemble_code(string); //takes the file path

void assemble_branching(string);

//load and store operations
void assemble_LS(string);

//jal operation
void assemble_J(string);

//lui and auipc
void assemble_U(string);

//arithmetic and logic operation
void assemble_AL(string);
string int_to_hex(int);
string int_to_bin(int);
void read_file(string);
void get_main_info();




int main() {

    init_registers();
    run_program();
    //check for label, if no label then get the instruction, otherwise store the current label in a variable


    cout << "The final results are: " << endl;
    get_main_info();
    return 0;
}

//initialization of registers
void init_registers() {

    string x = "x", num = "", reg_name = "";

    for (int i = 1; i <= 32; i++) {
        num = to_string(i);
        reg_name = x + num;
        registers[reg_name] = 0;
    }

    //    for (pair<const string,int>& x: registers) {
    //        cout << x.first << " => " << x.second << endl;
}

//reads the program from the file

    void run_program() {

        string file_path;

        cout << "Please enter the path of your file: ";
        cin >> file_path;

        while (!validate_file(file_path)) {
            cout << "Invalid file path, try again: ";
            cin >> file_path;
        }

        read_file(file_path);

        char input;
        cout << "Enter 'y' if you have any variables to add, else enter 'n': " << endl;
        cin >> input;


        while (input != 'y' && input != 'n') {
            cout << "invalid input: There any variables that you want to add to member? (y/n) " << endl;
            cin >> input;
        }
        if (input == 'y') {

            int amt;
            cout << "How many variables would you like to enter: ";
            cin >> amt;
            for (int i = 0; i < amt; i++) {
                int address, value;
                cout << "Enter the address of this variable: " << endl;
                cin >> address;
                while (!address || address == 0) {
                    cout << "Invalid input, try again: ";
                    cin >> address;
                }
                cout << "Enter the value inside this variable:  " << endl;
                cin >> value;

                variables[address] = value;

            }

        }

        do {
            get_main_info();
            assemble_code(memory[base_pc]);
        } while (halt == false);


        // while (halt == false) {
        //     get_main_info();
        //     assemble_code(memory[base_pc]);
        // }

    }



//process the instruction and send it right operation
void assemble_code(string inst)
{
    vector<string> branching = { "beq", "bge", "blt", "bltu", "bne", "bgeu"};
    vector<string> LS = { "lb", "lbu","lh", "lhu", "lw", "sb", "sh", "sw" };
    vector<string> J = { "jal" , "jalr"}; //unconditional branching
    vector<string> U = { "lui", "auipc" };
    vector<string> AL = { "add", "sub", "sll", "slt", "sltu", "xor", "srl", "sra", "or", "and", "addi", "slti", "sltiu", "xori", "ori", "andi", "slli", "srli", "srai" };
    vector<string> halting = { "ebreak", "ecall", "fence" };

    int idx = inst.find(" ");
    string sub_str = inst.substr(0, idx);
    cout << endl;
   cout << sub_str << endl;
   cout << endl;

    //checks if halting operations is found in the instruction
    for (auto i : halting)
    {
        int found = inst.find(i);
        if (found != string::npos)
        {
            halt = 1;
            return;
        }
    }

    cout << "instruction is " << inst << endl;

    if (find(branching.begin(), branching.end(), sub_str) != branching.end())
    {
       // cout << "found" << endl;
        assemble_branching(inst);
    }
    else if (find(LS.begin(), LS.end(), sub_str) != LS.end())
        assemble_LS(inst);
    else if (find(J.begin(), J.end(), sub_str) != J.end())
        assemble_J(inst);
    else if (find(U.begin(), U.end(), sub_str) != U.end())
        assemble_U(inst);
    else if (find(AL.begin(), AL.end(), sub_str) != AL.end())
        assemble_AL(inst);

}

void assemble_AL(string inst)
{
    //add x5, x6, x7
    //add rd, rs1, rs2
    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1); //erase inorder to start from x5

    idx = inst.find(",");
    string rd = inst.substr(0, idx);
    inst.erase(0, idx + 2);

    idx = inst.find(",");
    string rs1 = inst.substr(0, idx);
    inst.erase(0, idx + 2);


    string rs2 = inst;

    cout << rd << " " << rs1 << " " << rs2 << endl;

    //handling const x0
    if(rd == "x0")
    {
        base_pc += 4;
        return;
    }
    if (operation == "add")
    {
        registers[rd] = registers[rs1] + registers[rs2];
        base_pc += 4;
    }
    else if (operation == "sub")
    {
        registers[rd] = registers[rs1] - registers[rs2];
        base_pc += 4;
    }
    else if (operation == "sll")
    {
        registers[rd] = registers[rs1] << registers[rs2];
        base_pc + 4;
    }
    else if (operation == "slt")
    {
        if (registers[rs1] < registers[rs2])
        {
            registers[rd] = 1;
        }
        else
            registers[rd] = 0;

        base_pc += 4;
    }
    else if (operation == "sltu")
    {
        if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
        {
            registers[rd] = 1;
        }
        else
            registers[rd] = 0;

        base_pc += 4;
    }
    else if (operation == "xor")
    {
        registers[rd] = registers[rs1] ^ registers[rs2]; // ^ is XOR operator

        base_pc += 4;
    }
    else if (operation == "srl")
    {
        registers[rd] = registers[rs1] >> registers[rs2];

        base_pc += 4;
    }
    else if (operation == "sra")
    {
        registers[rd] = (registers[rs1] / (pow(2, registers[rs2]))) - 1;
        base_pc += 4;

    }
    else if (operation == "or")
    {
        registers[rd] = registers[rs1] | registers[rs2];
        base_pc += 4;

    }
    else if (operation == "and")
    {
        registers[rd] = registers[rs1] & registers[rs2];
        base_pc += 4;

    }
    else if (operation == "addi")
    {
        registers[rd] = registers[rs1] + stoi(rs2);
        base_pc += 4;
    }
    else if (operation == "slti")
    {
        if (registers[rs1] < stoi(rs2))
        {
            registers[rd] = 1;
        }
        else
            registers[rd] = 0;

        base_pc += 4;
    }
    else if (operation == "sltiu")
    {
        if ((unsigned int)registers[rs1] < stoi(rs2))
        {
            registers[rd] = 1;
        }
        else
            registers[rd] = 0;

        base_pc += 4;
    }
    else if (operation == "xori")
    {
        registers[rd] = registers[rs1] ^ stoi(rs2); // ^ is XOR operator

        base_pc += 4;
    }
    else if (operation == "ori")
    {
        registers[rd] = registers[rs1] | registers[rs2]; // ^ is XOR operator
        base_pc += 4;
    }
    else if (operation == "andi")
    {
       registers[rd] = registers[rs1] & stoi(rs2);
       base_pc += 4;

    }
    else if (operation == "slli")
    {
      registers[rd] = registers[rs1] << stoi(rs2);
      base_pc + 4;
    }
    else if (operation == "srli")
    {
      registers[rd] = registers[rs1] >> stoi(rs2);
      base_pc += 4;
    }
    else if (operation == "srai")
    {
    registers[rd] = (registers[rs1] / stoi(rs2)) - 1;
       base_pc += 4;
    }



}

void assemble_J(string inst) {

    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1);

    if (operation == "jal") {

        int curr_idx = inst.find(",");
        string destination = inst.substr(0, curr_idx);
        inst.erase(0, curr_idx + 2);

       /* if (destination == "x0") {
            base_pc += 4;
            return;
            //base_pc = labels[inst];
        }

        else {
            registers[destination] = base_pc + 4;
            base_pc = labels[inst];

            cout << "new pc: " << base_pc << endl;
            //base_pc = base_pc + curr_inst; //not sure which one
            //base_pc = registers[base_register + stoi(offset)];//go to base register + offset
        }*/

        if (destination != "x0") { registers[destination] = base_pc + 4; }
        base_pc = labels[inst];
    }
    else if (operation == "jalr") {
        string current_inst = inst; //x1, 100(x5);
        int curr_idx = inst.find(",");
        string destination = inst.substr(0, curr_idx); //x1
        inst.erase(0, idx + 2); //to delete the comma and the space after the comma
        //cout << destination << endl;

        string offset = inst.substr(0, inst.find("(")); //100
        inst.erase(0, inst.find("(") + 1);
        string base_register = inst.substr(0, inst.find(")")); //x5

        //jal x0, label
        if (destination == "x0") {
            base_pc += 4;
            return;
            //base_pc = labels[inst];
        }
        else {
            registers[destination] = base_pc + 4; //destination register = pc+4
            base_pc = registers[base_register] + stoi(offset);//go to base register + offset
        }


    }
}

void assemble_branching(string inst)
{
    //beq x5, x6, label
    //beq rs1, rs2, label

    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1);

    cout << operation << endl;

    idx = inst.find(",");
    string rs1 = inst.substr(0, idx);
    inst.erase(0, idx + 2);

    cout << rs1 << endl;

    idx = inst.find(",");
    string rs2 = inst.substr(0, idx);
    inst.erase(0, idx + 2);

    cout << rs2 << endl;

    string label = inst;

    cout << label << endl;


    if (operation == "beq")
    {
        if (registers[rs1] == registers[rs2])
            base_pc = labels[label];
        else
            base_pc += 4;
    }
    else if (operation == "bge")
    {
        if (registers[rs1] > registers[rs2])
        {
            cout << "yes: " << labels[label] << endl;
            base_pc = labels[label];
            cout << base_pc;

        }
        else
        {
            cout << "no: " << labels[label] << endl;
            base_pc += 4;
        }
    }
    else if (operation == "bne")
    {
        if (registers[rs1] != registers[rs2])
        {

            base_pc = labels[label];
        }
        else
            base_pc += 4;
    }
    else if (operation == "blt")
    {
        if (registers[rs1] < registers[rs2])
        {
            cout << "yes: " << labels[label] << endl;
            base_pc = labels[label];

        }

        else
        {
            cout << "no: " << labels[label] << endl;
            base_pc += 4;
        }
    }
    else if (operation == "bltu")
    {
        if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
            base_pc = labels[label];
        else
            base_pc += 4;
    }
    else if (operation == "bgeu")
    {
        if ((unsigned int)registers[rs1] > (unsigned int)registers[rs2])
            base_pc = labels[label];
        else
            base_pc += 4;
    }


}

void assemble_LS(string inst) {

    //sb, sw, sh

    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1);
    //cout << operation << endl;


    idx = inst.find(",");
    string destination = inst.substr(0, idx);
    inst.erase(0, idx + 2); //to delete the comma and the space after the comma
    cout << destination << endl;


    string offset = inst.substr(0, inst.find("("));
    inst.erase(0, inst.find("(") + 1);


    string base_register = inst.substr(0, inst.find(")"));

    //store word, halfword, byte
    if (operation == "sw" || operation == "sh" || operation == "sb") {
        variables[registers[base_register] + stoi(offset)] = registers[destination];
        base_pc += 4;
    }
    else if (operation == "lw" || operation == "lh" || operation == "lb") {
        registers[destination] = variables[registers[base_register] + stoi(offset)];
        base_pc += 4;
    }

}

void assemble_U(string inst)
{
    //lui rd, imm
    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1); //erase inorder to start from x5

    idx = inst.find(",");
    string rd = inst.substr(0, idx);
    inst.erase(0, idx + 2);

    string imm = inst;


    if (rd == "x0")
    {
        base_pc += 4;
        return;
    }
    if (operation == "lui")
    {
        registers[rd] = stoi(imm) * pow(2, 12);
        base_pc += 4;
    }
    else if (rd == "auipc")
    {
        registers[rd] = (stoi(imm) * pow(2, 12)) + base_pc;
        base_pc += 4;
    }
}


//validates that the file exists
bool validate_file(string path) {
    ifstream fin;
    fin.open(path);
    if (!fin) return 0;
    else return 1;
}

void get_main_info() {

    cout << "PC: " << base_pc << endl;

    cout << "Memory: " << endl;
    for (auto var : variables) {
        cout << var.first << ": " << var.second << "    " << endl;

        cout << "In Hex: " << int_to_hex(var.first) << ": " << var.second << "    " << endl;
        cout << "In binary: " << int_to_bin(var.first) << ": " << var.second << "    " << endl;
    }

    cout << endl;

    // cout << "Register Name\t\tContent: " << endl;
    // cout << "________________________" << endl;
    for (auto reg : registers) {
        cout << reg.first << ": " << reg.second << "    ";

    }

    cout << endl;


}

//used to find labels and store instructions
void read_file(string file_path) {

    int mem_address, program_counter;
    cout << "What memory address will you start from: ";
    cin >> mem_address;
    base_pc = mem_address; //store the memory address in the program counter

    string line, label;
    ifstream fin;
    fin.open(file_path);

    if (!fin) {
        cout << "Failed to open the file given the specified file path!" << endl;
        exit(1);
    }
    else if (fin.is_open()) {

        //read file paramaters
        while (getline(fin, line)) {
            string delim = ":"; //first delimiter
            int idx = line.find(delim); //find the index of the first space
            //if it is not a label
            if (idx != string::npos) { //if we find a label
                string label_name = line.substr(0, idx);
                labels[label_name] = mem_address;
                line.erase(0, idx + 1); //erase the line
                //string instruction = line.find(" ");
            }

            memory[mem_address] = line;
            mem_address += 4;

        }
    }


    for (auto i : labels)
    {
        cout << i.first << ": " << i.second << "     ";
        cout << endl;
    }

    cout << endl;

      for (auto i : memory)
    {
        cout << i.first << ": " << i.second << "     ";
        cout << endl;
    }


      cout << endl;

}


string int_to_hex(int i) {

    std::ostringstream ss;
    ss << std::hex << i;
    std::string result = ss.str();
    return "0x" + result;
}

string int_to_bin(int n)
{

    string ret = "";

    string r;
    while (n != 0) {
        r = (n % 2 == 0 ? "0" : "1") + r;
        n /= 2;
    }
    return r;


}
