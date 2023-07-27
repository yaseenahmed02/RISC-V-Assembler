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

// Global variables and data structures
map <string, int> registers; // 32 registers for RISC-V architecture
map<string, int> labels; // Stores label names and their corresponding memory addresses
map<int, string> memory; // Main memory containing instructions
map<int, int> variables; // Stores any variables with their existing values in memory
int base_pc = 0; // Program counter (points to the current instruction to be executed)
string working_file_path = ""; // Path of the program file that will be executed
bool halt = 0; // Flag to indicate whether the program should halt execution

// Function declarations
void init_registers(); // Initializes the registers with default values
void run_program(); // Starts the execution of the program
bool validate_file(string path); // Validates the file path provided by the user
void assemble_code(string instruction); // Processes and executes the given instruction

// Function declarations for different instruction types
void assemble_branching(string instruction); // Handles branching instructions (e.g., beq, bne, etc.)
void assemble_LS(string instruction); // Handles load and store instructions (e.g., lw, sw, etc.)
void assemble_J(string instruction); // Handles jump instructions (e.g., jal, jalr, etc.)
void assemble_U(string instruction); // Handles upper immediate instructions (e.g., lui, auipc)
void assemble_AL(string instruction); // Handles arithmetic and logic instructions (e.g., add, sub, andi, etc.)

// Helper functions
string int_to_hex(int value); // Converts an integer value to its hexadecimal representation
string int_to_bin(int value); // Converts an integer value to its binary representation
void read_file(string file_path); // Reads the program instructions from the specified file
void get_main_info(); // Displays information about the current state of the program execution

int main() {

    init_registers(); // Initialize the registers with default values
    run_program(); // Start executing the program

    // Display the final results after program execution
    cout << "The final results are: " << endl;
    get_main_info();

    return 0;
}

// Initializes the 32 registers with default values (all set to 0)
void init_registers() {
    string x = "x", num = "", reg_name = "";
    for (int i = 1; i <= 32; i++) {
        num = to_string(i);
        reg_name = x + num;
        registers[reg_name] = 0;
    }
}

// Runs the program, reads the instructions from the file, and executes them
void run_program() {
    string file_path;

    // Prompt the user to enter the file path containing the program instructions
    cout << "Please enter the path of your file: ";
    cin >> file_path;

    // Validate the provided file path
    while (!validate_file(file_path)) {
        cout << "Invalid file path, try again: ";
        cin >> file_path;
    }

    // Read the program instructions from the file and store them in memory
    read_file(file_path);

    char input;
    cout << "Enter 'y' if you have any variables to add, else enter 'n': " << endl;
    cin >> input;

    // Prompt the user to enter variables and their values
    while (input != 'y' && input != 'n') {
        cout << "Invalid input: Are there any variables that you want to add to memory? (y/n) " << endl;
        cin >> input;
    }

    // If the user wants to add variables to memory
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

    // Start executing the program instructions
    do {
        get_main_info();
        assemble_code(memory[base_pc]);
    } while (halt == false);
}

// Processes the given instruction and performs the corresponding operation
void assemble_code(string inst) {
    vector<string> branching = { "beq", "bge", "blt", "bltu", "bne", "bgeu" };
    vector<string> LS = { "lb", "lbu", "lh", "lhu", "lw", "sb", "sh", "sw" };
    vector<string> J = { "jal", "jalr" };
    vector<string> U = { "lui", "auipc" };
    vector<string> AL = { "add", "sub", "sll", "slt", "sltu", "xor", "srl", "sra", "or", "and", "addi", "slti", "sltiu", "xori", "ori", "andi", "slli", "srli", "srai" };
    vector<string> halting = { "ebreak", "ecall", "fence" };

    int idx = inst.find(" ");
    string sub_str = inst.substr(0, idx);

    // Check if the instruction is a halting operation
    for (auto i : halting) {
        int found = inst.find(i);
        if (found != string::npos) {
            halt = 1;
            return;
        }
    }

    cout << "Instruction is: " << inst << endl;

    // Determine the type of instruction and call the appropriate function
    if (find(branching.begin(), branching.end(), sub_str) != branching.end()) {
        assemble_branching(inst);
    }
    else if (find(LS.begin(), LS.end(), sub_str) != LS.end()) {
        assemble_LS(inst);
    }
    else if (find(J.begin(), J.end(), sub_str) != J.end()) {
        assemble_J(inst);
    }
    else if (find(U.begin(), U.end(), sub_str) != U.end()) {
        assemble_U(inst);
    }
    else if (find(AL.begin(), AL.end(), sub_str) != AL.end()) {
        assemble_AL(inst);
    }
}

