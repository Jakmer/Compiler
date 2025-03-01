#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "ErrorMessages.hpp"
#include "ProgramAllNode.hpp"
#include "SemanticAnalzyer.hpp"
#include "CodeGenerator.hpp"
#include "Context.hpp"

namespace compiler {

class Compiler {
    Context context;

   public:
    semana::ExitCode compile(ast::ProgramAllNode* astRoot, std::string& inputFile, std::string& outputFile);
};
}  // namespace compiler

#endif  // !COMPILER_HPP
