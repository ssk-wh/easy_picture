#!/bin/bash
# SimplePicture 编译脚本 (Linux)
# 依赖：cmake >= 3.16, ninja-build, qtbase5-dev, libqt5svg5-dev
#   sudo apt-get install build-essential cmake ninja-build qtbase5-dev libqt5svg5-dev

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

echo "=== SimplePicture Build (Linux) ==="
echo "项目目录: $SCRIPT_DIR"
echo ""

# 检查依赖
for tool in cmake ninja; do
    if ! command -v "$tool" &>/dev/null; then
        echo "ERROR: 缺少工具 '$tool'"
        echo "请执行: sudo apt-get install build-essential cmake ninja-build qtbase5-dev libqt5svg5-dev"
        exit 1
    fi
done

echo "[1/2] CMake 配置..."
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF

echo ""
echo "[2/2] 编译..."
cmake --build "$BUILD_DIR"

echo ""
echo "=== 编译成功 ==="
echo "输出: $BUILD_DIR/SimplePicture"
