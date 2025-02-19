cmake_minimum_required(VERSION 3.22)

add_subdirectory(logger)
add_subdirectory(settings)
add_subdirectory(utils)
add_subdirectory(lifter)
add_subdirectory(udm)
add_subdirectory(codeGen)
add_subdirectory(frontendQT)


add_executable(${PROJECT_NAME}  
    main.cpp
)

# Selecting which libraries of LLVM are being used
# and then append them to the llvm_libs variable
# so that they can be later linked to the executable
llvm_map_components_to_libnames(llvm_libs support core irreader object analysis)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        ${PROJECT_NAME}-frontendQT
        ${PROJECT_NAME}-udm
        ${PROJECT_NAME}-logger
        ${PROJECT_NAME}-utils
        ${PROJECT_NAME}-lifter
        ${PROJECT_NAME}-codeGen
        ${PROJECT_NAME}-settings
        # DO NOT MOVE LLVM UPPER(it will try to link it wrongly 
        # -> linking errors in .txt sections of own libs)
        ${llvm_libs}
        CURL::libcurl 
        spdlog::spdlog
        OpenSSL::SSL
        cpprestsdk::cpprest
        Qt6::Core Qt6::Gui Qt6::Widgets
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${INCL_DIR}
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        ${CXX_FLAGS}
)
