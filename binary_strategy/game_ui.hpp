/**
 * 双子战略 - 游戏UI系统
 */

#ifndef GAME_UI_HPP
#define GAME_UI_HPP

#include "game_system.hpp"
#include "battle_unit.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// 游戏UI类
class BinaryStrategyUI {
private:
    BinaryStrategyGame game;
    
public:
    // 显示主菜单
    void showMainMenu() {
        cout << "\n";
        cout << "╔════════════════════════════════════════════════════╗\n";
        cout << "║   双子战略 (Binary Strategy) - 二叉树战棋游戏     ║\n";
        cout << "╠════════════════════════════════════════════════════╣\n";
        cout << "║                                                    ║\n";
        cout << "║   游戏简介:                                        ║\n";
        cout << "║   在杰马利王国，双子战略是皇室传承的兵法          ║\n";
        cout << "║   1个Root→2个子兵→无限扩展，形成三角阵型          ║\n";
        cout << "║                                                    ║\n";
        cout << "║   核心概念:                                        ║\n";
        cout << "║   • Root单位: 带Root的棋子强化整棵子树             ║\n";
        cout << "║   • 二叉树阵型: 每个单位最多有两个子兵             ║\n";
        cout << "║   • 动态重连: 单位死亡时自动重连子树               ║\n";
        cout << "║   • Roguelike: 每波胜利后三选一奖励                ║\n";
        cout << "║                                                    ║\n";
        cout << "╠════════════════════════════════════════════════════╣\n";
        cout << "║   选择模式:                                        ║\n";
        cout << "║   [1] 主帅模式 - 主帅阵亡即失败                    ║\n";
        cout << "║   [2] 全军模式 - 消灭所有敌军获胜                  ║\n";
        cout << "║   [Q] 退出游戏                                     ║\n";
        cout << "╚════════════════════════════════════════════════════╝\n";
        cout << "\n请选择: ";
    }
    
    // 显示游戏状态
    void showGameState() {
        cout << "\n";
        cout << "╔════════════════════════════════════════════════════╗\n";
        cout << "║   双子战略 - 波次 " << setw(3) << game.getWave() 
             << " | 回合 " << setw(3) << game.getTurn() 
             << " | 得分: " << setw(6) << game.getScore() << "  ║\n";
        cout << "╠════════════════════════════════════════════════════╣\n";
        
        // 显示玩家阵型
        cout << "║                                                    ║\n";
        cout << "║   === 玩家阵型 ===                                 ║\n";
        displayFormation(game.getPlayerUnits(), true);
        
        // 显示敌人阵型
        cout << "║                                                    ║\n";
        cout << "║   === 敌人阵型 ===                                 ║\n";
        displayFormation(game.getEnemyUnits(), false);
        
        cout << "╠════════════════════════════════════════════════════╣\n";
        cout << "║   操作: [N] 下一回合 | [S] 查看状态 | [P] 暂停     ║\n";
        cout << "╚════════════════════════════════════════════════════╝\n";
    }
    
    // 显示阵型结构
    void displayFormation(const vector<StrategyNode*>& units, bool isPlayer) {
        for (auto* node : units) {
            if (!node->isNodeActive() || !node->getUnit()) continue;
            
            BattleUnit* unit = node->getUnit();
            if (!unit->isUnitAlive()) continue;
            
            cout << "║   Node " << setw(3) << node->getNodeId() << ": "
                 << unit->getUnitIcon() << " "
                 << unit->getUnitName() << setw(15) << "";
            
            // 显示生命值
            int healthPercent = unit->getCurrentHealth() * 100 / unit->getMaxHealth();
            cout << "HP: " << setw(3) << unit->getCurrentHealth() 
                 << "/" << setw(3) << unit->getMaxHealth();
            
            if (healthPercent < 30) {
                cout << " ⚠️";
            } else if (healthPercent > 70) {
                cout << " ✓";
            }
            
            cout << setw(5) << "" << "║\n";
            
            // 显示属性
            cout << "║           攻击:" << setw(3) << unit->getCurrentAttack()
                 << " 防御:" << setw(3) << unit->getCurrentDefense()
                 << " 范围:" << setw(2) << unit->getRange()
                 << setw(15) << "" << "║\n";
        }
    }
    
