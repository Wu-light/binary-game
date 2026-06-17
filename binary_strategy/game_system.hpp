/**
 * 双子战略 - 游戏主系统
 */

#ifndef GAME_SYSTEM_HPP
#define GAME_SYSTEM_HPP

#include "strategy_node.hpp"
#include "battle_unit.hpp"
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>

using namespace std;

// 游戏阶段
enum class GamePhase {
    SETUP,           // 阵型设置阶段
    BATTLE,          // 战斗阶段
    ROGUELIKE_REWARD,// 肉鸽奖励选择阶段
    GAME_OVER        // 游戏结束
};

// 战斗结果
enum class BattleResult {
    WIN,             // 获胜（消灭所有敌军）
    LOSE,            // 失败（主帅阵亡）
    ONGOING          // 战斗进行中
};

// Roguelike奖励选项
struct RoguelikeReward {
    UnitType unitType;    // 新单位类型（如果是单位奖励）
    BuffType buffType;    // Buff类型（如果是Buff奖励）
    bool isUnitReward;    // 是否是单位奖励
    int bonusValue;       // 加成数值
    string description;   // 描述
    
    RoguelikeReward(UnitType type) 
        : unitType(type), isUnitReward(true), bonusValue(0) {
        description = "新单位：" + getUnitTypeName(type);
    }
    
    RoguelikeReward(BuffType type, int value) 
        : buffType(type), isUnitReward(false), bonusValue(value) {
        description = getBuffDescription(type, value);
    }
    
    static string getUnitTypeName(UnitType type) {
        switch (type) {
            case UnitType::KNIGHT: return "骑士🛡️";
            case UnitType::ARCHER: return "弓箭手🏹";
            case UnitType::TANK: return "坦克🪨";
            case UnitType::HEALER: return "治疗者💚";
            case UnitType::ASSASSIN: return "刺客🗡️";
            case UnitType::MAGE: return "法师🔮";
            default: return "未知";
        }
    }
    
    static string getBuffDescription(BuffType type, int value) {
        switch (type) {
            case BuffType::ATTACK_BOOST: 
                return "永久攻击+" + to_string(value) + "%⚔️";
            case BuffType::DEFENSE_BOOST: 
                return "永久防御+" + to_string(value) + "%🛡️";
            case BuffType::HEALTH_REGEN: 
                return "每回合恢复" + to_string(value) + "HP💚";
            case BuffType::SHIELD: 
                return "护盾+" + to_string(value) + "点🛡️";
            default: return "特殊加成";
        }
    }
};

// 游戏主类
class BinaryStrategyGame {
private:
    StrategyNode* playerRoot;      // 玩家阵型根节点
    StrategyNode* enemyRoot;       // 敌人阵型根节点
    vector<StrategyNode*> playerUnits;  // 玩家所有单位节点
    vector<StrategyNode*> enemyUnits;   // 敌人所有单位节点
    vector<RoguelikeReward> currentRewards; // 当前奖励选项
    GamePhase phase;               // 当前游戏阶段
    BattleResult result;           // 战斗结果
    int wave;                      // 当前波次
    int score;                     // 总得分
    int nodeIdCounter;             // 节点ID计数器
    bool isCommanderMode;          // 是否是主帅模式
    int turn;                      // 当前回合数
    
public:
    BinaryStrategyGame(bool commanderMode = true) 
        : playerRoot(nullptr), enemyRoot(nullptr),
          phase(GamePhase::SETUP), result(BattleResult::ONGOING),
          wave(1), score(0), nodeIdCounter(0), 
          isCommanderMode(commanderMode), turn(0) {}
    
    // 初始化游戏
    void initGame() {
        // 创建玩家初始阵型（4人开局）
        createPlayerFormation();
        
        // 创建敌人阵型
        createEnemyFormation();
        
        phase = GamePhase::SETUP;
        result = BattleResult::ONGOING;
    }
    
