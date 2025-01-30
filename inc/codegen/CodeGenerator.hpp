#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

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
    int line;
    Marker(std::string &name, int &line) : name(name), line(line) {}
};

class CodeGenerator {
   public:
    CodeGenerator();
    ~CodeGenerator();
    semana::ExitCode generateCode(compiler::Context &context);

   private:
    void processNode(ASTNode *node);
    int getCurrentScope();
    void addCommand(std::string &symbolName, unsigned long &address);
    void addWrite(unsigned long &address);
    void addRead(unsigned long &address);
    void addAssign(std::vector<Instruction> &instructions);
    void addJumpIfAccIsTrue(int &jump);
    void saveInstructionsToFile();
    unsigned long getFreeRegister();

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
    Memory memory;
};

}  // namespace codegen

#endif  // CODE_GENERATOR_HPP
