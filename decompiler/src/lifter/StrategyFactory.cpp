#include "lifter/StrategyFactory.h"

#include "lifter/x86_64/X86_64StrategyFactory.h"
#include "lifter/arm64/ARM64StrategyFactory.h"

#include <iostream>

std::shared_ptr<lifter::StrategyFactory> lifter::StrategyFactory::getArch(llvm::Triple::ArchType arch) {
    const char* archStr = llvm::Triple::getArchTypeName(arch).data();
    switch(arch) {
        case llvm::Triple::x86_64:
            return std::make_shared<lifter::X86_64StrategyFactory>();
        case llvm::Triple::aarch64:
            return std::make_shared<lifter::ARM64StrategyFactory>();
        default:
            std::cout << "Architecture: [" << archStr << "] is not supported yet!";
            exit(1);
    }   
}
