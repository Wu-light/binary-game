---
title: "综合实践项目：双子战略 - 二叉树策略战棋游戏"
---

# 综合实践项目：双子战略 - 二叉树策略战棋游戏

> 创意综合应用项目 —— 利用二叉树数据结构的知识，实现一个基于树形阵型的策略对战游戏。

接下来我们希望大家利用二叉树数据结构的知识，来实现一个基于树形阵型的策略对战游戏。

游戏核心玩法围绕<strong>「二叉树阵型、Root机制、动态重连」</strong>展开，玩家需要理解二叉树在游戏中的应用，实现树的核心功能，通过保护Root、管理阵型、击败敌人来获得胜利。

## 项目简介

### 游戏核心规则

#### 1. 单位属性

每个战斗单位包含以下核心属性：

- <strong>类型：</strong>骑士、弓箭手、坦克、治疗者、刺客、法师（6 种）
- <strong>生命值：</strong>不同类型有不同的最大生命值
- <strong>攻击力：</strong>基础攻击力，可受Root加成提升
- <strong>防御力：</strong>基础防御力，可受Root加成提升
- <strong>攻击范围：</strong>1-3（近战或远程）
- <strong>移动速度：</strong>1-3（影响行动顺序）
- <strong>Root能力：</strong>骑士和坦克可以成为Root单位

#### 2. 阵型结构

- <strong>二叉树阵型：</strong>每个单位最多有两个子兵，形成树状结构
- <strong>Root节点：</strong>阵型的根节点，可成为Root的单位（骑士、坦克）
- <strong>父子关系：</strong>每个子兵有一个父节点，形成完整的树结构
- <strong>深度概念：</strong>Root深度为0，子兵深度为1，孙兵深度为2，以此类推

<strong>示例：</strong>
```
        Root (主帅) [深度=0]
       /              \
   骑士 [深度=1]    弓箭手 [深度=1]
   /    \              \
治疗者  刺客         法师 [深度=2]
```

#### 3. Root机制

Root单位是阵型的核心，具有特殊作用：

- <strong>加成效果：</strong>Root存活时，整棵子树获得攻击力和防御力加成
- <strong>加成计算：</strong>Root单位获得30%加成，子单位加成随深度衰减（公式：max(5, 30 - distanceFromRoot × 5)）
- <strong>失效机制：</strong>Root被消灭时，其子树所有单位立即失效，失去加成效果
- <strong>策略意义：</strong>保护Root是最高优先级策略

#### 4. 动态重连规则

当某单位死亡时，系统自动将其子节点挂载到最近存活的祖先节点下：

- <strong>查找祖先：</strong>从父节点开始向上遍历，找到第一个存活的祖先
- <strong>挂载逻辑：</strong>优先挂载到祖先的左子节点位置，若已满则挂载到右子节点
- <strong>深度更新：</strong>重连后需要递归更新整个子树的深度
- <strong>保持完整：</strong>避免树断裂，保持阵型完整性

<strong>示例：</strong>Root下的左子兵死亡，其两个孙兵会自动重连到Root或其他存活祖先节点。

#### 5. 游戏流程

- <strong>初始化：</strong>玩家获得初始阵型（4人：1主帅+3士兵）
- <strong>战斗：</strong>单位自动攻击，按速度排序行动
- <strong>死亡处理：</strong>单位死亡时触发动态重连
- <strong>Roguelike奖励：</strong>每波胜利后三选一奖励（新单位或永久Buff）
- <strong>下一波次：</strong>敌人强度随波次递增
- <strong>结束：</strong>主帅阵亡（主帅模式）或全军覆没（全军模式）时游戏结束

### 得分规则

| 得分项目 | 分值 | 说明 |
| --- | --- | --- |
| 波次基础分 | 波次数 × 100 | 每成功通过一个波次获得 |
| 存活单位奖励 | 每单位 50 分 | 鼓励保护所有单位 |
| Root存活奖励 | 200 分（每波次） | Root单位在波次结束时存活 |
| 快速击败奖励 | 100 分 | 该波次回合数少于10回合 |

游戏结束时，总分越高表现越好。

## 设计要点

核心算法部分应使用 C++ 语言完成，用户界面的形式不加限制。支持发挥想象设计更多的附加功能和规则。

### 1. 二叉树节点结构设计

<strong>场景：</strong>设计并实现二叉树的节点类 `StrategyNode`，包含以下核心要素：

- <strong>节点属性：</strong>parent指针、leftChild指针、rightChild指针、unit单位数据、depth深度、nodeId标识
- <strong>父子关系：</strong>正确维护双向链接，子节点指向父节点，父节点指向子节点
- <strong>动态管理：</strong>添加子节点、删除子节点、查找祖先等操作

<strong>核心实现要求：</strong>

