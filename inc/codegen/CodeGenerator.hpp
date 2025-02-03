#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <unordered_map>
#include <vector>

#include "ASTNode.hpp"
#include "Command.hpp"
#include "Context.hpp"
#include "ErrorMessages.hpp"
#include "InstructNodes.hpp"
#include "Instructions.hpp"
#include "Memory.hpp"

namespace codegen {

struct Marker {
    std::string name;
    long line;
    Marker(std::string &name, int &line) : name(name), line(line) {}
};

class CodeGenerator {
   public:
    CodeGenerator(compiler::Context &context);
    ~CodeGenerator();
    semana::ExitCode generateCode();

   private:
    void processNode(ASTNode *node);
    int getCurrentScope();
    void addCommand(std::string &symbolName, unsigned long &address);
    void addHalt();
    void addWrite(unsigned long &address);
    void addRead(unsigned long &address);
    void addAssign(std::vector<Instruction> &instructions);
    void addJumpIfAccIsTrue(int &jump);
    void setRValues();
    void jumpToMain();
    void saveInstructionsToFile();
    unsigned long getFreeRegister();
    long getMarkerForName(std::string &name);
    void updateOpcode(Opcode &opcode);
    bool isProcArgument(std::string &argName, int &scope);


    semana::ExitCode exitCode;
    std::vector<Instruction> instructions;
    std::vector<Marker> markers;  // used to mark procedures - when they start
    int lineCounter;
    Command currentCommand;
    std::string currentProcName;
    compiler::Context context;
    unsigned long accAddr;
    AssignNode assignNode;
    ConditionNode conditionNode;
    RepeatNode repeatNode;
    WhileNode whileNode;
    ForNode forNode;
    Memory memory;
    int noConditions;
    int noRepeats;
    int noWhiles;   // wrap these counters and nodes into struct
    int noFors;
    std::vector<unsigned long> heap;
    std::unordered_map<unsigned long, unsigned long> arrayPointers;
    std::string currProcCallName;
};

}  // namespace codegen

#endif  // CODE_GENERATOR_HPP
