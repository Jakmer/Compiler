# Compiler

This project contains a compiler built using BISON and FLEX, which translates code written in a simple imperative language into a virtual machine's instruction set. The virtual machine then executes this code based on a predefined set of operations.

Core of compiler is written in **C++** and tests are supported by **Bash**, **Python** and **Valgrind**.

## Imperative Language

Description of the language's functionality in bullet points:

- **Integer Arithmetic**: Supports addition, subtraction, multiplication, division, and modulo operations.
- **Variables and Arrays**:
  Integer variables can be declared and assigned values.
  Arrays can be declared with a specific index range (e.g., array[-10:10]).
- **Control Flow**:
  IF-THEN-ELSE for conditional execution.
  WHILE loops for repetitive execution with a condition.
  REPEAT-UNTIL loops that execute at least once before checking the condition.
  FOR loops with an iterator, supporting both increasing (TO) and decreasing (DOWNTO) sequences.
- **Procedures**:
- **Input/Output**:
  READ reads an integer from input and assigns it to a variable.
  WRITE prints a variable or number to output.
- **Syntax Rules**:
  Case-sensitive.
  Comments start with # and extend to the end of the line.
  Identifiers follow the pattern [_a-z]+ (lowercase letters and underscores).
  Integer constants are 64-bit in the source code but have no limit in execution.

Full specification in **specification.pdf**

Example program - **program.imp**:

```plaintext
# The binary representation of a number 

PROGRAM IS
        n, p
BEGIN
    READ n;
    REPEAT
        p:=n/2;
        p:=2*p;
        IF n>p THEN
            WRITE 1;
        ELSE
            WRITE 0;
        ENDIF
        n:=n/2;
    UNTIL n=0;
END
```

## Dependencies

To compile successfully you need to install the following tools:

    sudo apt install g++ make cmake flex bison

## Compilation

Compiler compilation is done by **Cmake**:

    mkdir build
    cd build
    cmake ..
    make

Virtual machine compilation is done by **Make**:

        cd vm
        make
Clean VM:

        make cleanall


## Usage

Compile program into assembly:

    ./compiler <input> <output>

Run compiled program with virtual machine:

    ./maszyna-wirtualna <output>


## Testing

There are provided three types of test

- Memory tests:

        make valgrind_test

- Tests checking if compiler puts errors for incorrect programs:

        make test

- Python tests for expected output:

        cd vm
        make
        python3 ../test/test-runner.py
        make cleanall

## Virtual Machine

**Author of virtual machine is Professor Maciej Gębala.**

The virtual machine consists of a program counter (**k**) and a series of memory cells (**p<sub>i</sub>**
), where **i** ranges from **0** to **2⁶⁴**. The first memory cell, **p<sub>0</sub>**
, acts as an accumulator. The program is a sequence of instructions executed one by one until the **HALT** instruction is encountered. The initial contents of the memory cells are undefined, and the program counter starts at **0**.

The machine operates on **64-bit integers** and executes commands that modify the state of the accumulator or perform memory operations. Each instruction has a specified execution cost, and the program flow is directed by jumps and conditional checks.


Instruction set:

| Instruction | Interpretation | Execution Time |
|-------------|----------------|----------------|
| **GET i**       | stores the input number in memory cell p<sub>i</sub> and k ← k + 1 | 100 |
| **PUT i**       | displays the content of memory cell p<sub>i</sub> and k ← k + 1 | 100 |
| **LOAD i**      | p<sub>0</sub> ← p<sub>i</sub> and k ← k + 1 | 10 |
| **STORE i**     | p<sub>i</sub> ← p<sub>0</sub> and k ← k + 1 | 10 |
| **LOADI i**     | p<sub>0</sub> ← p<sub>p<sub>i</sub></sub> and k ← k + 1 | 20 |
| **STOREI i**    | p<sub>p<sub>i</sub></sub> ← p<sub>0</sub> and k ← k + 1 | 20 |
| **ADD i**       | p<sub>0</sub> ← p<sub>0</sub> + p<sub>i</sub> and k ← k + 1 | 10 |
| **SUB i**       | p<sub>0</sub> ← p<sub>0</sub> − p<sub>i</sub> and k ← k + 1 | 10 |
| **ADDI i**      | p<sub>0</sub> ← p<sub>0</sub> + p<sub>p<sub>i</sub></sub> and k ← k + 1 | 20 |
| **SUBI i**      | p<sub>0</sub> ← p<sub>0</sub> − p<sub>p<sub>i</sub></sub> and k ← k + 1 | 20 |
| **SET x**       | p<sub>0</sub> ← x and k ← k + 1 | 50 |
| **HALF**        | p<sub>0</sub> ← ⌊ p<sub>0</sub> / 2 ⌋ and k ← k + 1 | 5 |
| **JUMP j**      | k ← k + j | 1 |
| **JPOS j**      | if p<sub>0</sub> > 0 then k ← k + j, otherwise k ← k + 1 | 1 |
| **JZERO j**     | if p<sub>0</sub> = 0 then k ← k + j, otherwise k ← k + 1 | 1 |
| **JNEG j**      | if p<sub>0</sub> < 0 then k ← k + j, otherwise k ← k + 1 | 1 |
| **RTRN i**      | k ← p<sub>i</sub> | 10 |
| **HALT**        | stop the program | 0 |