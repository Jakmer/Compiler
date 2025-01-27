#include "Symbol.hpp"

namespace semana {

std::string toString(const SymbolType &symbolType) {
    switch (symbolType) {
        case SymbolType::VARIABLE:
            return "Variable";
        case SymbolType::PROCEDURE:
            return "Procedure";
        case SymbolType::ARRAY:
            return "Array";
        case SymbolType::RVALUE:
            return "Rvalue";
        case SymbolType::ITERATOR:
            return "Iterator";
        case SymbolType::ARRAY_ELEMENT:
            return "Array element";
        case SymbolType::PROCEDURE_ARG:
            return "Procedure argument";
        default:
            throw std::invalid_argument("Unkonw symbol type");
    }
}

}
