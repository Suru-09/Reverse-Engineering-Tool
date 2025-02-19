#include "udm/IntervalGraph.h"
#include "logger/LoggerManager.h"

#include "utils/UdmUtils.h"

#include <iostream>
#include <vector>

#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/IR/Instructions.h>

#include <spdlog/spdlog.h>

udm::IntervalGraph::IntervalGraph(llvm::PostDominatorTree& pdt)
:
dt(pdt)
{
    logger = logger::LoggerManager::getInstance()->getLogger("udm");
}

void udm::IntervalGraph::setIntervals(const std::vector<Interval>& intervals)
{
    this->intervals = intervals;
}

bool udm::IntervalGraph::addInterval(Interval interval)
{
    auto found = std::find_if(intervals.begin(), intervals.end(), [&interval](Interval i) {
        return i.getFirstBlock()->getName() == interval.getFirstBlock()->getName();
    });

    if(found != intervals.end())
    {
        return false;
    }

    intervals.push_back(interval);
    return true;
}

bool udm::IntervalGraph::containsInterval(Interval interval) const
{
    auto found = std::find_if(intervals.begin(), intervals.end(), [&interval](Interval i) {
        return i == interval;
    });

    if(found == intervals.end())
    {
        return false;
    }

    return true;
}

udm::IntervalGraph::iterator udm::IntervalGraph::begin() noexcept
{
    return intervals.begin();
}

udm::IntervalGraph::const_iterator udm::IntervalGraph::begin() const noexcept
{
    return intervals.begin();
}

udm::IntervalGraph::const_iterator udm::IntervalGraph::cbegin() const noexcept
{
    return intervals.cbegin();
}

udm::IntervalGraph::reverse_iterator udm::IntervalGraph::rbegin() noexcept
{
    return intervals.rbegin();
}

udm::IntervalGraph::iterator udm::IntervalGraph::end() noexcept
{
    return intervals.end();
}

udm::IntervalGraph::const_iterator udm::IntervalGraph::end() const noexcept
{
    return intervals.end();
}

udm::IntervalGraph::const_iterator udm::IntervalGraph::cend() const noexcept
{
    return intervals.cend();
}

udm::IntervalGraph::reverse_iterator udm::IntervalGraph::rend() noexcept
{
    return intervals.rend();
}

size_t udm::IntervalGraph::size() const noexcept
{
    return intervals.size();
}

bool udm::IntervalGraph::empty() const noexcept
{
    return intervals.empty();
}

udm::Interval& udm::IntervalGraph::operator[](size_t index) noexcept
{
    return intervals[index];
}

const udm::Interval& udm::IntervalGraph::operator[](size_t index) const noexcept
{
    return intervals[index];
}

void udm::IntervalGraph::setHeadersOfIntervals(udm::Interval& headers, FuncInfo& funcInfo)
{
    for(const auto& h: headers)
    {
        if(funcInfo.exists(h->getName().str()))
        {
            funcInfo[h->getName().str()].setIsHeader(true);
        }
    }
}

std::vector<udm::Interval> udm::IntervalGraph::intervalsGraph(llvm::Function& f, udm::FuncInfo& funcInfo)
{
    llvm::ReversePostOrderTraversal<llvm::Function*> rpot(&f);
    udm::Interval headers;
    udm::Interval interval;
    std::vector<udm::Interval> intervalsVec;

    for(auto& bb : rpot)
    {
        // the first basic block in a function is always a header
        if(headers.isEmpty())
        {
            headers.addBlock(bb);
        }
        interval.addBlock(bb);

        auto predecessors = utils::UdmUtils::getPredecessors(bb);
        logger->info("Size of predecessors: <{}>", predecessors.size());
        if(!std::any_of(predecessors.begin(), predecessors.end(), [&interval](auto& bbName) {
            return interval.containsBlock(bbName);
        }))
        {
           interval.addBlock(bb);
           continue; 
        }

        headers.addBlock(bb);
        intervalsVec.emplace_back(interval);
        interval.clear();
    }
    intervalsVec.emplace_back(interval);
    
    //add info about headers in function information
    setHeadersOfIntervals(headers, funcInfo);

    // print intervals
    std::size_t idx = 0;
    std::for_each(intervalsVec.begin(), intervalsVec.end(), [&idx, this](const udm::Interval& i) {
        logger->info("Interval number <{}>", ++idx);
        std::for_each(i.cbegin(), i.cend(), [this](const llvm::BasicBlock* bb) {
            logger->info("BB: <{}>", bb->getName().str());
        });
    });

    return intervalsVec;
}