// Processes arithmetic and logic instructions
void assemble_AL(string inst)
{
    // Example instruction format: "add x5, x6, x7"
    
    int idx = inst.find(" ");
    string operation = inst.substr(0, idx); // Extract the operation (e.g., "add", "sub", etc.)
    inst.erase(0, idx + 1); // Remove the operation from the instruction to get the remaining operands

    idx = inst.find(",");
    string rd = inst.substr(0, idx); // Extract the destination register (rd) name
    inst.erase(0, idx + 2);

    idx = inst.find(",");
    string rs1 = inst.substr(0, idx); // Extract the first source register (rs1) name
    inst.erase(0, idx + 2);

    string rs2 = inst; // The remaining part is the second source register (rs2)

    cout << rd << " " << rs1 << " " << rs2 << endl;

    // Handling the case when the destination register is x0 (no operation needed)
    if (rd == "x0")
    {
        base_pc += 4;
        return;
    }

    // Perform the specified arithmetic or logic operation and update the destination register
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
        base_pc + 4; // NOTE: There is a typo here, it should be base_pc += 4;
    }
    else if (operation == "slt")
    {
        if (registers[rs1] < registers[rs2])
            registers[rd] = 1;
        else
            registers[rd] = 0;
        base_pc += 4;
    }
    else if (operation == "sltu")
    {
        if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
            registers[rd] = 1;
        else
            registers[rd] = 0;
        base_pc += 4;
    }
    else if (operation == "xor")
    {
        registers[rd] = registers[rs1] ^ registers[rs2]; // ^ is the XOR operator
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
        registers[rd] = registers[rs1] + stoi(rs2); // stoi() converts the second operand to an integer
        base_pc += 4;
    }
    else if (operation == "slti")
    {
        if (registers[rs1] < stoi(rs2))
            registers[rd] = 1;
        else
            registers[rd] = 0;
        base_pc += 4;
    }
    else if (operation == "sltiu")
    {
        if ((unsigned int)registers[rs1] < stoi(rs2))
            registers[rd] = 1;
        else
            registers[rd] = 0;
        base_pc += 4;
    }
    else if (operation == "xori")
    {
        registers[rd] = registers[rs1] ^ stoi(rs2); // ^ is the XOR operator
        base_pc += 4;
    }
    else if (operation == "ori")
    {
        registers[rd] = registers[rs1] | registers[rs2]; // ^ is the OR operator
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
        base_pc + 4; // NOTE: There is a typo here, it should be base_pc += 4;
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

// Processes jump instructions (jal and jalr)
void assemble_J(string inst) {

    // Example instruction format: "jal x5, label" or "jalr x1, 100(x5)"

    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1);

    // If the instruction is a "jal" (Jump and Link) operation
    if (operation == "jal") {

        int curr_idx = inst.find(",");
        string destination = inst.substr(0, curr_idx); // Extract the destination register name (rd)
        inst.erase(0, curr_idx + 2); // Remove the destination register part from the instruction

        // If the destination register is x0, no operation needed
        if (destination != "x0") {
            registers[destination] = base_pc + 4; // Save the current PC + 4 (return address) to the destination register
        }

        string label = inst; // The remaining part is the label to jump to
        base_pc = labels[label]; // Update the program counter to the address of the label

    }
    // If the instruction is a "jalr" (Jump and Link Register) operation
    else if (operation == "jalr") {
        string current_inst = inst; // Example: "x1, 100(x5)";
        int curr_idx = inst.find(",");
        string destination = inst.substr(0, curr_idx); // Extract the destination register name (rd)
        inst.erase(0, idx + 2); // Remove the destination register part from the instruction

        string offset = inst.substr(0, inst.find("(")); // Extract the offset value
        inst.erase(0, inst.find("(") + 1);
        string base_register = inst.substr(0, inst.find(")")); // Extract the base register name

        // If the destination register is x0, no operation needed
        if (destination == "x0") {
            base_pc += 4;
            return;
        }
        else {
            registers[destination] = base_pc + 4; // Save the current PC + 4 (return address) to the destination register
            base_pc = registers[base_register] + stoi(offset); // Update the program counter to (base register + offset)
        }
    }
}

