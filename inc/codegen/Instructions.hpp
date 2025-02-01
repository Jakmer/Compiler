#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <ostream>
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
    HALT,
    UNDEFINED
};

enum InstructionMode {
    VALUE,
    LABEL,
    RVALUE,
};

class Instruction {
   public:
    Opcode opcode;
    long value;
    std::string label;
    InstructionMode mode;
    bool doNotModify;

    Instruction(Opcode opcode, long value = 0)
        : opcode(opcode), value(value), mode(VALUE), doNotModify(false) {}
    Instruction(Opcode opcode, std::string& label,
                InstructionMode mode = LABEL)  // constructor for label jump
        : opcode(opcode), label(label), mode(mode), doNotModify(false) {}
    Instruction(Opcode opcode, long value, bool treatAsPointer)
        : opcode(opcode), value(value), doNotModify(treatAsPointer) {}

    static long getExecutionTime(Opcode opcode) {
        return executionTimes[opcode];
    }

   private:
    static const std::vector<long> executionTimes;
};

inline std::ostream& operator<<(std::ostream& os, const Opcode& opcode) {
    switch (opcode) {
        case GET:
            os << "GET";
            break;
        case PUT:
            os << "PUT";
            break;
        case LOAD:
            os << "LOAD";
            break;
        case STORE:
            os << "STORE";
            break;
        case LOADI:
            os << "LOADI";
            break;
        case STOREI:
            os << "STOREI";
            break;
        case ADD:
            os << "ADD";
            break;
        case SUB:
            os << "SUB";
            break;
        case ADDI:
            os << "ADDI";
            break;
        case SUBI:
            os << "SUBI";
            break;
        case SET:
            os << "SET";
            break;
        case HALF:
            os << "HALF";
            break;
        case JUMP:
            os << "JUMP";
            break;
        case JPOS:
            os << "JPOS";
            break;
        case JZERO:
            os << "JZERO";
            break;
        case JNEG:
            os << "JNEG";
            break;
        case RTRN:
            os << "RTRN";
            break;
        case HALT:
            os << "HALT";
            break;
        case UNDEFINED:
            os << "UNDEFINED";
            break;
        default:
            os << "UNKNOWN_OPCODE";
            break;
    }
    return os;
}

#endif  // !INSTRUCTIONS_HPP
