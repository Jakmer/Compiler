#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <vector>

enum Opcode {
    GET,
    PUT,
    LOAD,
    STORE,
    LOADI,
    STOREI,
    ADD,
    SUB,
    ADDI,
    SUBI,
    SET,
    HALF,
    JUMP,
    JPOS,
    JZERO,
    JNEG,
    RTRN,
    HALT
};

class Instruction {
public:
    Opcode opcode;
    long value;

    Instruction(Opcode opcode, long value = 0)
        : opcode(opcode), value(value) {}

    static long getExecutionTime(Opcode opcode) {
        return executionTimes[opcode];
    }

private:
    static const std::vector<long> executionTimes;
};

const std::vector<long> Instruction::executionTimes = {
    100, // GET
    100, // PUT
    10,  // LOAD
    10,  // STORE
    20,  // LOADI
    20,  // STOREI
    10,  // ADD
    10,  // SUB
    20,  // ADDI
    20,  // SUBI
    50,  // SET
    5,   // HALF
    1,   // JUMP
    1,   // JPOS
    1,   // JZERO
    1,   // JNEG
    10,  // RTRN
    0    // HALT
};

#endif // !INSTRUCTIONS_HPP
