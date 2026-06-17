/**
 * 双子战略 - 战斗单位类
 */

#ifndef BATTLE_UNIT_HPP
#define BATTLE_UNIT_HPP

#include "unit_types.hpp"
#include <vector>
#include <string>

using namespace std;

// 战斗单位类
class BattleUnit {
private:
    UnitType type;
    UnitAttributes baseAttributes;
    int currentHealth;
    bool isAlive;
    bool isRoot;              // 是否是Root单位
    bool isCommander;         // 是否是主帅
    int depth;                // 在树中的深度（Root为0）
    vector<Buff> activeBuffs; // 激活的Buff
    
public:
    BattleUnit(UnitType t, bool commander = false) 
        : type(t), isAlive(true), isRoot(false), 
          isCommander(commander), depth(-1) {
        baseAttributes = UnitAttributes::createAttributes(t);
        currentHealth = baseAttributes.maxHealth;
    }
    
    // 应用Root加成（基于深度）
    void applyRootBonus(int rootDepth) {
        depth = rootDepth;
        if (isRoot) {
            // Root单位获得特殊加成
            Buff rootBuff(BuffType::ROOT_BONUS, 30, -1, true);
            activeBuffs.push_back(rootBuff);
        } else {
            // 非Root单位的加成随深度衰减
            int distanceFromRoot = rootDepth - depth;
            if (distanceFromRoot > 0) {
                int bonusPercent = max(5, 30 - distanceFromRoot * 5);
                Buff distanceBuff(BuffType::ROOT_BONUS, bonusPercent, -1, true);
                activeBuffs.push_back(distanceBuff);
            }
        }
    }
    
    // 移除Root加成
    void removeRootBonus() {
        activeBuffs.erase(
            remove_if(activeBuffs.begin(), activeBuffs.end(),
                [](const Buff& b) { return b.type == BuffType::ROOT_BONUS; }),
            activeBuffs.end()
        );
        isRoot = false;
        depth = -1;
    }
    
    // 计算当前攻击力（考虑所有Buff）
    int getCurrentAttack() const {
        int attack = baseAttributes.attack;
        for (const auto& buff : activeBuffs) {
            if (buff.type == BuffType::ATTACK_BOOST || buff.type == BuffType::ROOT_BONUS) {
                attack = attack * (100 + buff.value) / 100;
            }
        }
        return attack;
    }
    
    // 计算当前防御力
    int getCurrentDefense() const {
        int defense = baseAttributes.defense;
        for (const auto& buff : activeBuffs) {
            if (buff.type == BuffType::DEFENSE_BOOST || buff.type == BuffType::ROOT_BONUS) {
                defense = defense * (100 + buff.value) / 100;
            }
        }
        return defense;
    }
    
    // 受到伤害
    void takeDamage(int damage) {
        int actualDamage = max(1, damage - getCurrentDefense());
        
        // 触发特殊技能（骑士的盾牌格挡）
        if (type == UnitType::KNIGHT) {
            actualDamage = actualDamage / 2; // 减少50%伤害
        }
        
        currentHealth -= actualDamage;
        
        if (currentHealth <= 0) {
            currentHealth = 0;
            isAlive = false;
        }
    }
    
    // 恢复生命值
    void heal(int amount) {
        if (!isAlive) return;
        currentHealth = min(currentHealth + amount, baseAttributes.maxHealth);
    }
    
    // 添加Buff
    void addBuff(const Buff& buff) {
        activeBuffs.push_back(buff);
    }
    
    // 更新Buff持续时间
    void updateBuffs() {
        for (auto& buff : activeBuffs) {
            if (!buff.isPermanent) {
                buff.duration--;
                if (buff.duration <= 0) {
                    // 移除过期Buff
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
    
    // 获取单位图标
    string getUnitIcon() const {
        if (!isAlive) return "💀";
        if (isCommander) return "👑";
        if (isRoot) return "⭐";
        
        switch (type) {
            case UnitType::KNIGHT: return "🛡️";
            case UnitType::ARCHER: return "🏹";
            case UnitType::TANK: return "🪨";
            case UnitType::HEALER: return "💚";
            case UnitType::ASSASSIN: return "🗡️";
            case UnitType::MAGE: return "🔮";
            default: return "👤";
        }
    }
    
    // 获取单位名称
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
        
        if (isCommander) name += "(主帅)";
        if (isRoot) name += "(Root)";
        
        return name;
    }
    
    // Getters
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
    
    // Setters
    void setRoot(bool root) { isRoot = root; }
    void setDepth(int d) { depth = d; }
};

#endif // BATTLE_UNIT_HPP