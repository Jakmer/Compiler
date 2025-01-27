#include "Instructions.hpp"

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

