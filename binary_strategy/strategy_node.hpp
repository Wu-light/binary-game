/**
 * 双子战略 - 策略节点类（扩展TreeNode用于战棋游戏）
 */

#ifndef STRATEGY_NODE_HPP
#define STRATEGY_NODE_HPP

#include "battle_unit.hpp"
#include <vector>
#include <memory>

using namespace std;

// 策略节点类 - 二叉树节点扩展版
class StrategyNode {
private:
    StrategyNode* parent;       // 父节点指针
    StrategyNode* leftChild;    // 左子节点
    StrategyNode* rightChild;   // 右子节点
    BattleUnit* unit;           // 该节点上的战斗单位
    int nodeId;                 // 节点唯一ID
    int depth;                  // 在树中的深度
    bool isActive;              // 是否激活（有单位且有有效连接）
    
public:
    StrategyNode(int id, BattleUnit* u = nullptr) 
        : parent(nullptr), leftChild(nullptr), rightChild(nullptr),
          unit(u), nodeId(id), depth(0), isActive(u != nullptr) {}
    
    // 添加左子节点
    StrategyNode* addLeftChild(BattleUnit* childUnit, int childId) {
        if (leftChild) {
            // 已有左子节点，先清理
            delete leftChild;
        }
        
        leftChild = new StrategyNode(childId, childUnit);
        leftChild->parent = this;
        leftChild->depth = depth + 1;
        
        if (childUnit && childUnit->canBeRoot()) {
            childUnit->setRoot(true);
            childUnit->setDepth(leftChild->depth);
        }
        
        return leftChild;
    }
    
    // 添加右子节点
    StrategyNode* addRightChild(BattleUnit* childUnit, int childId) {
        if (rightChild) {
            delete rightChild;
        }
        
        rightChild = new StrategyNode(childId, childUnit);
        rightChild->parent = this;
        rightChild->depth = depth + 1;
        
        if (childUnit && childUnit->canBeRoot()) {
            childUnit->setRoot(true);
            childUnit->setDepth(rightChild->depth);
        }
        
        return rightChild;
    }
    
    // 单位死亡时的动态重连逻辑
    void handleUnitDeath() {
        if (!unit) return;
        
        unit = nullptr;
        isActive = false;
        
        // 将子节点重连到最近的存活祖先节点
        reconnectChildren();
    }
    
    // 重连子节点到最近的存活祖先
    void reconnectChildren() {
        StrategyNode* newParent = findNearestAliveAncestor();
        
        if (leftChild && leftChild->isActive) {
            if (newParent) {
                // 尝试挂载到新父节点
                if (!newParent->leftChild) {
                    newParent->leftChild = leftChild;
                    leftChild->parent = newParent;
                    updateDepth(leftChild, newParent->depth + 1);
                } else if (!newParent->rightChild) {
                    newParent->rightChild = leftChild;
                    leftChild->parent = newParent;
                    updateDepth(leftChild, newParent->depth + 1);
                }
            }
            leftChild->handleRootStatus();
        }
        
        if (rightChild && rightChild->isActive) {
            if (newParent) {
                if (!newParent->leftChild) {
                    newParent->leftChild = rightChild;
                    rightChild->parent = newParent;
                    updateDepth(rightChild, newParent->depth + 1);
                } else if (!newParent->rightChild) {
                    newParent->rightChild = rightChild;
                    rightChild->parent = newParent;
                    updateDepth(rightChild, newParent->depth + 1);
                }
            }
            rightChild->handleRootStatus();
        }
        
        leftChild = nullptr;
        rightChild = nullptr;
    }
    
    // 查找最近的存活祖先节点
    StrategyNode* findNearestAliveAncestor() {
        StrategyNode* current = parent;
        while (current) {
            if (current->isActive && current->unit && current->unit->isUnitAlive()) {
                return current;
            }
            current = current->parent;
        }
        return nullptr;
    }
    
    // 更新节点深度
    void updateDepth(StrategyNode* node, int newDepth) {
        if (!node) return;
        node->depth = newDepth;
        
        if (node->unit) {
            node->unit->setDepth(newDepth);
        }
        
        // 递归更新所有子节点
        if (node->leftChild) {
            updateDepth(node->leftChild, newDepth + 1);
        }
        if (node->rightChild) {
            updateDepth(node->rightChild, newDepth + 1);
        }
    }
    
    // 处理Root状态
    void handleRootStatus() {
        if (!unit) return;
        
        // 检查是否应该成为Root
        bool shouldBeRoot = unit->canBeRoot() && 
                          (!parent || !parent->unit || !parent->unit->isUnitAlive());
        
        if (shouldBeRoot && !unit->isUnitRoot()) {
            unit->setRoot(true);
            unit->applyRootBonus(depth);
        } else if (!shouldBeRoot && unit->isUnitRoot()) {
            unit->removeRootBonus();
        }
    }
    
    // 应用Root加成到整个子树
    void applyRootBonusToSubtree() {
        if (!unit || !unit->isUnitRoot()) return;
        
        applyBonusRecursive(this, depth);
    }
    
    // 递归应用Root加成
    void applyBonusRecursive(StrategyNode* node, int rootDepth) {
        if (!node || !node->unit) return;
        
        node->unit->applyRootBonus(rootDepth);
        
        if (node->leftChild) {
            applyBonusRecursive(node->leftChild, rootDepth);
        }
        if (node->rightChild) {
            applyBonusRecursive(node->rightChild, rootDepth);
        }
    }
    
    // 获取所有存活的后代节点
    vector<StrategyNode*> getAllAliveDescendants() {
        vector<StrategyNode*> descendants;
        collectAliveDescendants(this, descendants);
        return descendants;
    }
    
    // 递归收集存活后代
    void collectAliveDescendants(StrategyNode* node, vector<StrategyNode*>& descendants) {
        if (!node) return;
        
        if (node->isActive && node->unit && node->unit->isUnitAlive()) {
            descendants.push_back(node);
        }
        
        collectAliveDescendants(node->leftChild, descendants);
        collectAliveDescendants(node->rightChild, descendants);
    }
    
    // 获取子树大小
    int getSubtreeSize() {
        int size = 0;
        countSubtreeSize(this, size);
        return size;
    }
    
    // 递归计算子树大小
    void countSubtreeSize(StrategyNode* node, int& size) {
        if (!node) return;
        if (node->isActive) size++;
        
        countSubtreeSize(node->leftChild, size);
        countSubtreeSize(node->rightChild, size);
    }
    
    // Getters
    StrategyNode* getParent() const { return parent; }
    StrategyNode* getLeftChild() const { return leftChild; }
    StrategyNode* getRightChild() const { return rightChild; }
    BattleUnit* getUnit() const { return unit; }
    int getNodeId() const { return nodeId; }
    int getDepth() const { return depth; }
    bool isNodeActive() const { return isActive; }
    
    // Setters
    void setUnit(BattleUnit* u) { 
        unit = u; 
        isActive = (u != nullptr);
        if (u) {
            u->setDepth(depth);
        }
    }
    
    // 析构函数
    ~StrategyNode() {
        if (leftChild) delete leftChild;
        if (rightChild) delete rightChild;
        // unit由外部管理，这里不删除
    }
};

#endif // STRATEGY_NODE_HPP