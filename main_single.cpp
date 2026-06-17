/**
 * 双子战略 - 单文件版本
 * 可直接在在线C++编译器运行
 * 访问：https://www.onlinegdb.com/online_c++_compiler
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

// ==================== 单位类型定义 ====================

enum class UnitType {
    KNIGHT,      // 骑士 - 近战，高血量
    ARCHER,      // 弓箭手 - 远程，中等血量
    TANK,        // 坦克 - 高防御，低攻击
    HEALER,      // 治疗者 - 辅助
    ASSASSIN,    // 刺客 - 高攻击，低血量
    MAGE         // 法师 - 远程魔法
};

struct UnitAttributes {
    int maxHealth;
    int attack;
    int defense;
    int range;
    int speed;
    bool canBeRoot;
    string specialSkill;
    
    static UnitAttributes createAttributes(UnitType type) {
        UnitAttributes attr;
        switch (type) {
            case UnitType::KNIGHT:
                attr.maxHealth = 150;
                attr.attack = 25;
                attr.defense = 20;
                attr.range = 1;
                attr.speed = 2;
                attr.canBeRoot = true;
                attr.specialSkill = "盾牌格挡:减少50%伤害";
                break;
            case UnitType::ARCHER:
                attr.maxHealth = 80;
                attr.attack = 30;
                attr.defense = 10;
                attr.range = 3;
                attr.speed = 1;
                attr.canBeRoot = false;
                attr.specialSkill = "精准射击:攻击力+20%";
                break;
            case UnitType::TANK:
                attr.maxHealth = 200;
                attr.attack = 15;
                attr.defense = 40;
                attr.range = 1;
                attr.speed = 1;
                attr.canBeRoot = true;
                attr.specialSkill = "坚守阵地:防御力+50%";
                break;
            case UnitType::HEALER:
                attr.maxHealth = 60;
                attr.attack = 10;
                attr.defense = 5;
                attr.range = 2;
                attr.speed = 1;
                attr.canBeRoot = false;
                attr.specialSkill = "治疗光环:每回合恢复队友20HP";
                break;
            case UnitType::ASSASSIN:
                attr.maxHealth = 50;
                attr.attack = 40;
                attr.defense = 5;
                attr.range = 1;
                attr.speed = 3;
                attr.canBeRoot = false;
                attr.specialSkill = "暗杀:对Root单位伤害+100%";
                break;
            case UnitType::MAGE:
                attr.maxHealth = 70;
                attr.attack = 35;
                attr.defense = 8;
                attr.range = 2;
                attr.speed = 1;
                attr.canBeRoot = false;
                attr.specialSkill = "法术连击:攻击3个目标";
                break;
        }
        return attr;
    }
};

enum class BuffType {
    ATTACK_BOOST,
    DEFENSE_BOOST,
    HEALTH_REGEN,
    SHIELD,
    ROOT_BONUS
};

struct Buff {
    BuffType type;
    int value;
    int duration;
    bool isPermanent;
    
    Buff(BuffType t, int v, int d, bool perm = false)
        : type(t), value(v), duration(d), isPermanent(perm) {}
};

// ==================== 战斗单位类 ====================

class BattleUnit {
private:
    UnitType type;
    UnitAttributes baseAttributes;
    int currentHealth;
    bool isAlive;
    bool isRoot;
    bool isCommander;
    int depth;
    vector<Buff> activeBuffs;
    
public:
    BattleUnit(UnitType t, bool commander = false) 
        : type(t), isAlive(true), isRoot(false), 
          isCommander(commander), depth(-1) {
        baseAttributes = UnitAttributes::createAttributes(t);
        currentHealth = baseAttributes.maxHealth;
    }
    
    void applyRootBonus(int rootDepth) {
        depth = rootDepth;
        if (isRoot) {
            Buff rootBuff(BuffType::ROOT_BONUS, 30, -1, true);
            activeBuffs.push_back(rootBuff);
        } else {
            int distanceFromRoot = rootDepth - depth;
            if (distanceFromRoot > 0) {
                int bonusPercent = max(5, 30 - distanceFromRoot * 5);
                Buff distanceBuff(BuffType::ROOT_BONUS, bonusPercent, -1, true);
                activeBuffs.push_back(distanceBuff);
            }
        }
    }
    
    void removeRootBonus() {
        activeBuffs.erase(
            remove_if(activeBuffs.begin(), activeBuffs.end(),
                [](const Buff& b) { return b.type == BuffType::ROOT_BONUS; }),
            activeBuffs.end()
        );
        isRoot = false;
        depth = -1;
    }
    
    int getCurrentAttack() const {
        int attack = baseAttributes.attack;
        for (const auto& buff : activeBuffs) {
            if (buff.type == BuffType::ATTACK_BOOST || buff.type == BuffType::ROOT_BONUS) {
                attack = attack * (100 + buff.value) / 100;
            }
        }
        return attack;
    }
    
    int getCurrentDefense() const {
        int defense = baseAttributes.defense;
        for (const auto& buff : activeBuffs) {
            if (buff.type == BuffType::DEFENSE_BOOST || buff.type == BuffType::ROOT_BONUS) {
                defense = defense * (100 + buff.value) / 100;
            }
        }
        return defense;
    }
    
    void takeDamage(int damage) {
        int actualDamage = max(1, damage - getCurrentDefense());
        
        if (type == UnitType::KNIGHT) {
            actualDamage = actualDamage / 2;
        }
        
        currentHealth -= actualDamage;
        
        if (currentHealth <= 0) {
            currentHealth = 0;
            isAlive = false;
        }
    }
    
    void heal(int amount) {
        if (!isAlive) return;
        currentHealth = min(currentHealth + amount, baseAttributes.maxHealth);
    }
    
    void addBuff(const Buff& buff) {
        activeBuffs.push_back(buff);
    }
    
    void updateBuffs() {
        for (auto& buff : activeBuffs) {
            if (!buff.isPermanent) {
                buff.duration--;
                if (buff.duration <= 0) {
                    activeBuffs.erase(
                        remove_if(activeBuffs.begin(), activeBuffs.end(),
                            [](const Buff& b) { return b.duration <= 0 && !b.isPermanent; }),
                        activeBuffs.end()
                    );
                    break;
                }
            }
        }
    }
    
    string getUnitIcon() const {
        if (!isAlive) return "X";
        if (isCommander) return "C";
        if (isRoot) return "R";
        
        switch (type) {
            case UnitType::KNIGHT: return "K";
            case UnitType::ARCHER: return "A";
            case UnitType::TANK: return "T";
            case UnitType::HEALER: return "H";
            case UnitType::ASSASSIN: return "S";
            case UnitType::MAGE: return "M";
            default: return "?";
        }
    }
    
    string getUnitName() const {
        string name;
        switch (type) {
            case UnitType::KNIGHT: name = "骑士"; break;
            case UnitType::ARCHER: name = "弓箭手"; break;
            case UnitType::TANK: name = "坦克"; break;
            case UnitType::HEALER: name = "治疗者"; break;
            case UnitType::ASSASSIN: name = "刺客"; break;
            case UnitType::MAGE: name = "法师"; break;
        }
        
        if (isCommander) name += "(主)";
        if (isRoot) name += "(R)";
        
        return name;
    }
    
    UnitType getType() const { return type; }
    int getCurrentHealth() const { return currentHealth; }
    int getMaxHealth() const { return baseAttributes.maxHealth; }
    bool isUnitAlive() const { return isAlive; }
    bool isUnitRoot() const { return isRoot; }
    bool isUnitCommander() const { return isCommander; }
    int getDepth() const { return depth; }
    int getRange() const { return baseAttributes.range; }
    int getSpeed() const { return baseAttributes.speed; }
    const UnitAttributes& getAttributes() const { return baseAttributes; }
    
    void setRoot(bool root) { isRoot = root; }
    void setDepth(int d) { depth = d; }
};

// ==================== 策略节点类 ====================

class StrategyNode {
private:
    StrategyNode* parent;
    StrategyNode* leftChild;
    StrategyNode* rightChild;
    BattleUnit* unit;
    int nodeId;
    int depth;
    bool isActive;
    
public:
    StrategyNode(int id, BattleUnit* u = nullptr) 
        : parent(nullptr), leftChild(nullptr), rightChild(nullptr),
          unit(u), nodeId(id), depth(0), isActive(u != nullptr) {}
    
    StrategyNode* addLeftChild(BattleUnit* childUnit, int childId) {
        if (leftChild) delete leftChild;
        
        leftChild = new StrategyNode(childId, childUnit);
        leftChild->parent = this;
        leftChild->depth = depth + 1;
        
        if (childUnit && childUnit->canBeRoot()) {
            childUnit->setRoot(true);
            childUnit->setDepth(leftChild->depth);
        }
        
        return leftChild;
    }
    
    StrategyNode* addRightChild(BattleUnit* childUnit, int childId) {
        if (rightChild) delete rightChild;
        
        rightChild = new StrategyNode(childId, childUnit);
        rightChild->parent = this;
        rightChild->depth = depth + 1;
        
        if (childUnit && childUnit->canBeRoot()) {
            childUnit->setRoot(true);
            childUnit->setDepth(rightChild->depth);
        }
        
        return rightChild;
    }
    
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
    
    void updateDepth(StrategyNode* node, int newDepth) {
        if (!node) return;
        node->depth = newDepth;
        
        if (node->unit) {
            node->unit->setDepth(newDepth);
        }
        
        if (node->leftChild) updateDepth(node->leftChild, newDepth + 1);
        if (node->rightChild) updateDepth(node->rightChild, newDepth + 1);
    }
    
    void applyBonusRecursive(StrategyNode* node, int rootDepth) {
        if (!node || !node->unit) return;
        
        node->unit->applyRootBonus(rootDepth);
        
        if (node->leftChild) applyBonusRecursive(node->leftChild, rootDepth);
        if (node->rightChild) applyBonusRecursive(node->rightChild, rootDepth);
    }
    
    void applyRootBonusToSubtree() {
        if (!unit || !unit->isUnitRoot()) return;
        applyBonusRecursive(this, depth);
    }
    
    StrategyNode* getParent() const { return parent; }
    StrategyNode* getLeftChild() const { return leftChild; }
    StrategyNode* getRightChild() const { return rightChild; }
    BattleUnit* getUnit() const { return unit; }
    int getNodeId() const { return nodeId; }
    int getDepth() const { return depth; }
    bool isNodeActive() const { return isActive; }
    
    void setUnit(BattleUnit* u) { 
        unit = u; 
        isActive = (u != nullptr);
        if (u) u->setDepth(depth);
    }
    
    ~StrategyNode() {
        if (leftChild) delete leftChild;
        if (rightChild) delete rightChild;
    }
};

// ==================== 游戏系统 ====================

enum class GamePhase {
    SETUP,
    BATTLE,
    ROGUELIKE_REWARD,
    GAME_OVER
};

enum class BattleResult {
    WIN,
    LOSE,
    ONGOING
};

struct RoguelikeReward {
    UnitType unitType;
    BuffType buffType;
    bool isUnitReward;
    int bonusValue;
    string description;
    
    RoguelikeReward(UnitType type) 
        : unitType(type), isUnitReward(true), bonusValue(0) {
        description = "新单位: " + getUnitTypeName(type);
    }
    
    RoguelikeReward(BuffType type, int value) 
        : buffType(type), isUnitReward(false), bonusValue(value) {
        description = getBuffDescription(type, value);
    }
    
    static string getUnitTypeName(UnitType type) {
        switch (type) {
            case UnitType::KNIGHT: return "骑士[K]";
            case UnitType::ARCHER: return "弓箭手[A]";
            case UnitType::TANK: return "坦克[T]";
            case UnitType::HEALER: return "治疗者[H]";
            case UnitType::ASSASSIN: return "刺客[S]";
            case UnitType::MAGE: return "法师[M]";
            default: return "未知";
        }
    }
    
    static string getBuffDescription(BuffType type, int value) {
        switch (type) {
            case BuffType::ATTACK_BOOST: 
                return "永久攻击+" + to_string(value) + "%";
            case BuffType::DEFENSE_BOOST: 
                return "永久防御+" + to_string(value) + "%";
            case BuffType::HEALTH_REGEN: 
                return "每回合恢复" + to_string(value) + "HP";
            case BuffType::SHIELD: 
                return "护盾+" + to_string(value) + "点";
            default: return "特殊加成";
        }
    }
};

class BinaryStrategyGame {
private:
    StrategyNode* playerRoot;
    StrategyNode* enemyRoot;
    vector<StrategyNode*> playerUnits;
    vector<StrategyNode*> enemyUnits;
    vector<RoguelikeReward> currentRewards;
    GamePhase phase;
    BattleResult result;
    int wave;
    int score;
    int nodeIdCounter;
    bool isCommanderMode;
    int turn;
    vector<string> battleLog;
    
public:
    BinaryStrategyGame(bool commanderMode = true) 
        : playerRoot(nullptr), enemyRoot(nullptr),
          phase(GamePhase::SETUP), result(BattleResult::ONGOING),
          wave(1), score(0), nodeIdCounter(0), 
          isCommanderMode(commanderMode), turn(0) {}
    
    void initGame() {
        createPlayerFormation();
        createEnemyFormation();
        phase = GamePhase::SETUP;
        result = BattleResult::ONGOING;
    }
    
    void createPlayerFormation() {
        if (playerRoot) delete playerRoot;
        playerUnits.clear();
        
        BattleUnit* commander = new BattleUnit(UnitType::KNIGHT, true);
        commander->setRoot(true);
        playerRoot = new StrategyNode(nodeIdCounter++, commander);
        playerUnits.push_back(playerRoot);
        
        BattleUnit* leftUnit = new BattleUnit(UnitType::ARCHER);
        StrategyNode* leftNode = playerRoot->addLeftChild(leftUnit, nodeIdCounter++);
        playerUnits.push_back(leftNode);
        
        BattleUnit* rightUnit = new BattleUnit(UnitType::HEALER);
        StrategyNode* rightNode = playerRoot->addRightChild(rightUnit, nodeIdCounter++);
        playerUnits.push_back(rightNode);
        
        playerRoot->applyRootBonusToSubtree();
    }
    
    void createEnemyFormation() {
        if (enemyRoot) delete enemyRoot;
        enemyUnits.clear();
        
        int enemyCount = wave + 2;
        
        UnitType enemyRootType = getRandomUnitType(true);
        BattleUnit* enemyCommander = new BattleUnit(enemyRootType);
        enemyCommander->setRoot(true);
        enemyRoot = new StrategyNode(nodeIdCounter++, enemyCommander);
        enemyUnits.push_back(enemyRoot);
        
        int addedCount = 1;
        vector<StrategyNode*> availableParents = {enemyRoot};
        
        while (addedCount < enemyCount && !availableParents.empty()) {
            StrategyNode* parent = availableParents.front();
            availableParents.erase(availableParents.begin());
            
            if (addedCount < enemyCount) {
                BattleUnit* leftUnit = new BattleUnit(getRandomUnitType());
                StrategyNode* leftNode = parent->addLeftChild(leftUnit, nodeIdCounter++);
                enemyUnits.push_back(leftNode);
                availableParents.push_back(leftNode);
                addedCount++;
            }
            
            if (addedCount < enemyCount) {
                BattleUnit* rightUnit = new BattleUnit(getRandomUnitType());
                StrategyNode* rightNode = parent->addRightChild(rightUnit, nodeIdCounter++);
                enemyUnits.push_back(rightNode);
                availableParents.push_back(rightNode);
                addedCount++;
            }
        }
        
        enhanceEnemyUnits();
        enemyRoot->applyRootBonusToSubtree();
    }
    
    void enhanceEnemyUnits() {
        int bonusPercent = wave * 10;
        
        for (auto* node : enemyUnits) {
            if (node->getUnit() && node->getUnit()->isUnitAlive()) {
                Buff attackBuff(BuffType::ATTACK_BOOST, bonusPercent, -1, true);
                Buff defenseBuff(BuffType::DEFENSE_BOOST, bonusPercent / 2, -1, true);
                node->getUnit()->addBuff(attackBuff);
                node->getUnit()->addBuff(defenseBuff);
            }
        }
    }
    
    void battleTurn() {
        if (phase != GamePhase::BATTLE) return;
        
        turn++;
        battleLog.clear();
        
        vector<pair<StrategyNode*, bool>> actionOrder;
        
        for (auto* node : playerUnits) {
            if (node->isNodeActive() && node->getUnit() && node->getUnit()->isUnitAlive()) {
                actionOrder.push_back({node, true});
            }
        }
        
        for (auto* node : enemyUnits) {
            if (node->isNodeActive() && node->getUnit() && node->getUnit()->isUnitAlive()) {
                actionOrder.push_back({node, false});
            }
        }
        
        sort(actionOrder.begin(), actionOrder.end(), 
            [](const pair<StrategyNode*, bool>& a, const pair<StrategyNode*, bool>& b) {
                return a.first->getUnit()->getSpeed() > b.first->getUnit()->getSpeed();
            });
        
        for (auto& action : actionOrder) {
            if (action.first->getUnit() && action.first->getUnit()->isUnitAlive()) {
                performUnitAction(action.first, action.second);
            }
        }
        
        handleDeaths();
        checkBattleResult();
        updateAllBuffs();
    }
    
    void performUnitAction(StrategyNode* node, bool isPlayer) {
        BattleUnit* unit = node->getUnit();
        if (!unit || !unit->isUnitAlive()) return;
        
        string unitName = unit->getUnitName();
        
        if (unit->getType() == UnitType::HEALER) {
            StrategyNode* allyToHeal = findAllyToHeal(node, isPlayer);
            if (allyToHeal && allyToHeal->getUnit()) {
                int healAmount = 20 + wave * 2;
                allyToHeal->getUnit()->heal(healAmount);
                battleLog.push_back(unitName + " 治疗 " + allyToHeal->getUnit()->getUnitName() 
                                  + " +" + to_string(healAmount) + "HP");
            }
        }
        
        vector<StrategyNode*>& targets = isPlayer ? enemyUnits : playerUnits;
        StrategyNode* target = findBestTarget(node, targets);
        
        if (target && target->getUnit() && target->getUnit()->isUnitAlive()) {
            int damage = unit->getCurrentAttack();
            
            applySpecialSkill(unit, target->getUnit(), damage);
            
            string logMsg = unitName + " 攻击 " + target->getUnit()->getUnitName() 
                          + " (伤害:" + to_string(damage) + ")";
            
            int prevHealth = target->getUnit()->getCurrentHealth();
            target->getUnit()->takeDamage(damage);
            int actualDamage = prevHealth - target->getUnit()->getCurrentHealth();
            
            logMsg += " 实际:" + to_string(actualDamage);
            
            if (!target->getUnit()->isUnitAlive()) {
                logMsg += " [击杀!]";
            }
            
            battleLog.push_back(logMsg);
        }
    }
    
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
    
    int calculateTargetPriority(StrategyNode* attacker, StrategyNode* target) {
        BattleUnit* attackUnit = attacker->getUnit();
        BattleUnit* targetUnit = target->getUnit();
        
        int priority = 0;
        
        if (targetUnit->isUnitCommander()) priority += 100;
        if (targetUnit->isUnitRoot()) priority += 50;
        
        if (attackUnit->getType() == UnitType::ASSASSIN && targetUnit->isUnitRoot()) {
            priority += 200;
        }
        
        int healthPercent = targetUnit->getCurrentHealth() * 100 / targetUnit->getMaxHealth();
        priority += (100 - healthPercent);
        
        return priority;
    }
    
    void applySpecialSkill(BattleUnit* attacker, BattleUnit* target, int& damage) {
        switch (attacker->getType()) {
            case UnitType::ASSASSIN:
                if (target->isUnitRoot()) damage *= 2;
                break;
            case UnitType::ARCHER:
                damage = damage * 120 / 100;
                break;
        }
    }
    
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
    
    void handleDeaths() {
        for (auto* node : playerUnits) {
            if (node->isNodeActive() && node->getUnit() && !node->getUnit()->isUnitAlive()) {
                if (playerRoot) playerRoot->applyRootBonusToSubtree();
            }
        }
        
        for (auto* node : enemyUnits) {
            if (node->isNodeActive() && node->getUnit() && !node->getUnit()->isUnitAlive()) {
                if (enemyRoot) enemyRoot->applyRootBonusToSubtree();
            }
        }
    }
    
    void checkBattleResult() {
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
    
    void updateAllBuffs() {
        for (auto* node : playerUnits) {
            if (node->getUnit()) node->getUnit()->updateBuffs();
        }
        
        for (auto* node : enemyUnits) {
            if (node->getUnit()) node->getUnit()->updateBuffs();
        }
    }
    
    void generateRewards() {
        currentRewards.clear();
        
        for (int i = 0; i < 3; ++i) {
            bool isUnit = (rand() % 100) < 60;
            
            if (isUnit) {
                currentRewards.push_back(RoguelikeReward(getRandomUnitType()));
            } else {
                int rewardType = rand() % 4;
                int bonusValue = 10 + (rand() % 20);
                
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
    
    void selectReward(int rewardIndex) {
        if (rewardIndex < 0 || rewardIndex >= currentRewards.size()) return;
        
        RoguelikeReward& reward = currentRewards[rewardIndex];
        
        if (reward.isUnitReward) {
            addNewUnit(reward.unitType);
        } else {
            applyBuffToAllUnits(reward.buffType, reward.bonusValue);
        }
        
        wave++;
        phase = GamePhase::SETUP;
        score += calculateWaveScore();
        
        createEnemyFormation();
    }
    
    void addNewUnit(UnitType type) {
        BattleUnit* newUnit = new BattleUnit(type);
        
        StrategyNode* bestParent = nullptr;
        int minDepth = 100;
        
        for (auto* node : playerUnits) {
            if (!node->isNodeActive() || !node->getUnit()) continue;
            
            if (!node->getLeftChild() || !node->getRightChild()) {
                if (node->getDepth() < minDepth) {
                    minDepth = node->getDepth();
                    bestParent = node;
                }
            }
        }
        
        if (bestParent) {
            if (!bestParent->getLeftChild()) {
                bestParent->addLeftChild(newUnit, nodeIdCounter++);
            } else if (!bestParent->getRightChild()) {
                bestParent->addRightChild(newUnit, nodeIdCounter++);
            }
            
            StrategyNode* newNode = new StrategyNode(nodeIdCounter++, newUnit);
            playerUnits.push_back(newNode);
            
            if (playerRoot) playerRoot->applyRootBonusToSubtree();
        }
    }
    
    void applyBuffToAllUnits(BuffType type, int value) {
        for (auto* node : playerUnits) {
            if (node->getUnit() && node->getUnit()->isUnitAlive()) {
                Buff buff(type, value, -1, true);
                node->getUnit()->addBuff(buff);
            }
        }
    }
    
    int calculateWaveScore() {
        int waveScore = 0;
        
        waveScore += wave * 100;
        
        for (auto* node : playerUnits) {
            if (node->getUnit() && node->getUnit()->isUnitAlive()) {
                waveScore += 50;
            }
        }
        
        if (playerRoot && playerRoot->getUnit() && playerRoot->getUnit()->isUnitAlive()) {
            waveScore += 200;
        }
        
        if (turn < 10) waveScore += 100;
        
        return waveScore;
    }
    
    UnitType getRandomUnitType(bool canBeRoot = false) {
        static vector<UnitType> allTypes = {
            UnitType::KNIGHT, UnitType::ARCHER, UnitType::TANK,
            UnitType::HEALER, UnitType::ASSASSIN, UnitType::MAGE
        };
        
        if (canBeRoot) {
            return (rand() % 2 == 0) ? UnitType::KNIGHT : UnitType::TANK;
        }
        
        return allTypes[rand() % allTypes.size()];
    }
    
    void setPhase(GamePhase p) { phase = p; }
    
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
    const vector<string>& getBattleLog() const { return battleLog; }
    
    ~BinaryStrategyGame() {
        if (playerRoot) delete playerRoot;
        if (enemyRoot) delete enemyRoot;
    }
};

// ==================== 游戏UI ====================

class BinaryStrategyUI {
private:
    BinaryStrategyGame game;
    
public:
    void showMainMenu() {
        cout << "\n";
        cout << "========================================\n";
        cout << "   双子战略 (Binary Strategy)\n";
        cout << "========================================\n";
        cout << "\n";
        cout << "   核心概念:\n";
        cout << "   - Root单位: 强化整棵子树\n";
        cout << "   - 二叉树阵型: 每个单位最多2个子兵\n";
        cout << "   - Roguelike: 每波胜利后三选一奖励\n";
        cout << "\n";
        cout << "========================================\n";
        cout << "   选择模式:\n";
        cout << "   [1] 主帅模式 - 主帅阵亡即失败\n";
        cout << "   [2] 全军模式 - 消灭所有敌军获胜\n";
        cout << "   [Q] 退出游戏\n";
        cout << "========================================\n";
        cout << "\n请选择: ";
    }
    
    void showGameState() {
        cout << "\n";
        cout << "========================================\n";
        cout << "   波次 " << setw(3) << game.getWave() 
             << " | 回合 " << setw(3) << game.getTurn() 
             << " | 得分: " << setw(6) << game.getScore() << "\n";
        cout << "========================================\n";
        
        cout << "\n   === 玩家阵型 ===\n";
        displayFormation(game.getPlayerUnits(), true);
        
        cout << "\n   === 敌人阵型 ===\n";
        displayFormation(game.getEnemyUnits(), false);
        
        cout << "\n   === 战斗日志 ===\n";
        displayBattleLog();
        
        cout << "========================================\n";
        cout << "   操作: [N]下一回合 [A]自动 [P]暂停\n";
        cout << "========================================\n";
    }
    
    void displayFormation(const vector<StrategyNode*>& units, bool isPlayer) {
        int aliveCount = 0;
        int totalHP = 0;
        
        for (auto* node : units) {
            if (!node->isNodeActive() || !node->getUnit()) continue;
            BattleUnit* unit = node->getUnit();
            if (unit->isUnitAlive()) {
                aliveCount++;
                totalHP += unit->getCurrentHealth();
            }
        }
        
        cout << "   存活: " << aliveCount << " | 总血量: " << totalHP << "\n";
        
        for (auto* node : units) {
            if (!node->isNodeActive() || !node->getUnit()) continue;
            
            BattleUnit* unit = node->getUnit();
            if (!unit->isUnitAlive()) continue;
            
            cout << "   [" << setw(2) << node->getNodeId() << "] "
                 << unit->getUnitIcon() << " " << unit->getUnitName();
            
            int healthPercent = unit->getCurrentHealth() * 100 / unit->getMaxHealth();
            cout << " HP:" << setw(3) << unit->getCurrentHealth() 
                 << "/" << setw(3) << unit->getMaxHealth();
            
            if (unit->isUnitCommander()) cout << " [主]";
            if (unit->isUnitRoot()) cout << " [R]";
            
            cout << "\n";
            
            cout << "       ATK:" << setw(3) << unit->getCurrentAttack()
                 << " DEF:" << setw(3) << unit->getCurrentDefense()
                 << " SPD:" << setw(2) << unit->getSpeed() << "\n";
        }
    }
    
    void displayBattleLog() {
        const auto& log = game.getBattleLog();
        int maxLines = 5;
        
        int start = max(0, (int)log.size() - maxLines);
        
        for (int i = start; i < log.size(); ++i) {
            cout << "   " << log[i] << "\n";
        }
        
        if (log.empty()) {
            cout << "   (等待行动...)\n";
        }
    }
    
    void showRoguelikeRewards() {
        cout << "\n";
        cout << "========================================\n";
        cout << "   波次胜利！选择奖励（三选一）\n";
        cout << "========================================\n";
        
        const auto& rewards = game.getRewards();
        for (int i = 0; i < rewards.size(); ++i) {
            cout << "   [" << (i + 1) << "] " << rewards[i].description << "\n";
        }
        
        cout << "========================================\n";
        cout << "\n请选择奖励 (1-3): ";
    }
    
    void showGameOver() {
        cout << "\n";
        if (game.getResult() == BattleResult::WIN) {
            cout << "========================================\n";
            cout << "          胜利！成功击败所有敌人！\n";
            cout << "========================================\n";
        } else {
            cout << "========================================\n";
            cout << "          失败！主帅阵亡或全军覆没\n";
            cout << "========================================\n";
        }
        
        cout << "\n   === 最终得分 ===\n";
        cout << "   总得分: " << setw(6) << game.getScore() << "\n";
        cout << "   总波次: " << setw(6) << game.getWave() << "\n";
        cout << "   总回合: " << setw(6) << game.getTurn() << "\n";
        
        cout << "\n   [R] 重新开始\n";
        cout << "   [Q] 退出游戏\n";
        cout << "========================================\n";
        cout << "\n请选择: ";
    }
    
    void run() {
        while (true) {
            showMainMenu();
            
            char choice;
            cin >> choice;
            choice = toupper(choice);
            
            if (choice == 'Q') {
                cout << "\n感谢游玩！再见！\n";
                break;
            }
            
            if (choice >= '1' && choice <= '2') {
                bool commanderMode = (choice == '1');
                playGame(commanderMode);
            }
        }
    }
    
    void playGame(bool commanderMode) {
        game.initGame();
        bool autoMode = false;
        
        while (game.getPhase() != GamePhase::GAME_OVER) {
            if (game.getPhase() == GamePhase::BATTLE) {
                if (!autoMode) {
                    showGameState();
                    
                    cout << "\n请选择操作: ";
                    char action;
                    cin >> action;
                    action = toupper(action);
                    
                    switch (action) {
                        case 'N':
                            game.battleTurn();
                            break;
                            
                        case 'A':
                            autoMode = true;
                            cout << "\n进入自动战斗模式...\n";
                            break;
                            
                        case 'P':
                            cout << "\n暂停中...\n";
                            break;
                            
                        default:
                            cout << "\n无效操作！\n";
                    }
                } else {
                    showGameState();
                    game.battleTurn();
                    
                    if (game.getPhase() != GamePhase::BATTLE) {
                        autoMode = false;
                    }
                }
            } else if (game.getPhase() == GamePhase::ROGUELIKE_REWARD) {
                handleRoguelikeReward();
            } else if (game.getPhase() == GamePhase::SETUP) {
                game.setPhase(GamePhase::BATTLE);
            }
        }
        
        handleGameOver();
    }
    
    void handleRoguelikeReward() {
        showRoguelikeRewards();
        
        int rewardChoice;
        cin >> rewardChoice;
        
        if (rewardChoice >= 1 && rewardChoice <= 3) {
            game.selectReward(rewardChoice - 1);
            cout << "\n已选择奖励！准备下一波次...\n";
        }
    }
    
    void handleGameOver() {
        while (true) {
            showGameOver();
            char endChoice;
            cin >> endChoice;
            endChoice = toupper(endChoice);
            
            switch (endChoice) {
                case 'R':
                    game.initGame();
                    playGame(game.getResult() != BattleResult::LOSE);
                    return;
                case 'Q':
                    cout << "\n感谢游玩！再见！\n";
                    return;
            }
        }
    }
};

// ==================== 主程序 ====================

int main() {
    cout << "正在启动 双子战略 (Binary Strategy)...\n";
    cout << "游戏类型: 策略战棋/Roguelike\n";
    cout << "核心概念: 二叉树阵型、Root机制\n\n";
    
    BinaryStrategyUI gameUI;
    gameUI.run();
    
    return 0;
}