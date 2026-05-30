#!/bin/bash

OS=$(uname -s)

if [ "$OS" = "Linux" ]; then
    python3 -m venv .venv
elif [[ "$OS" =~ MINGW* ]] || [[ "$OS" =~ CYGWIN* ]]; then
    python -m venv .venv
else
    echo "未知的系統: $OS"
fi

if [ "$OS" = "Linux" ]; then
    source .venv/bin/activate
elif [[ "$OS" =~ MINGW* ]] || [[ "$OS" =~ CYGWIN* ]]; then
    source .venv/Scripts/activate
else
    echo "未知的系統: $OS"
fi

# 安裝 west 工具
pip install west

# 進入 tools 目錄，使用 -l 參數初始化 west，然後返回上一層
west init -l tools

# 更新 west 模組和專案
west update

# 匯出 zephyr 環境變數
west zephyr-export

# 安裝 Zephyr 核心的 Python 需求套件
pip install -r zephyr/scripts/requirements.txt
