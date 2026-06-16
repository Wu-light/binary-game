/**
 * 双子战略 - 主程序入口
 * 基于二叉树结构的策略对战游戏
 */

#include "game_ui.hpp"
#include <iostream>

using namespace std;

int main() {
    cout << "正在启动 双子战略 (Binary Strategy)...\n";
    cout << "游戏类型: 策略战棋/Roguelike\n";
    cout << "核心概念: 二叉树阵型、Root机制、动态重连\n\n";
    
    BinaryStrategyUI gameUI;
    gameUI.run();
    
    return 0;
}