    // 创建玩家阵型
    void createPlayerFormation() {
        // 清理旧阵型
        if (playerRoot) delete playerRoot;
        playerUnits.clear();
        
        // 创建主帅（Root）
        BattleUnit* commander = new BattleUnit(UnitType::KNIGHT, true);
        playerRoot = new StrategyNode(nodeIdCounter++, commander);
        playerUnits.push_back(playerRoot);
        
        // 添加两个子兵
        BattleUnit* leftUnit = new BattleUnit(UnitType::ARCHER);
        StrategyNode* leftNode = playerRoot->addLeftChild(leftUnit, nodeIdCounter++);
        playerUnits.push_back(leftNode);
        
        BattleUnit* rightUnit = new BattleUnit(UnitType::HEALER);
        StrategyNode* rightNode = playerRoot->addRightChild(rightUnit, nodeIdCounter++);
        playerUnits.push_back(rightNode);
        
        // 应用Root加成
        playerRoot->applyRootBonusToSubtree();
    }
    
    // 创建敌人阵型（基于波次）
    void createEnemyFormation() {
        if (enemyRoot) delete enemyRoot;
        enemyUnits.clear();
        
        // 根据波次调整敌人强度
        int enemyCount = wave + 2; // 波次1: 3个敌人，波次2: 4个...
        
        // 创建敌人Root
        UnitType enemyRootType = getRandomUnitType(true);
        BattleUnit* enemyCommander = new BattleUnit(enemyRootType);
        enemyRoot = new StrategyNode(nodeIdCounter++, enemyCommander);
        enemyUnits.push_back(enemyRoot);
        
        // 添加敌人子兵
        for (int i = 0; i < enemyCount - 1; ++i) {
            BattleUnit* enemyUnit = new BattleUnit(getRandomUnitType());
            StrategyNode* enemyNode = new StrategyNode(nodeIdCounter++, enemyUnit);
            enemyUnits.push_back(enemyNode);
            
            // 随机挂载到现有节点
            StrategyNode* parent = enemyUnits[rand() % (enemyUnits.size() - 1)];
            if (!parent->getLeftChild()) {
                parent->addLeftChild(enemyUnit, nodeIdCounter++);
            } else if (!parent->getRightChild()) {
                parent->addRightChild(enemyUnit, nodeIdCounter++);
            }
        }
        
        enemyRoot->applyRootBonusToSubtree();
    }
    
    // 战斗回合
    void battleTurn() {
        if (phase != GamePhase::BATTLE) return;
        
        turn++;
        
        // 玩家单位行动
        for (auto* node : playerUnits) {
            if (node->isNodeActive() && node->getUnit()) {
                performUnitAction(node, true);
            }
        }
        
        // 敌人单位行动
        for (auto* node : enemyUnits) {
            if (node->isNodeActive() && node->getUnit()) {
                performUnitAction(node, false);
            }
        }
        
        // 处理死亡单位的重连
        handleDeaths();
        
        // 检查战斗结果
        checkBattleResult();
        
        // 更新Buff
        updateAllBuffs();
    }
    
    // 单位行动（攻击/治疗）
    void performUnitAction(StrategyNode* node, bool isPlayer) {
        BattleUnit* unit = node->getUnit();
        if (!unit || !unit->isUnitAlive()) return;
        
        // 寻找攻击目标
        vector<StrategyNode*>& targets = isPlayer ? enemyUnits : playerUnits;
        StrategyNode* target = findBestTarget(node, targets);
        
        if (target && target->getUnit()) {
            int damage = unit->getCurrentAttack();
            
            // 应用特殊技能
            applySpecialSkill(unit, target->getUnit(), damage);
            
            // 执行攻击
            target->getUnit()->takeDamage(damage);
            
            // 治疗者的特殊行为
            if (unit->getType() == UnitType::HEALER) {
                // 治疗队友
                StrategyNode* allyToHeal = findAllyToHeal(node, isPlayer);
                if (allyToHeal && allyToHeal->getUnit()) {
                    allyToHeal->getUnit()->heal(20);
                }
            }
        }
    }
    
