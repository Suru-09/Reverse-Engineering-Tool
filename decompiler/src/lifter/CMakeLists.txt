cmake_minimum_required(VERSION 3.22)

add_library(${PROJECT_NAME}-lifter
    arm64/ARM64Strategy.cpp
    ExecutableType.cpp
    LifterContext.cpp
    StrategyFactory.cpp
    x86_64/X86_64ELFStrategy.cpp
    x86_64/X86_64StrategyFactory.cpp
    arm64/ARM64StrategyFactory.cpp
    server/RetdecClient.cpp
)

target_include_directories(${PROJECT_NAME}-lifter
    PRIVATE
        ${INCL_DIR}
)