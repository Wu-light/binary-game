/**
 * 双子战略 - 策略节点类（学生实现版）
 * 
 * TODO: 学生需要实现二叉树的核心功能
 * 
 * 这个文件定义了策略节点的接口，但核心的二叉树功能需要学生自己实现。
 * 请查看 TODO 标记的部分，完成相应的实现。
 */

#ifndef STRATEGY_NODE_HPP
#define STRATEGY_NODE_HPP

#include "battle_unit.hpp"
#include <vector>

using namespace std;

/**
 * 策略节点类 - 二叉树节点
 * 
 * 学生需要实现以下核心功能：
 * 1. 二叉树的基本结构（parent, leftChild, rightChild）
 * 2. 添加子节点的方法
 * 3. 单位死亡时的动态重连逻辑
 * 4. Root加成的计算和应用
 * 
 * 提示：二叉树是一种特殊的树结构，每个节点最多有两个子节点。
 * 在这个游戏中，玩家和敌人的阵型都是用二叉树表示的。
 */

class StrategyNode {
private:
    // TODO: 定义二叉树的节点结构
    // 提示：需要 parent, leftChild, rightChild 三个指针
    
    StrategyNode* parent;       // 父节点指针
    StrategyNode* leftChild;    // 左子节点
    StrategyNode* rightChild;   // 右子节点
    BattleUnit* unit;           // 该节点上的战斗单位
    int nodeId;                 // 节点唯一ID
    int depth;                  // 在树中的深度
    bool isActive;              // 是否激活
    
public:
    // 构造函数 - 已实现
    StrategyNode(int id, BattleUnit* u = nullptr);
    
    // TODO: 实现添加子节点的方法
    /**
     * 添加左子节点
     * @param childUnit 子节点上的战斗单位
     * @param childId 子节点的ID
     * @return 创建的子节点指针
     * 
     * 提示：
     * 1. 检查是否已有左子节点（如果有需要先删除）
     * 2. 创建新的节点
     * 3. 设置父子关系
     * 4. 更新深度信息
     * 5. 如果子节点可以成为Root，设置其Root状态
     */
    StrategyNode* addLeftChild(BattleUnit* childUnit, int childId);
    
    /**
     * 添加右子节点
     * @param childUnit 子节点上的战斗单位
     * @param childId 子节点的ID
     * @return 创建的子节点指针
     * 
     * 提示：实现方式与 addLeftChild 类似
     */
    StrategyNode* addRightChild(BattleUnit* childUnit, int childId);
    
    // TODO: 实现单位死亡时的动态重连逻辑
    /**
     * 处理单位死亡
     * 
     * 提示：
     * 1. 标记节点为非活跃状态
     * 2. 将子节点重连到最近的存活祖先节点
     * 3. 避免树断裂，保持阵型完整性
     * 
     * 这是二叉树游戏的核心机制！
     */
    void handleUnitDeath();
    
    // TODO: 实现查找最近存活祖先的方法
    /**
     * 查找最近的存活祖先节点
     * @return 存活的祖先节点指针，如果没有返回 nullptr
     * 
     * 提示：从当前节点的父节点开始，向上遍历直到找到存活的节点
     */
    StrategyNode* findNearestAliveAncestor();
    
    // TODO: 实现深度更新方法
    /**
     * 更新节点及其子树的深度
     * @param node 要更新的节点
     * @param newDepth 新的深度值
     * 
     * 提示：这是一个递归方法，需要更新当前节点和所有子节点
     */
    void updateDepth(StrategyNode* node, int newDepth);
    
    // TODO: 实现Root加成应用方法
    /**
     * 应用Root加成到整个子树
     * 
     * 提示：
     * 1. 检查当前节点是否是Root
     * 2. 如果是，调用递归方法应用加成到所有后代节点
     */
    void applyRootBonusToSubtree();
    
    // TODO: 实现递归应用Root加成的方法
    /**
     * 递归应用Root加成
     * @param node 当前节点
     * @param rootDepth Root的深度
     * 
     * 提示：对当前节点应用加成，然后递归处理左子树和右子树
     */
    void applyBonusRecursive(StrategyNode* node, int rootDepth);
    
    // Getter方法 - 已实现
    StrategyNode* getParent() const { return parent; }
    StrategyNode* getLeftChild() const { return leftChild; }
    StrategyNode* getRightChild() const { return rightChild; }
    BattleUnit* getUnit() const { return unit; }
    int getNodeId() const { return nodeId; }
    int getDepth() const { return depth; }
    bool isNodeActive() const { return isActive; }
    