std::pair<std::string, std::string> udm::IntervalGraph::backEdgeToPreviousInterval(udm::Interval interval)
{
    for(auto& bb : interval)
    {
        auto predecessors = utils::UdmUtils::getPredecessors(bb);
        for(auto& pred : predecessors)
        {
            if(isLowerOrEqBB(pred, bb->getName().str()))
            {
                return std::make_pair(bb->getName().str(), pred);
            }
        }
    }
    return std::make_pair("", "");
}

bool udm::IntervalGraph::isLowerOrEqBB(std::string firstBB, std::string secondBB)
{
    bool foundFirst = false;
    for(auto& interval : intervals)
    {
        for(auto& bb : interval)
        {            
            if(bb->getName().str() == secondBB)
            {
                if(!foundFirst)
                {
                    return true;
                }
            }
            
            if(bb->getName().str() == firstBB)
            {
                foundFirst = true;
            }
        }
    }
    return false;
}

void udm::IntervalGraph::setBlocksInLoop(const std::vector<std::string>& blocks, udm::FuncInfo& funcInfo)
{
    for(auto& bbName : blocks)
    {
        if(funcInfo.exists(bbName))
        {
            funcInfo[bbName].setIsLoop(true);
        }
    }
}

void udm::IntervalGraph::setBlockLoopType(const std::pair<std::string, std::string>& backEdge, udm::FuncInfo& funcInfo)
{
    if(funcInfo.exists(backEdge.first))
    {
        auto type = getLoopType(backEdge);
        auto backEdgeFirst = backEdge.first;
        logger->info("I am setting: <{}> with value: <{}>", backEdgeFirst, udm::BBInfo::getLoopTypeString(static_cast<size_t>(type)));
        auto bbBlock = getBB(backEdgeFirst);
        auto predecessors = utils::UdmUtils::getPredecessors(bbBlock);

        // !!! neededPred fix for setting the preheader to start the loop, instead of the first
        // !!! basic block in the loop
        std::string neededPred = "";
        bool isPointingToItself = false;
        for(auto& pred : predecessors)
        {
            if(pred == backEdgeFirst)
            {
                isPointingToItself = true;
                continue;
            }
            else if(isPointingToItself) {
                neededPred = pred;
                break;
            }
        }

        if (!neededPred.empty())
        {
            if(funcInfo.exists(neededPred))
            {
                funcInfo[neededPred].setLoopType(type);
                funcInfo[neededPred].setIsLoop(true);
                funcInfo[neededPred].setHeadToLatch(std::make_pair(neededPred, backEdge.second));
            }
        }
        else
        {
            funcInfo[backEdge.first].setLoopType(type);
            funcInfo[backEdge.first].setIsLoop(true);
            funcInfo[backEdge.first].setHeadToLatch(backEdge);
        }
    }
}

