#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "Context.hpp"
#include "ErrorMessages.hpp"
#include "ASTNode.hpp"

namespace codegen {
class CodeGenerator {
   public:
    CodeGenerator() {};
    ~CodeGenerator() {};
    semana::ExitCode generateCode(compiler::Context &context);

   private:
    void processNode(ASTNode* node);
    semana::ExitCode exitCode;
};
}  // namespace codegen

#endif  // CODE_GENERATOR_HPP