    // 寻找最佳攻击目标
    StrategyNode* findBestTarget(StrategyNode* attacker, vector<StrategyNode*>& targets) {
        BattleUnit* attackUnit = attacker->getUnit();
        if (!attackUnit) return nullptr;
        
        StrategyNode* bestTarget = nullptr;
        int bestPriority = -1;
        
        for (auto* target : targets) {
            if (!target->isNodeActive() || !target->getUnit()) continue;
            if (!target->getUnit()->isUnitAlive()) continue;
            
            int priority = calculateTargetPriority(attacker, target);
            if (priority > bestPriority) {
                bestPriority = priority;
                bestTarget = target;
            }
        }
        
        return bestTarget;
    }
    
    // 计算目标优先级
    int calculateTargetPriority(StrategyNode* attacker, StrategyNode* target) {
        BattleUnit* attackUnit = attacker->getUnit();
        BattleUnit* targetUnit = target->getUnit();
        
        int priority = 0;
        
        // 主帅优先级最高
        if (targetUnit->isUnitCommander()) {
            priority += 100;
        }
        
        // Root单位次优先
        if (targetUnit->isUnitRoot()) {
            priority += 50;
        }
        
        // 刺客对Root额外优先
        if (attackUnit->getType() == UnitType::ASSASSIN && targetUnit->isUnitRoot()) {
            priority += 200;
        }
        
        // 低血量单位优先
        int healthPercent = targetUnit->getCurrentHealth() * 100 / targetUnit->getMaxHealth();
        priority += (100 - healthPercent);
        
        return priority;
    }
    
    // 应用特殊技能
    void applySpecialSkill(BattleUnit* attacker, BattleUnit* target, int& damage) {
        switch (attacker->getType()) {
            case UnitType::ASSASSIN:
                if (target->isUnitRoot()) {
                    damage *= 2; // 对Root伤害翻倍
                }
                break;
            case UnitType::ARCHER:
                damage = damage * 120 / 100; // 精准射击
                break;
            case UnitType::MAGE:
                // 法术连击：额外攻击2个目标
                // 这里简化处理，实际需要多目标逻辑
                break;
        }
    }
    
    // 寻找需要治疗的队友
    StrategyNode* findAllyToHeal(StrategyNode* healer, bool isPlayer) {
        vector<StrategyNode*>& allies = isPlayer ? playerUnits : enemyUnits;
        
        StrategyNode* mostDamaged = nullptr;
        int lowestHealthPercent = 100;
        
        for (auto* ally : allies) {
            if (!ally->isNodeActive() || !ally->getUnit()) continue;
            if (!ally->getUnit()->isUnitAlive() || ally == healer) continue;
            
            int healthPercent = ally->getUnit()->getCurrentHealth() * 100 / 
                               ally->getUnit()->getMaxHealth();
            if (healthPercent < lowestHealthPercent) {
                lowestHealthPercent = healthPercent;
                mostDamaged = ally;
            }
        }
        
        return mostDamaged;
    }
    
    // 处理死亡单位
    void handleDeaths() {
        // 检查玩家单位死亡
        for (auto* node : playerUnits) {
            if (node->isNodeActive() && node->getUnit() && !node->getUnit()->isUnitAlive()) {
                node->handleUnitDeath();
                // 重新计算Root加成
                if (playerRoot) {
                    playerRoot->applyRootBonusToSubtree();
                }
            }
        }
        
        // 检查敌人单位死亡
        for (auto* node : enemyUnits) {
            if (node->isNodeActive() && node->getUnit() && !node->getUnit()->isUnitAlive()) {
                node->handleUnitDeath();
                if (enemyRoot) {
                    enemyRoot->applyRootBonusToSubtree();
                }
            }
        }
    }
    