```cpp
class StrategyNode {
private:
    StrategyNode* parent;       // 父节点指针
    StrategyNode* leftChild;    // 左子节点
    StrategyNode* rightChild;   // 右子节点
    BattleUnit* unit;           // 战斗单位
    int nodeId;                 // 节点ID
    int depth;                  // 深度
    bool isActive;              // 是否激活
    
public:
    // 学生需要实现以下方法：
    StrategyNode(int id, BattleUnit* u);  // 构造函数
    StrategyNode* addLeftChild(BattleUnit* unit, int id);   // 添加左子节点
    StrategyNode* addRightChild(BattleUnit* unit, int id);  // 添加右子节点
    StrategyNode* findNearestAliveAncestor();               // 查找最近存活祖先
    void updateDepth(StrategyNode* node, int newDepth);     // 更新子树深度
    void handleUnitDeath();                                 // 处理单位死亡
    void applyRootBonusToSubtree();                        // 应用Root加成
    ~StrategyNode();                                        // 析构函数
};
```

### 2. 添加子节点逻辑

<strong>场景：</strong>当获得新单位时，需要将其添加到阵型树中。

<strong>实现要点：</strong>

1. 检查目标位置是否已有子节点，若有则需要删除（避免内存泄漏）
2. 创建新节点并设置父子关系（parent指针指向当前节点）
3. 计算新节点深度（父节点深度 + 1）
4. 如果单位可以成为Root，设置Root状态和深度信息

<strong>可选附加功能：</strong>支持智能选择最佳父节点添加新单位（如选择深度最小的节点）。

### 3. 动态重连机制

<strong>场景：</strong>单位死亡后，需要自动将子节点重连到存活的祖先节点。

<strong>实现要点：</strong>

1. 查找最近的存活祖先节点（从parent开始向上遍历）
2. 尝试挂载子节点：优先左子节点位置，若已满则右子节点
3. 递归更新整个子树的深度（重连后深度改变）
4. 重新计算并应用Root加成（深度变化影响加成数值）

<strong>这是二叉树游戏的核心机制！正确实现动态重连是本项目的关键。 </strong>

### 4. Root加成计算

<strong>场景：</strong>Root单位存活时，需要为整棵子树提供攻击力和防御力加成。

<strong>实现要点：</strong>

1. 检查当前节点是否是Root单位
2. 递归遍历整个子树，为每个后代节点应用加成
3. 计算加成数值：Root获得30%，后代随深度衰减（公式：max(5, 30 - distance × 5)）
4. 通过Buff系统应用到单位的攻击力和防御力属性

<strong>示例：</strong>
- Root（深度0）：30% 加成
- 子兵（深度1）：25% 加成
- 孙兵（深度2）：20% 加成

### 5. 深度更新算法

<strong>场景：</strong>树结构变化时（添加节点、重连节点），需要更新相关节点的深度。

<strong>实现要点：</strong>

1. 使用递归算法更新当前节点及其所有后代节点的深度
2. 设置终止条件：空节点直接返回
3. 先更新当前节点，再递归处理左子树和右子树
4. 同步更新单位对象的深度属性

<strong>递归设计提示：</strong>
```cpp
void updateDepth(StrategyNode* node, int newDepth) {
    if (!node) return;  // 终止条件
    node->depth = newDepth;
    if (node->unit) node->unit->setDepth(newDepth);
    updateDepth(node->leftChild, newDepth + 1);   // 递归左子树
    updateDepth(node->rightChild, newDepth + 1);  // 递归右子树
}
```

## 项目要求

1. <strong>数据结构设计：</strong>合理设计二叉树节点结构，正确维护父子关系，避免内存泄漏。
2. <strong>核心功能实现：</strong>实现添加子节点、查找祖先、动态重连、Root加成计算、深度更新等功能。
3. <strong>游戏集成：</strong>将实现的二叉树功能集成到提供的游戏框架中，确保游戏正常运行。
4. <strong>用户交互：</strong>利用提供的UI系统展示游戏状态，支持自动战斗和手动控制。
5. <strong>代码质量：</strong>编写清晰、可维护的代码，添加适当的注释说明核心算法思路。

<strong>提示</strong>

本项目是对二叉树数据结构知识的综合运用。你需要深入理解二叉树的结构特点、父子关系维护、递归算法设计、动态内存管理等核心概念。通过实现真实的游戏应用，将理论知识转化为实践能力。

<strong>项目特色：</strong>
- 已提供完整游戏框架（战斗系统、UI、Roguelike奖励等）
- 学生只需实现核心的二叉树功能（`student_strategy_node.hpp`）
- 完成后可获得一个可玩的策略游戏
- 真实应用场景，深刻理解数据结构的价值

<strong>评分重点：</strong>
- 基础功能（60分）：构造函数、添加子节点、析构函数、基本Getter/Setter
- 进阶功能（30分）：动态重连逻辑、Root加成计算
- 优秀加分（10分）：代码优化、测试完整、注释清晰

<strong>常见错误提醒：</strong>
- 忘记删除旧子节点导致内存泄漏
- 忘记设置parent指针导致双向链接断裂
- 递归方法缺少终止条件导致无限递归
- 深度计算错误导致Root加成失效
- 检查nullptr避免程序崩溃

通过完成这个项目，你将深刻理解二叉树在实际应用中的作用，掌握树结构的动态维护方法，提升面向对象编程和算法设计能力。
