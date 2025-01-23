#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ErrorMessages.hpp"
#include "Context.hpp"

namespace codegen {
class CodeGenerator {
   public:
    CodeGenerator(){};
    ~CodeGenerator(){};
    semana::ExitCode generateCode(compiler::Context &context){ 
        return semana::ExitCode::SUCCESS;
    }
};
}  // namespace codegen

#endif  // CODE_GENERATOR_HPP