    // Setter方法 - 已实现
    void setUnit(BattleUnit* u) {
        unit = u;
        isActive = (u != nullptr);
        if (u) u->setDepth(depth);
    }
    
    // 析构函数 - TODO: 学生需要实现
    /**
     * 析构函数
     * 
     * 提示：需要删除左子树和右子树（递归删除）
     * 注意：unit由外部管理，这里不删除
     */
    ~StrategyNode();
};

#endif // STRATEGY_NODE_HPP

/*
 * ====================
 * 实现提示和示例代码
 * ====================
 * 
 * 下面是一些实现提示，可以帮助学生理解如何实现二叉树：
 * 
 * 1. 添加子节点的示例结构：
 * 
 * StrategyNode* StrategyNode::addLeftChild(BattleUnit* childUnit, int childId) {
 *     // 1. 如果已有左子节点，先删除
 *     if (leftChild) {
 *         delete leftChild;  // 注意：这会删除整个左子树！
 *     }
 *     
 *     // 2. 创建新节点
 *     leftChild = new StrategyNode(childId, childUnit);
 *     
 *     // 3. 设置父子关系
 *     leftChild->parent = this;  // 子节点的父指针指向当前节点
 *     
 *     // 4. 更新深度
 *     leftChild->depth = depth + 1;  // 子节点深度 = 父节点深度 + 1
 *     
 *     // 5. 设置Root状态
 *     if (childUnit && childUnit->canBeRoot()) {
 *         childUnit->setRoot(true);
 *         childUnit->setDepth(leftChild->depth);
 *     }
 *     
 *     return leftChild;
 * }
 * 
 * 2. 查找最近存活祖先的示例：
 * 
 * StrategyNode* StrategyNode::findNearestAliveAncestor() {
 *     StrategyNode* current = parent;  // 从父节点开始
 *     
 *     while (current != nullptr) {
 *         // 检查当前节点是否存活
 *         if (current->isActive && current->unit && current->unit->isUnitAlive()) {
 *             return current;  // 找到了！
 *         }
 *         current = current->parent;  // 继续向上查找
 *     }
 *     
 *     return nullptr;  // 没找到存活的祖先
 * }
 * 
 * 3. 递归更新深度的示例：
 * 
 * void StrategyNode::updateDepth(StrategyNode* node, int newDepth) {
 *     if (!node) return;  // 空节点直接返回
 *     
 *     // 更新当前节点
 *     node->depth = newDepth;
 *     if (node->unit) {
 *         node->unit->setDepth(newDepth);
 *     }
 *     
 *     // 递归更新左子树
 *     if (node->leftChild) {
 *         updateDepth(node->leftChild, newDepth + 1);
 *     }
 *     
 *     // 递归更新右子树
 *     if (node->rightChild) {
 *         updateDepth(node->rightChild, newDepth + 1);
 *     }
 * }
 * 
 * ====================
 * 学习目标
 * ====================
 * 
 * 通过实现这个项目，学生将学习：
 * 1. 二叉树的基本概念和结构
 * 2. 链式存储结构的实现
 * 3. 递归算法的应用
 * 4. 动态内存管理
 * 5. 数据结构在实际游戏中的应用
 * 6. 面向对象编程实践
 * 
 * ====================
 * 测试建议
 * ====================
 * 
 * 实现完成后，建议进行以下测试：
 * 
 * 1. 基本功能测试：
 *    - 创建一个Root节点
 *    - 添加左子节点和右子节点
 *    - 检查父子关系是否正确
 *    - 检查深度计算是否正确
 * 
 * 2. 动态重连测试：
 *    - 创建一个3层的二叉树
 *    - 让中间节点"死亡"
 *    - 检查子节点是否正确重连到祖先节点
 *    - 检查深度是否正确更新
 * 
 * 3. Root加成测试：
 *    - 创建一个Root节点
 *    - 添加多个子节点
 *    - 应用Root加成
 *    - 检查每个节点的攻击力和防御力是否正确增加
 * 
 * ====================
 * 常见错误
 * ====================
 * 
 * 1. 内存泄漏：忘记删除旧的子节点
 * 2. 父指针错误：忘记设置 parent 指针
 * 3. 深度计算错误：没有正确更新子树的深度
 * 4. 递归终止错误：忘记检查 nullptr
 * 5. Root状态错误：忘记更新 unit 的 Root 状态
 */