// Processes branching instructions (beq, bge, bne, blt, bltu, bgeu)
void assemble_branching(string inst)
{
    // Example instruction format: "beq x5, x6, label" or "bge rs1, rs2, label"

    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1);

    // Extract the first source register (rs1)
    idx = inst.find(",");
    string rs1 = inst.substr(0, idx);
    inst.erase(0, idx + 2);

    // Extract the second source register (rs2)
    idx = inst.find(",");
    string rs2 = inst.substr(0, idx);
    inst.erase(0, idx + 2);

    // The remaining part is the label to jump to
    string label = inst;

    // Print the extracted values for debugging purposes (optional)
    cout << operation << endl;
    cout << rs1 << endl;
    cout << rs2 << endl;
    cout << label << endl;

    // Execute the corresponding branching operation based on the instruction
    if (operation == "beq")
    {
        if (registers[rs1] == registers[rs2])
            base_pc = labels[label]; // Jump to the label's address if rs1 and rs2 are equal
        else
            base_pc += 4; // Otherwise, proceed to the next instruction
    }
    else if (operation == "bge")
    {
        if (registers[rs1] >= registers[rs2])
            base_pc = labels[label]; // Jump to the label's address if rs1 is greater than or equal to rs2
        else
            base_pc += 4; // Otherwise, proceed to the next instruction
    }
    else if (operation == "bne")
    {
        if (registers[rs1] != registers[rs2])
            base_pc = labels[label]; // Jump to the label's address if rs1 and rs2 are not equal
        else
            base_pc += 4; // Otherwise, proceed to the next instruction
    }
    else if (operation == "blt")
    {
        if (registers[rs1] < registers[rs2])
            base_pc = labels[label]; // Jump to the label's address if rs1 is less than rs2
        else
            base_pc += 4; // Otherwise, proceed to the next instruction
    }
    else if (operation == "bltu")
    {
        if ((unsigned int)registers[rs1] < (unsigned int)registers[rs2])
            base_pc = labels[label]; // Jump to the label's address if rs1 is less than rs2 (unsigned comparison)
        else
            base_pc += 4; // Otherwise, proceed to the next instruction
    }
    else if (operation == "bgeu")
    {
        if ((unsigned int)registers[rs1] >= (unsigned int)registers[rs2])
            base_pc = labels[label]; // Jump to the label's address if rs1 is greater than or equal to rs2 (unsigned comparison)
        else
            base_pc += 4; // Otherwise, proceed to the next instruction
    }
}


// Processes load and store instructions (lw, lh, lb, sw, sh, sb)
void assemble_LS(string inst) {

    // Example instruction format: "lw x5, 100(x6)" or "sb x1, 10(x2)"

    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1);

    // Extract the destination register (rd) or source register (rs) depending on the operation
    idx = inst.find(",");
    string destination = inst.substr(0, idx);
    inst.erase(0, idx + 2); // Remove the destination/source register part from the instruction

    // Extract the offset value from the instruction
    string offset = inst.substr(0, inst.find("("));
    inst.erase(0, inst.find("(") + 1);

    // Extract the base register name from the instruction
    string base_register = inst.substr(0, inst.find(")"));

    // Print the extracted values for debugging purposes (optional)
    cout << destination << endl;
    cout << offset << endl;
    cout << base_register << endl;

    // Store word, halfword, or byte into memory
    if (operation == "sw" || operation == "sh" || operation == "sb") {
        variables[registers[base_register] + stoi(offset)] = registers[destination];
        base_pc += 4;
    }
    // Load word, halfword, or byte from memory
    else if (operation == "lw" || operation == "lh" || operation == "lb") {
        registers[destination] = variables[registers[base_register] + stoi(offset)];
        base_pc += 4;
    }
}