    // 检查战斗结果
    void checkBattleResult() {
        // 主帅模式：检查主帅存活
        if (isCommanderMode) {
            bool playerCommanderAlive = false;
            bool enemyCommanderAlive = false;
            
            for (auto* node : playerUnits) {
                if (node->getUnit() && node->getUnit()->isUnitCommander() && 
                    node->getUnit()->isUnitAlive()) {
                    playerCommanderAlive = true;
                    break;
                }
            }
            
            for (auto* node : enemyUnits) {
                if (node->getUnit() && node->getUnit()->isUnitCommander() && 
                    node->getUnit()->isUnitAlive()) {
                    enemyCommanderAlive = true;
                    break;
                }
            }
            
            if (!playerCommanderAlive) {
                result = BattleResult::LOSE;
                phase = GamePhase::GAME_OVER;
            } else if (!enemyCommanderAlive) {
                result = BattleResult::WIN;
                phase = GamePhase::ROGUELIKE_REWARD;
                generateRewards();
            }
        } else {
            // 非主帅模式：检查全军存活
            bool anyPlayerAlive = false;
            bool anyEnemyAlive = false;
            
            for (auto* node : playerUnits) {
                if (node->getUnit() && node->getUnit()->isUnitAlive()) {
                    anyPlayerAlive = true;
                    break;
                }
            }
            
            for (auto* node : enemyUnits) {
                if (node->getUnit() && node->getUnit()->isUnitAlive()) {
                    anyEnemyAlive = true;
                    break;
                }
            }
            
            if (!anyPlayerAlive) {
                result = BattleResult::LOSE;
                phase = GamePhase::GAME_OVER;
            } else if (!anyEnemyAlive) {
                result = BattleResult::WIN;
                phase = GamePhase::ROGUELIKE_REWARD;
                generateRewards();
            }
        }
    }
    
    // 更新所有Buff
    void updateAllBuffs() {
        for (auto* node : playerUnits) {
            if (node->getUnit()) {
                node->getUnit()->updateBuffs();
            }
        }
        
        for (auto* node : enemyUnits) {
            if (node->getUnit()) {
                node->getUnit()->updateBuffs();
            }
        }
    }
    
    // 生成Roguelike奖励选项
    void generateRewards() {
        currentRewards.clear();
        
        // 生成3个随机奖励选项
        for (int i = 0; i < 3; ++i) {
            bool isUnit = (rand() % 100) < 60; // 60%概率是新单位
            
            if (isUnit) {
                currentRewards.push_back(RoguelikeReward(getRandomUnitType()));
            } else {
                // Buff奖励
                int rewardType = rand() % 4;
                int bonusValue = 10 + (rand() % 20); // 10-30
                
                BuffType buffType;
                switch (rewardType) {
                    case 0: buffType = BuffType::ATTACK_BOOST; break;
                    case 1: buffType = BuffType::DEFENSE_BOOST; break;
                    case 2: buffType = BuffType::HEALTH_REGEN; break;
                    case 3: buffType = BuffType::SHIELD; break;
                }
                
                currentRewards.push_back(RoguelikeReward(buffType, bonusValue));
            }
        }
    }
    
    // 选择奖励
    void selectReward(int rewardIndex) {
        if (rewardIndex < 0 || rewardIndex >= currentRewards.size()) return;
        
        RoguelikeReward& reward = currentRewards[rewardIndex];
        
        if (reward.isUnitReward) {
            // 添加新单位到阵型
            addNewUnit(reward.unitType);
        } else {
            // 应用Buff到所有存活单位
            applyBuffToAllUnits(reward.buffType, reward.bonusValue);
        }
        
        // 进入下一波次
        wave++;
        phase = GamePhase::SETUP;
        score += calculateWaveScore();
        
        // 生成新的敌人阵型
        createEnemyFormation();
    }
    