    // 显示Roguelike奖励选择
    void showRoguelikeRewards() {
        cout << "\n";
        cout << "╔════════════════════════════════════════════════════╗\n";
        cout << "║   🎉 波次胜利！选择奖励（三选一）                  ║\n";
        cout << "╠════════════════════════════════════════════════════╣\n";
        
        const auto& rewards = game.getRewards();
        for (int i = 0; i < rewards.size(); ++i) {
            cout << "║   [" << (i + 1) << "] " << rewards[i].description 
                 << setw(35 - rewards[i].description.length()) << "" << "║\n";
        }
        
        cout << "╚════════════════════════════════════════════════════╝\n";
        cout << "\n请选择奖励 (1-3): ";
    }
    
    // 显示游戏结束界面
    void showGameOver() {
        cout << "\n";
        if (game.getResult() == BattleResult::WIN) {
            cout << "╔════════════════════════════════════════════════════╗\n";
            cout << "║          🎉 胜利！成功击败所有敌人！ 🎉            ║\n";
            cout << "╠════════════════════════════════════════════════════╣\n";
        } else {
            cout << "╔════════════════════════════════════════════════════╗\n";
            cout << "║          😢 失败！主帅阵亡或全军覆没 😢            ║\n";
            cout << "╠════════════════════════════════════════════════════╣\n";
        }
        
        // 显示最终得分
        cout << "║                                                    ║\n";
        cout << "║   === 最终得分 ===                                 ║\n";
        cout << "║   波次得分: " << setw(6) << game.getScore() << setw(30) << "" << "║\n";
        cout << "║   总波次:   " << setw(6) << game.getWave() << setw(30) << "" << "║\n";
        cout << "║   总回合:   " << setw(6) << game.getTurn() << setw(30) << "" << "║\n";
        cout << "║                                                    ║\n";
        
        cout << "║   [R] 重新开始                                     ║\n";
        cout << "║   [Q] 退出游戏                                     ║\n";
        cout << "╚════════════════════════════════════════════════════╝\n";
        cout << "\n请选择: ";
    }
    
    // 显示单位详情
    void showUnitDetail(StrategyNode* node) {
        if (!node || !node->getUnit()) return;
        
        BattleUnit* unit = node->getUnit();
        
        cout << "\n";
        cout << "╔════════════════════════════════════════════════════╗\n";
        cout << "║   单位详情 - Node " << setw(3) << node->getNodeId() 
             << setw(28) << "" << "║\n";
        cout << "╠════════════════════════════════════════════════════╣\n";
        
        cout << "║   " << unit->getUnitIcon() << " " << unit->getUnitName() 
             << setw(35) << "" << "║\n";
        cout << "║                                                    ║\n";
        
        // 显示属性
        cout << "║   生命值: " << setw(3) << unit->getCurrentHealth() 
             << "/" << setw(3) << unit->getMaxHealth();
        int healthPercent = unit->getCurrentHealth() * 100 / unit->getMaxHealth();
        cout << " (" << setw(3) << healthPercent << "%)" 
             << setw(10) << "" << "║\n";
        
        cout << "║   攻击力: " << setw(3) << unit->getCurrentAttack() 
             << " (基础:" << setw(3) << unit->getAttributes().attack << ")"
             << setw(10) << "" << "║\n";
        
        cout << "║   防御力: " << setw(3) << unit->getCurrentDefense() 
             << " (基础:" << setw(3) << unit->getAttributes().defense << ")"
             << setw(10) << "" << "║\n";
        
        cout << "║   攻击范围: " << setw(2) << unit->getRange() 
             << setw(30) << "" << "║\n";
        
        cout << "║   移动速度: " << setw(2) << unit->getSpeed() 
             << setw(30) << "" << "║\n";
        
        cout << "║                                                    ║\n";
        
        // 显示特殊技能
        cout << "║   特殊技能:                                        ║\n";
        cout << "║   " << unit->getAttributes().specialSkill 
             << setw(35) << "" << "║\n";
        
        // 显示树结构信息
        cout << "║                                                    ║\n";
        cout << "║   树结构信息:                                      ║\n";
        cout << "║   深度: " << setw(2) << node->getDepth() 
             << " | Root: " << (unit->isUnitRoot() ? "是" : "否")
             << " | Commander: " << (unit->isUnitCommander() ? "是" : "否")
             << setw(8) << "" << "║\n";
        
        if (node->getParent()) {
            cout << "║   父节点: Node " << setw(3) << node->getParent()->getNodeId() 
                 << setw(25) << "" << "║\n";
        }
        
        if (node->getLeftChild() && node->getLeftChild()->getUnit()) {
            cout << "║   左子节点: " << node->getLeftChild()->getUnit()->getUnitIcon() 
                 << " Node " << setw(3) << node->getLeftChild()->getNodeId() 
                 << setw(20) << "" << "║\n";
        }
        
        if (node->getRightChild() && node->getRightChild()->getUnit()) {
            cout << "║   右子节点: " << node->getRightChild()->getUnit()->getUnitIcon() 
                 << " Node " << setw(3) << node->getRightChild()->getNodeId() 
                 << setw(20) << "" << "║\n";
        }
        
        cout << "╚════════════════════════════════════════════════════╝\n";
    }
    
