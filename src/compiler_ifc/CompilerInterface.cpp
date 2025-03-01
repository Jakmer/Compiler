#include "CompilerInterface.hpp"

namespace compiler {

semana::ExitCode Compiler::compile(ast::ProgramAllNode* astRoot,
                                   std::string& inputFile,
                                   std::string& outputFile) {
    context.astRoot = astRoot;
    context.outputFile = outputFile;

    semana::SemanticAnalyzer semAnalyzer(inputFile);
    std::cout << "Starting semantic analyzer ...\n";
    semana::ExitCode exitCode = semAnalyzer.analyze(context);
    std::cout << "Semantic analyzer finished with exit code " << exitCode
              << ".\n";
    if (exitCode != semana::ExitCode::SUCCESS) {
        std::cerr << "Ending ...\n";
        return exitCode;
    }

    codegen::CodeGenerator codeGenerator(context);
    exitCode = codeGenerator.generateCode();

    return exitCode;
}
}  // namespace compiler