void udm::IntervalGraph::setFollowBlock(const std::pair<std::string, std::string>& backEdge, udm::FuncInfo& funcInfo)
{
    if(!funcInfo.exists(backEdge.first))
    {
        return;
    }

    auto follow = getFollowBlock(backEdge);
    logger->info("[IntervalGraph::setFollowBlock] Follow node for backedge: <{}> -> <{}> where follow node is: <{}>", backEdge.first, backEdge.second, follow);
    funcInfo[backEdge.first].setFollowNode(follow);

    // if follow node has exactly 2 predecessors & they are unconditional branches
    //  then set the conditional type as ConditionalType::ELSE
    auto bbFollow = getBB(follow);
    if(!bbFollow) {
        return;
    }

    auto pred = utils::UdmUtils::getPredecessors(bbFollow);
    bool condition = isConditionalSimpleELSE(pred);
    if(condition)
    {
        logger->info("[IntervalGraph::setFollowBlock] Setting conditional type as ELSE for: <{}>", backEdge.first);
        funcInfo[backEdge.first].setConditionalType(udm::BBInfo::ConditionalType::ELSE);
    }

}

bool udm::IntervalGraph::isConditionalSimpleELSE(std::vector<std::string> &pred) {
    if(pred.size() != 2)
    {
        logger->info("[IntervalGraph::isConditionalSimpleELSE] Size of predecessors not 2: <{}>", pred.size());
        return false;
    }

    auto condition = std::all_of(pred.begin(), pred.end(), [this](std::string& predName) {
        auto bbPred = getBB(predName);
        if(!bbPred) {
            return false;
        }
        const auto terminator = bbPred->getTerminator();
        if(!terminator)
        {
            return false;
        }
        if( auto* branchInst = llvm::dyn_cast<llvm::BranchInst>(terminator) )
        {
            logger->info("[IntervalGraph::isConditionalSimpleELSE] Branch instruction is unconditional: <{}> for pred: {}", branchInst->isUnconditional(), predName);
            return branchInst->isUnconditional();
        }
        return false;
    });
    return condition;
}

void udm::IntervalGraph::loopStructure(udm::FuncInfo& funcInfo)
{
    for(auto& interval : intervals)
    { 
        auto backEdge = backEdgeToPreviousInterval(interval);
        logger->critical(
            "Backedge: <{}> -> <{}>",
            backEdge.first,
            backEdge.second );

        if(backEdge.first.empty())
        {
            continue;
        }

        auto blocks = getBlocksBetweenLatchAndHeader(backEdge);
        setBlocksInLoop(blocks, funcInfo);
        setBlockLoopType(backEdge, funcInfo);
        setFollowBlock(backEdge, funcInfo);          
    }
}

bool udm::IntervalGraph::isBBlockbeforeInterval(std::string& bbName, udm::Interval interval)
{
    for(const auto& intv: intervals)
    {
        if(intv == interval)
        {
            break;
        }

        if(intv.containsBlock(bbName))
        {
            return true;
        }
    }
    return false;
}

std::vector<std::string> udm::IntervalGraph::getBlocksBetweenLatchAndHeader(std::pair<std::string, std::string> backEdge)
{
    bool startAdd = false;
    std::string stop = backEdge.second;
    std::vector<std::string> nodesBetweenLatchAndHeader;

    if(backEdge.first == backEdge.second)
    {
        nodesBetweenLatchAndHeader.push_back(backEdge.first);
        return nodesBetweenLatchAndHeader;
    }

    for(auto& interval : intervals)
    {
        for(auto& bb : interval)
        {
            const auto& bbName = bb->getName().str();
            logger->info("getBlocksBetweenLatchAndHeader: <{}>", bbName);
            if(bbName == backEdge.first)
            {
                startAdd = true;
            }

            if(startAdd)
            {
                nodesBetweenLatchAndHeader.push_back(bb->getName().str());
            }

            if(bbName == stop)
            {
                return nodesBetweenLatchAndHeader;
            }
        }
    }
    return nodesBetweenLatchAndHeader;
}

size_t udm::IntervalGraph::getNumSuccessors(std::string bbName)
{
    size_t count = 0;
    for(auto& interval : intervals)
    {
        auto block = interval.getBlock(bbName);
        if(block != nullptr)
        {
            return utils::UdmUtils::getSuccessors(block).size();
        }
    }
    return count;
}