    // 显示暂停菜单
    void showPauseMenu() {
        cout << "\n";
        cout << "╔════════════════════════════════════════════════════╗\n";
        cout << "║            游戏已暂停                              ║\n";
        cout << "╠════════════════════════════════════════════════════╣\n";
        cout << "║   [C] 继续游戏                                     ║\n";
        cout << "║   [R] 重新开始                                     ║\n";
        cout << "║   [M] 返回主菜单                                   ║\n";
        cout << "║   [Q] 退出游戏                                     ║\n";
        cout << "╚════════════════════════════════════════════════════╝\n";
        cout << "\n请选择: ";
    }
    
    // 显示战斗日志
    void showBattleLog(const string& message) {
        cout << "\n[Battle] " << message << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    
    // 主游戏循环
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
    
    // 游戏主循环
    void playGame(bool commanderMode) {
        game.initGame();
        
        while (game.getPhase() != GamePhase::GAME_OVER) {
            if (game.getPhase() == GamePhase::BATTLE) {
                showGameState();
                
                cout << "\n请选择操作: ";
                char action;
                cin >> action;
                action = toupper(action);
                
                switch (action) {
                    case 'N': // 下一回合
                        game.battleTurn();
                        break;
                        
                    case 'S': // 查看状态
                        // 显示单位详情（简化版本）
                        break;
                        
                    case 'P': // 暂停
                        handlePause();
                        break;
                        
                    default:
                        cout << "\n无效操作！请重试。\n";
                        this_thread::sleep_for(chrono::milliseconds(500));
                }
            } else if (game.getPhase() == GamePhase::ROGUELIKE_REWARD) {
                handleRoguelikeReward();
            } else if (game.getPhase() == GamePhase::SETUP) {
                // 进入战斗阶段
                game.setPhase(GamePhase::BATTLE);
            }
        }
        
        // 游戏结束
        handleGameOver();
    }
    
    // 处理暂停
    void handlePause() {
        while (true) {
            showPauseMenu();
            char pauseChoice;
            cin >> pauseChoice;
            pauseChoice = toupper(pauseChoice);
            
            switch (pauseChoice) {
                case 'C':
                    return;
                case 'R':
                    game.initGame();
                    return;
                case 'M':
                    return;
                case 'Q':
                    cout << "\n感谢游玩！再见！\n";
                    exit(0);
            }
        }
    }
    
    // 处理Roguelike奖励选择
    void handleRoguelikeReward() {
        showRoguelikeRewards();
        
        int rewardChoice;
        cin >> rewardChoice;
        
        if (rewardChoice >= 1 && rewardChoice <= 3) {
            game.selectReward(rewardChoice - 1);
            cout << "\n已选择奖励！准备下一波次...\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
        } else {
            cout << "\n无效选择！请重新选择。\n";
        }
    }
    
    // 处理游戏结束
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

#endif // GAME_UI_HPP