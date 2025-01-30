#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <limits>

namespace codegen {

class Register {
   public:
    unsigned long address;
    bool isLocked;  // if true then memory class won't return it as free register

    Register(unsigned long address) : address(address), isLocked(false) {}
    
    void lock() {
        if (isLocked) throw std::runtime_error("Register is already locked!");
        isLocked = true;
    }
    
    void unlock() {
        if (!isLocked)
            throw std::runtime_error("Register is already unlocked!");
        isLocked = false;
    }
};

class Memory {
   public:
    Memory() = default;
    Memory(unsigned long lastUsedAddress) : firstFreeAddr(lastUsedAddress + 1) {
        // Sanity check to prevent overflow
        if (firstFreeAddr == 0 || firstFreeAddr < lastUsedAddress) {
            throw std::runtime_error("Invalid firstFreeAddr calculated.");
        }

        registers.emplace(firstFreeAddr, Register(firstFreeAddr));
        firstFreeAddr++;
    }

    void unlockReg(const unsigned long &regAddr){
        auto it = registers.find(regAddr);
        if(it != registers.end()){
            it->second.unlock();
            return;
        }
        throw std::runtime_error("Didn't find received address in memory");
    }

    void lockReg(const unsigned long &regAddr){
        auto it = registers.find(regAddr);
        if(it != registers.end()){
            it->second.lock();
            return;
        }
        throw std::runtime_error("Didn't find received address in memory");
    }

    unsigned long& getFreeRegister() {
        for(auto &pair : registers){
            if(!pair.second.isLocked){
                return pair.second.address;
            }
        }
    
        // Check for potential overflow
        if (firstFreeAddr == std::numeric_limits<unsigned long>::max()) {
            throw std::runtime_error("No more free addresses available.");
        }
    
        auto result = registers.emplace(firstFreeAddr, Register(firstFreeAddr));
        if (!result.second) {
            throw std::runtime_error("Failed to insert new register.");
        }
        unsigned long& returnedAddr = result.first->second.address;
        firstFreeAddr++;
    
        return returnedAddr;
    }

   private:
    unsigned long firstFreeAddr;
    std::unordered_map<unsigned long, Register> registers;
};

}  // namespace codegen

#endif  // MEMORY_HPP
