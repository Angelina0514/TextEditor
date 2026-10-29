@echo off
chcp 65001 >nul
rem ============ 文本编辑器 一键编译运行 ============
rem 设置 Qt 和 MinGW 的路径
set "PATH=D:\Qt\5.15.2\mingw81_64\bin;D:\Qt\Tools\mingw810_64\bin;%PATH%"

cd /d "D:\TextEditor"
if not exist build mkdir build
cd build

echo [1/3] 生成 Makefile (qmake)...
qmake ..\TextEditor.pro
if errorlevel 1 goto :err

echo [2/3] 编译 (mingw32-make)...
mingw32-make
if errorlevel 1 goto :err

echo [3/3] 编译成功，启动程序...
start "" release\TextEditor.exe
exit /b 0

:err
echo.
echo ========== 编译失败，请检查上方错误信息 ==========
pause
exit /b 1
