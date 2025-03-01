#include "Memory.hpp"

namespace codegen {

Register::Register(unsigned long address) : address(address), isLocked(false) {}

void Register::lock() {
    if (isLocked) throw std::runtime_error("Register is already locked!");
    isLocked = true;
}

void Register::unlock() {
    if (!isLocked) throw std::runtime_error("Register is already unlocked!");
    isLocked = false;
}

Memory::Memory(unsigned long lastUsedAddress)
    : firstFreeAddr(lastUsedAddress + 1) {
    // Sanity check to prevent overflow
    if (firstFreeAddr == 0 || firstFreeAddr < lastUsedAddress) {
        throw std::runtime_error("Invalid firstFreeAddr calculated.");
    }

    registers.emplace(firstFreeAddr, Register(firstFreeAddr));
    firstFreeAddr++;
}

void Memory::unlockReg(const unsigned long &regAddr) {
    auto it = registers.find(regAddr);
    if (it != registers.end()) {
        it->second.unlock();
        return;
    }
    throw std::runtime_error("Didn't find received address in memory");
}

void Memory::lockReg(const unsigned long &regAddr) {
    auto it = registers.find(regAddr);
    if (it != registers.end()) {
        it->second.lock();
        return;
    }
    throw std::runtime_error("Didn't find received address in memory");
}

unsigned long &Memory::getFreeRegister() {
    for (auto &pair : registers) {
        if (!pair.second.isLocked) {
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
    unsigned long &returnedAddr = result.first->second.address;
    firstFreeAddr++;

    return returnedAddr;
}

}  // namespace codegen