size_t udm::IntervalGraph::getNumPredecessors(std::string bbName)
{
    auto block = getBB(bbName);
    if(block != nullptr)
    {
        return utils::UdmUtils::getPredecessors(block).size();
    }
    return -1;
}

llvm::BasicBlock* udm::IntervalGraph::getBB(std::string bbName)
{
    for(auto& interval : intervals)
    {
        auto block = interval.getBlock(bbName);
        if(block != nullptr)
        {
            return block;
        }
    }
    return nullptr;
}

udm::BBInfo::LoopType udm::IntervalGraph::getLoopType(std::pair<std::string, std::string> backEdge)
{
    if(backEdge.first.empty())
    {
        return udm::BBInfo::LoopType::NONE;
    }

    const std::string preheader = "preheader";
    if ( backEdge.first.substr(backEdge.first.length() - preheader.length()) == preheader
        || backEdge.second.substr(backEdge.second.length() - preheader.length()) == preheader
        || ( backEdge.first == backEdge.second &&
            ( backEdge.first.find("ph") != std::string::npos || backEdge.second.find("ph") != std::string::npos )
        )

    )
    {
        return udm::BBInfo::LoopType::WHILE;
    }

    auto nBetweenLatchAndHeader = getBlocksBetweenLatchAndHeader(backEdge);
    size_t headerSuccessorsNum = getNumSuccessors(backEdge.first); 
    size_t latchSuccessorsNum = getNumSuccessors(backEdge.second);

    if(backEdge.first == backEdge.second)
    {
        std::vector<std::string> preds = utils::UdmUtils::getPredecessors(getBB(backEdge.first));
        logger->critical("[getLoopType] Node header: <{}>, preds size: <{}>", preds.front(), preds.size());
        for(auto& pred : preds)
        {
            logger->critical("[getLoopType] Node pred: <{}>", pred);
            if(pred.find(preheader) != std::string::npos || pred.find("ph") != std::string::npos)
            {
                return udm::BBInfo::LoopType::WHILE;
            }
        }
    }

    logger->critical("[getLoopType] Node header: <{}>", headerSuccessorsNum);
    logger->critical("[getLoopType] Node latch: <{}>", latchSuccessorsNum);

    if(headerSuccessorsNum >= 2)
    {
        if(latchSuccessorsNum >= 2)
        {   
            auto bb = getBB(backEdge.second);
            if(!bb)
            {
                logger->critical("BB is null");
                return udm::BBInfo::LoopType::NONE;
            }

            auto predVector = utils::UdmUtils::getPredecessors(bb);
            auto firstPred = predVector.front();
            if(firstPred == "")
            {
                logger->critical("First pred is null");
                return udm::BBInfo::LoopType::NONE;
            }

            auto found = std::find(nBetweenLatchAndHeader.begin(), nBetweenLatchAndHeader.end(), firstPred);
            if(found != nBetweenLatchAndHeader.end())
            {
                return udm::BBInfo::LoopType::DO_WHILE;
            }
  
            return udm::BBInfo::LoopType::WHILE;
        }
        return udm::BBInfo::LoopType::DO_WHILE;
    }

    if(latchSuccessorsNum >= 2)
    {
        return udm::BBInfo::LoopType::WHILE;
    }

    return udm::BBInfo::LoopType::INFINITE;
}