    // 添加新单位到阵型
    void addNewUnit(UnitType type) {
        BattleUnit* newUnit = new BattleUnit(type);
        
        // 寻找合适的位置插入
        StrategyNode* bestParent = findBestParentForNewUnit();
        
        if (bestParent) {
            if (!bestParent->getLeftChild()) {
                bestParent->addLeftChild(newUnit, nodeIdCounter++);
            } else if (!bestParent->getRightChild()) {
                bestParent->addRightChild(newUnit, nodeIdCounter++);
            }
            
            StrategyNode* newNode = new StrategyNode(nodeIdCounter++, newUnit);
            playerUnits.push_back(newNode);
            
            // 更新Root加成
            if (playerRoot) {
                playerRoot->applyRootBonusToSubtree();
            }
        }
    }
    
    // 寻找最佳父节点用于新单位
    StrategyNode* findBestParentForNewUnit() {
        StrategyNode* bestParent = nullptr;
        int minDepth = 100;
        
        for (auto* node : playerUnits) {
            if (!node->isNodeActive() || !node->getUnit()) continue;
            
            // 优先选择有空间且深度最小的节点
            if (!node->getLeftChild() || !node->getRightChild()) {
                if (node->getDepth() < minDepth) {
                    minDepth = node->getDepth();
                    bestParent = node;
                }
            }
        }
        
        return bestParent;
    }
    
    // 应用Buff到所有单位
    void applyBuffToAllUnits(BuffType type, int value) {
        for (auto* node : playerUnits) {
            if (node->getUnit() && node->getUnit()->isUnitAlive()) {
                Buff buff(type, value, -1, true); // 永久Buff
                node->getUnit()->addBuff(buff);
            }
        }
    }
    
    // 计算波次得分
    int calculateWaveScore() {
        int waveScore = 0;
        
        // 基础波次分
        waveScore += wave * 100;
        
        // 存活单位奖励
        for (auto* node : playerUnits) {
            if (node->getUnit() && node->getUnit()->isUnitAlive()) {
                waveScore += 50;
            }
        }
        
        // Root存活奖励
        if (playerRoot && playerRoot->getUnit() && playerRoot->getUnit()->isUnitAlive()) {
            waveScore += 200;
        }
        
        // 快速击败奖励（回合数少）
        if (turn < 10) {
            waveScore += 100;
        }
        
        return waveScore;
    }
    
    // 随机单位类型
    UnitType getRandomUnitType(bool canBeRoot = false) {
        static vector<UnitType> allTypes = {
            UnitType::KNIGHT, UnitType::ARCHER, UnitType::TANK,
            UnitType::HEALER, UnitType::ASSASSIN, UnitType::MAGE
        };
        
        if (canBeRoot) {
            // Root只能是骑士或坦克
            return (rand() % 2 == 0) ? UnitType::KNIGHT : UnitType::TANK;
        }
        
        return allTypes[rand() % allTypes.size()];
    }
    
    // Setters
    void setPhase(GamePhase p) { phase = p; }
    
    // Getters
    GamePhase getPhase() const { return phase; }
    BattleResult getResult() const { return result; }
    int getWave() const { return wave; }
    int getScore() const { return score; }
    int getTurn() const { return turn; }
    StrategyNode* getPlayerRoot() const { return playerRoot; }
    StrategyNode* getEnemyRoot() const { return enemyRoot; }
    const vector<RoguelikeReward>& getRewards() const { return currentRewards; }
    const vector<StrategyNode*>& getPlayerUnits() const { return playerUnits; }
    const vector<StrategyNode*>& getEnemyUnits() const { return enemyUnits; }
    
    // 析构函数
    ~BinaryStrategyGame() {
        if (playerRoot) delete playerRoot;
        if (enemyRoot) delete enemyRoot;
    }
};

#endif // GAME_SYSTEM_HPP