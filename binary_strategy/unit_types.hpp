/**
 * 双子战略 - 单位类型定义
 */

#ifndef UNIT_TYPES_HPP
#define UNIT_TYPES_HPP

#include <string>
#include <vector>

using namespace std;

// 单位类型
enum class UnitType {
    KNIGHT,      // 骑士 - 近战，高血量
    ARCHER,      // 弓箭手 - 远程，中等血量
    TANK,        // 坦克 - 高防御，低攻击
    HEALER,      // 治疗者 - 辅助，可以为队友治疗
    ASSASSIN,    //刺客 - 高攻击，低血量
    MAGE         // 法师 - 远程魔法，特殊技能
};

// 单位属性结构
struct UnitAttributes {
    int maxHealth;        // 最大生命值
    int attack;           // 攻击力
    int defense;          // 防御力
    int range;            // 攻击范围
    int speed;            // 移动速度
    bool canBeRoot;       // 是否可以成为Root
    string specialSkill;  // 特殊技能描述
    
    // 根据单位类型初始化属性
    static UnitAttributes createAttributes(UnitType type) {
        UnitAttributes attr;
        switch (type) {
            case UnitType::KNIGHT:
                attr.maxHealth = 150;
                attr.attack = 25;
                attr.defense = 20;
                attr.range = 1;  // 近战
                attr.speed = 2;
                attr.canBeRoot = true;
                attr.specialSkill = "盾牌格挡：减少50%伤害";
                break;
            case UnitType::ARCHER:
                attr.maxHealth = 80;
                attr.attack = 30;
                attr.defense = 10;
                attr.range = 3;  // 远程
                attr.speed = 1;
                attr.canBeRoot = false;
                attr.specialSkill = "精准射击：攻击力+20%";
                break;
            case UnitType::TANK:
                attr.maxHealth = 200;
                attr.attack = 15;
                attr.defense = 40;
                attr.range = 1;
                attr.speed = 1;
                attr.canBeRoot = true;
                attr.specialSkill = "坚守阵地：防御力+50%";
                break;
            case UnitType::HEALER:
                attr.maxHealth = 60;
                attr.attack = 10;
                attr.defense = 5;
                attr.range = 2;
                attr.speed = 1;
                attr.canBeRoot = false;
                attr.specialSkill = "治疗光环：每回合恢复队友20HP";
                break;
            case UnitType::ASSASSIN:
                attr.maxHealth = 50;
                attr.attack = 40;
                attr.defense = 5;
                attr.range = 1;
                attr.speed = 3;
                attr.canBeRoot = false;
                attr.specialSkill = "暗杀：对Root单位伤害+100%";
                break;
            case UnitType::MAGE:
                attr.maxHealth = 70;
                attr.attack = 35;
                attr.defense = 8;
                attr.range = 2;
                attr.speed = 1;
                attr.canBeRoot = false;
                attr.specialSkill = "法术连击：攻击3个目标";
                break;
        }
        return attr;
    }
};

// Buff类型
enum class BuffType {
    ATTACK_BOOST,     // 攻击力提升
    DEFENSE_BOOST,    // 防御力提升
    HEALTH_REGEN,     // 生命恢复
    SHIELD,           // 护盾
    ROOT_BONUS        // Root加成（特殊）
};

// Buff结构
struct Buff {
    BuffType type;
    int value;          // 加成数值
    int duration;       // 持续回合数
    bool isPermanent;   // 是否永久
    
    Buff(BuffType t, int v, int d, bool perm = false)
        : type(t), value(v), duration(d), isPermanent(perm) {}
};

#endif // UNIT_TYPES_HPP