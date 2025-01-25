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
    semana::ExitCode compile(ast::ProgramAllNode* astRoot, std::string& inputFile, std::string& outputFile) {
        context.astRoot = astRoot;

        semana::SemanticAnalyzer semAnalyzer(inputFile);
        std::cout << "Starting semantic analyzer ...\n";
        semana::ExitCode exitCode = semAnalyzer.analyze(context);
        std::cout << "Semantic analyzer finished with exit code " << exitCode << ".\n";
        if (exitCode != semana::ExitCode::SUCCESS) {
            std::cerr<< "Ending ...\n";
            return exitCode;
        }

        codegen::CodeGenerator codeGenerator;
        exitCode = codeGenerator.generateCode(context);

        return exitCode;
    }
};
}  // namespace compiler

#endif  // !COMPILER_HPP
