#ifndef REVERSE_ENGINEERING_TOOL_GENERATEFILEVISITOR_H
#define REVERSE_ENGINEERING_TOOL_GENERATEFILEVISITOR_H

#include <string>
#include <memory>
#include <spdlog/spdlog.h>

#include "codeGen/ast/HllAstNodeVisitor.h"
#include "codeGen/DefineVariablesHandler.h"
#include "codeGen/ast/PHINodeHandler.h"
#include "udm/FuncInfo.h"

namespace codeGen::ast {

// forward declarations
class LlvmFunctionNode;
class LlvmInstructionNode;
class LlvmBasicBlockNode;


class GenerateFileVisitor : public HllAstNodeVisitor {
public:
    explicit GenerateFileVisitor(llvm::Function& f);
    ~GenerateFileVisitor() override = default;

    std::pair<std::string, std::string> visit(std::shared_ptr<LlvmFunctionNode> node) override;
    std::pair<std::string, std::string> visit(std::shared_ptr<LlvmInstructionNode> node) override;
    std::pair<std::string, std::string> visit(std::shared_ptr<LlvmBasicBlockNode> node) override;

    void setFuncInfo(const udm::FuncInfo& funcInfo);

    udm::FuncInfo getFuncInfo() const;
    std::unordered_map<std::string, std::vector<std::string>> getOutput() const;

    bool writeToFile(const std::string& filename);
private:
    std::unordered_map<std::string, std::vector<std::string>> output;
    std::unordered_map<std::string, std::string> singleUseVariables;
    std::unordered_map<std::string, std::string> variablesToBeReplacedAtTheEnd;
    codeGen::DefineVariablesHandler defVarHandler;
    std::map<std::string, std::vector<codeGen::Variable>> definedVariables;
    bool areVariablesDefined;
    std::string outputFilename;
    std::string lastBasicBlockName;
    unsigned int indentationLevel;
    bool inLoop;
    udm::FuncInfo funcInfo;
    llvm::Function& llvmFun;
    std::shared_ptr<spdlog::logger> logger;
private:
    void addPhiNodesValues(const std::pair<std::string, std::string>& phiNode);
    void addReturnValue(const std::string& bbLabel);
    void replaceStackVarWithAlias(const std::vector<codeGen::ast::StackVarAlias> &aliases);
    void replaceOneStackVarWithAlias(const codeGen::ast::StackVarAlias& alias);

    std::string getFinalReturnBody(const std::string& bbLabel);

    void addVariablesDefinitions();
    void replaceVarsThatNeedToBeReplacedAtEnd();
    void sanitizeReturnValuesForVoidTypeFunctions();


    bool checkAndReplaceSingleUseVars(std::shared_ptr<LlvmInstructionNode> &node);
};

}   // namespace codeGen::ast


#endif //REVERSE_ENGINEERING_TOOL_GENERATEFILEVISITOR_H
