#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <limits>

namespace codegen {

class Register {
   public:
    Register(unsigned long address);
    void lock();
    void unlock();

    unsigned long address;
    bool isLocked;  // if true then memory class won't return it as free register
};

class Memory {
   public:
    Memory() = default;
    Memory(unsigned long lastUsedAddress);
    void unlockReg(const unsigned long &regAddr);
    void lockReg(const unsigned long &regAddr);
    unsigned long& getFreeRegister();

   private:
    unsigned long firstFreeAddr;
    std::unordered_map<unsigned long, Register> registers;
};

}  // namespace codegen

#endif  // MEMORY_HPP
