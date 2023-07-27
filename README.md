
# RISC-V Assembler Project

This project is a RISC-V assembler implemented in C++ that takes RISC-V assembly code as input and converts it into machine code. The assembler supports a subset of RISC-V instructions, including branching, load/store, jump, arithmetic, and logic instructions.

## Contributors

- Yaseen Ahmed
- Karim Sherif
- Farah Kabesh

## Submitted on

April 20, 2022

## Project Overview

The assembler is capable of reading RISC-V assembly code from a file, converting it into machine code, and simulating the execution of the program. It supports 32 registers (x0 to x31) and provides functionalities for load/store operations, branching, jumps, arithmetic, and logic operations.

## Getting Started

### Prerequisites

- C++ compiler
- Input file containing RISC-V assembly code (with valid file path)

### Instructions to Run

1. Clone the repository or download the source code.
2. Compile the code using a C++ compiler.
3. Execute the compiled binary.
4. Enter the file path of the input file containing RISC-V assembly code.
5. Follow the on-screen instructions for further input (if any).

## Supported RISC-V Instructions

The assembler supports the following RISC-V instructions:

### Branching Instructions

- beq: Branch if equal
- bne: Branch if not equal
- bge: Branch if greater than or equal to
- blt: Branch if less than
- bgeu: Branch if greater than or equal to (unsigned)
- bltu: Branch if less than (unsigned)

### Load/Store Instructions

- lb: Load byte
- lbu: Load byte unsigned
- lh: Load halfword
- lhu: Load halfword unsigned
- lw: Load word
- sb: Store byte
- sh: Store halfword
- sw: Store word

### Jump Instructions

- jal: Jump and link
- jalr: Jump and link register

### Arithmetic and Logic Instructions

- add: Add
- sub: Subtract
- sll: Shift left logical
- slt: Set less than
- sltu: Set less than unsigned
- xor: Exclusive OR
- srl: Shift right logical
- sra: Shift right arithmetic
- or: Bitwise OR
- and: Bitwise AND
- addi: Add immediate
- slti: Set less than immediate
- sltiu: Set less than immediate unsigned
- xori: Exclusive OR immediate
- ori: Bitwise OR immediate
- andi: Bitwise AND immediate
- slli: Shift left logical immediate
- srli: Shift right logical immediate
- srai: Shift right arithmetic immediate

### Halt Instructions

- ebreak: Breakpoint
- ecall: Environment call
- fence: Fence instruction

## Input File Format

The input file should contain RISC-V assembly code written line-by-line. The assembler will parse this file to generate machine code for each instruction.


The assembler will store the memory address of the label and use it for branching and jump instructions.

## Sample Usage

```cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>

//... (The rest of the code)

int main() {
    // Initialization and run the assembler
    init_registers();
    run_program();

    // Display final results
    cout << "The final results are: " << endl;
    get_main_info();
    return 0;
}
```

## Test Cases 
The following are some sample test cases used to demonstrate the functionality of the RISC-V assembler: 
### Finding Max  (`findingmax.txt`)  

```assembly 
addi x9, x0, 1000 
addi x18, x0, 1004 
lw x10, 0(x9) 
lw x11, 0(x18) 
bge x10, x11, yes 
jal x0, no 
yes: add x11, x0, x10
no: ebreak
```

### Sample Output
```
PC: 32

Memory:
1000: 1000
1004: 1004

Register Name      Content:
x0: 				0      
x1: 				0      
x2: 				0      
...   
x30: 				0      
x31: 				0

```
### Finding Min (`findingmin.txt`)
### Swap (`swap.txt`)


## Limitations

The assembler supports a subset of RISC-V instructions and may not handle certain advanced instructions or features.


## Contributing

We welcome contributions to this project. If you find any issues or have suggestions for improvement, feel free to open an issue or submit a pull request.


## Acknowledgments

Special thanks to the RISC-V community for providing the RISC-V instruction set architecture and specifications.