// Processes U-type instructions (lui and auipc)
void assemble_U(string inst)
{
    // Example instruction format: "lui rd, imm" or "auipc rd, imm"

    int idx = inst.find(" ");
    string operation = inst.substr(0, idx);
    inst.erase(0, idx + 1);

    // Extract the destination register (rd)
    idx = inst.find(",");
    string rd = inst.substr(0, idx);
    inst.erase(0, idx + 2);

    // The remaining part is the immediate value (imm)
    string imm = inst;

    // If the destination register is x0, no operation needed
    if (rd == "x0")
    {
        base_pc += 4;
        return;
    }

    // Execute the corresponding U-type operation based on the instruction
    if (operation == "lui")
    {
        registers[rd] = stoi(imm) * pow(2, 12); // Load the immediate value to the destination register (rd)
        base_pc += 4;
    }
    else if (operation == "auipc")
    {
        registers[rd] = (stoi(imm) * pow(2, 12)) + base_pc; // Load the immediate value + current PC to the destination register (rd)
        base_pc += 4;
    }
}

// Validates that the file exists
bool validate_file(string path) {
    ifstream fin;
    fin.open(path);
    if (!fin)
        return false; // Return false if the file does not exist
    else
        return true; // Return true if the file exists
}

// Display information about the program's current state (PC, Memory, Registers)
void get_main_info() {

    cout << "PC: " << base_pc << endl;

    cout << "Memory: " << endl;
    for (auto var : variables) {
        cout << var.first << ": " << var.second << "    " << endl;

        cout << "In Hex: " << int_to_hex(var.first) << ": " << var.second << "    " << endl;
        cout << "In binary: " << int_to_bin(var.first) << ": " << var.second << "    " << endl;
    }

    cout << endl;

    // Display the content of all registers
    for (auto reg : registers) {
        cout << reg.first << ": " << reg.second << "    ";
    }

    cout << endl;
}

// Reads the program from the file and populates memory and labels
void read_file(string file_path) {

    int mem_address, program_counter;
    cout << "What memory address will you start from: ";
    cin >> mem_address;
    base_pc = mem_address; // Store the memory address in the program counter

    string line, label;
    ifstream fin;
    fin.open(file_path);

    if (!fin) {
        cout << "Failed to open the file given the specified file path!" << endl;
        exit(1);
    }
    else if (fin.is_open()) {

        // Read file parameters
        while (getline(fin, line)) {
            string delim = ":"; // First delimiter
            int idx = line.find(delim); // Find the index of the first colon (to detect labels)
            // If it is not a label
            if (idx != string::npos) { // If we find a label
                string label_name = line.substr(0, idx);
                labels[label_name] = mem_address; // Store the label and its corresponding memory address
                line.erase(0, idx + 1); // Erase the label part from the line
            }

            memory[mem_address] = line; // Store the instruction in the memory at the current memory address
            mem_address += 4; // Increment the memory address for the next instruction

        }
    }

    // Display the labels and their corresponding memory addresses
    for (auto i : labels) {
        cout << i.first << ": " << i.second << "     ";
        cout << endl;
    }

    cout << endl;

    // Display the contents of the memory (instructions)
    for (auto i : memory) {
        cout << i.first << ": " << i.second << "     ";
        cout << endl;
    }

    cout << endl;
}

// Convert an integer to a hexadecimal string (prefix with "0x")
string int_to_hex(int i) {
    std::ostringstream ss;
    ss << std::hex << i;
    std::string result = ss.str();
    return "0x" + result;
}

// Convert an integer to a binary string
string int_to_bin(int n) {
    string ret = "";

    string r;
    while (n != 0) {
        r = (n % 2 == 0 ? "0" : "1") + r;
        n /= 2;
    }
    return r;
}



