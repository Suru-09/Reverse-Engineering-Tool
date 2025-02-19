#ifndef __LIFTER_UTILS__H
#define __LIFTER_UTILS__H

#include <iostream>
#include <memory>

#include "lifter/IArchitectureStrategy.h"
#include "lifter/LifterContext.h"

namespace utils
{

std::unique_ptr<lifter::IArchitectureStrategy> getStrategy();
std::shared_ptr<lifter::LifterContext> getLifterCtx();
void cleanDownloadedFiles(std::size_t threshold = 0);

} // namespace utils



#endif