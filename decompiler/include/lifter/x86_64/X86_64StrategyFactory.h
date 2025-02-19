#ifndef __X86_64STRATEGY_FACTORY__H
#define __X86_64STRATEGY_FACTORY__H


#include "lifter/StrategyFactory.h"
#include <llvm/Object/Binary.h>


#include <memory>

namespace lifter
{

class X86_64StrategyFactory : public StrategyFactory {
public:
    std::unique_ptr<IArchitectureStrategy> createStrategy(ExecutableType::BinaryType binType);
};

} // namespace lifter


#endif