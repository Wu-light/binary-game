#!/bin/bash

echo "========================================"
echo "双子战略编译脚本 (Linux/macOS)"
echo "========================================"
echo ""

# 检查G++编译器
if command -v g++ >/dev/null 2>&1; then
    echo "使用 G++ 编译器..."
    g++ -std=c++17 -o binary_strategy main.cpp
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "========================================"
        echo "编译成功!"
        echo "可执行文件: binary_strategy"
        echo "========================================"
        echo ""
        echo "运行游戏:"
        echo "  ./binary_strategy"
        echo ""
    else
        echo ""
        echo "========================================"
        echo "编译失败!"
        echo "请检查编译器配置"
        echo "========================================"
        echo ""
    fi
else
    echo "错误: 未找到 G++ 编译器!"
    echo "请安装 G++ 编译器:"
    echo "  Ubuntu/Debian: sudo apt-get install g++"
    echo "  macOS: xcode-select --install"
    echo ""
fi