std::string udm::IntervalGraph::getFollowBlock(const std::pair<std::string, std::string>& backEdge)
{   
    std::string header = backEdge.first;
    std::string latch = backEdge.second;

    if(header.empty())
    {
        return "";
    }

    auto nBetweenLatchAndHeader = getBlocksBetweenLatchAndHeader(backEdge);
    
    auto foundNode = [&nBetweenLatchAndHeader](const std::string& node)
    {
        auto found = std::find(nBetweenLatchAndHeader.begin(), nBetweenLatchAndHeader.end(), node);
        return found != nBetweenLatchAndHeader.end();
    };

    auto followNodeFirstSucc = [&](const std::vector<std::string>& successors)
    {
        bool foundFirstSucc = foundNode(successors.front()) && successors.front() != header;
        if(!foundFirstSucc)
        {
            return successors.back();
        }
        return successors.front();
    };

    auto latchSuccesors = utils::UdmUtils::getSuccessors(getBB(backEdge.second));
    auto headerSuccesors = utils::UdmUtils::getSuccessors(getBB(header));

    logger->critical("Node header follow: <{}>", latchSuccesors.front());
    logger->critical("Node latch follow: <{}>", headerSuccesors.front());

    auto loopType = getLoopType(backEdge);
    if(loopType == udm::BBInfo::LoopType::WHILE)
    {
        return followNodeFirstSucc(latchSuccesors);
    }
    else if(loopType == udm::BBInfo::LoopType::DO_WHILE)
    {
        return followNodeFirstSucc(headerSuccesors);
    }
    else if(loopType == udm::BBInfo::LoopType::INFINITE)
    {
        std::string follow = header;
        for(auto& n: nBetweenLatchAndHeader)
        {
            if(!foundNode(n))
            {
                continue;
            }

            auto followSuccInfinite = [&](const std::string& node)
            {
                auto found = foundNode(node);
                return found && follow != node && isLowerOrEqBB(node, follow);
            };

            if(followSuccInfinite(latchSuccesors.front()))
            {
                follow = latchSuccesors.front();
            }
            else if(followSuccInfinite(latchSuccesors.back()))
            {
                follow = latchSuccesors.back();
            }
        }
        
        if(follow != header)
        {
            return follow;
        }
    }
    return "";
}

llvm::BasicBlock* udm::IntervalGraph::findImediateDominator(llvm::BasicBlock* bb)
{
    auto node = dt.getNode(bb);
    return node && node->getIDom() ? node->getIDom()->getBlock() : nullptr;
}

void udm::IntervalGraph::twoWayConditionalBranch(udm::FuncInfo& funcInfo)
{
    std::vector<std::string> unresolved;
    for(auto intervIter = intervals.rbegin(); intervIter != intervals.rend(); ++intervIter)
    {   
        auto interval = *intervIter;
        for(auto bbIter = interval.rbegin(); bbIter != interval.rend(); ++ bbIter)
        {
            auto block = *bbIter;
            std::string bb = block->getName().str();
            logger->warn("BB: <{}>", bb);
            size_t bbOuterEdge = getNumSuccessors(bb);
            bool isHead = false;
            if(funcInfo.exists(bb))
            {
                isHead = funcInfo[bb].getIsHeader();
            }
            logger->warn("Outer edges: <{}>", bbOuterEdge);
            if( bbOuterEdge >= 2)
            {
                auto imedDom = findImediateDominator(block);
                logger->warn("is imedDom null?: <{}>", imedDom == nullptr);
                if(!imedDom)
                {
                    continue;
                }
                std::string imed = imedDom->getName().str();
                logger->warn("BB: <{}> imedDom: <{}>", bb, imed);
                logger->warn("Preds: <{}>", getNumPredecessors(imed));

                if(getNumPredecessors(imed) >= 2)
                {
                    // if all predecessprs are a unconditional branch
                    auto bbPred = utils::UdmUtils::getPredecessors(getBB(imed));
                    bool condition = isConditionalSimpleELSE(bbPred);
                    if(condition)
                    {
                        funcInfo[bb].setConditionalType(udm::BBInfo::ConditionalType::ELSE);
                    }
                    logger->info("Adding follow node: <{}>", imed);
                    funcInfo[bb].setFollowNode(imed);
                    for(const auto& notSolved: unresolved)
                    {
                        if(condition)
                        {
                            funcInfo[bb].setConditionalType(udm::BBInfo::ConditionalType::ELSE);
                        }
                        funcInfo[notSolved].setFollowNode(imed);
                    }
                }
                else
                {
                    unresolved.push_back(bb);
                }
            }
            
        }
    }
}