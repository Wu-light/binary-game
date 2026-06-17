@echo off
echo ========================================
echo 双子战略编译脚本
echo ========================================
echo.

REM 检查编译器
where cl >nul 2>&1
if %errorlevel% equ 0 (
    echo 使用 MSVC 编译器...
    cl /EHsc /std:c++17 /Fe:binary_strategy.exe main.cpp
    goto :end
)

where g++ >nul 2>&1
if %errorlevel% equ 0 (
    echo 使用 MinGW G++ 编译器...
    g++ -std=c++17 -o binary_strategy.exe main.cpp
    goto :end
)

echo 错误: 未找到编译器!
echo 请安装以下任一编译器:
echo   1. Microsoft Visual Studio (MSVC)
echo   2. MinGW-w64 (G++)
echo.
echo 安装指南:
echo   MSVC: https://visualstudio.microsoft.com/
echo   MinGW: https://www.mingw-w64.org/
echo.
pause
exit /b 1

:end
if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo 编译成功!
    echo 可执行文件: binary_strategy.exe
    echo ========================================
    echo.
    echo 运行游戏:
    echo   binary_strategy.exe
    echo.
) else (
    echo.
    echo ========================================
    echo 编译失败!
    echo 请检查编译器配置
    echo ========================================
    echo.
)

pause