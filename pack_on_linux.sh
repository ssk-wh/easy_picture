#!/bin/bash
# SimplePicture 打包脚本 (Linux) —— 生成 .deb 安装包
# 依赖：build-essential cmake ninja-build qtbase5-dev libqt5svg5-dev dpkg-dev debhelper
#   sudo apt-get install build-essential cmake ninja-build qtbase5-dev libqt5svg5-dev dpkg-dev debhelper

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 从 CMakeLists.txt 提取版本号
VERSION=$(grep -oP 'project\(SimplePicture VERSION \K[0-9.]+' "$SCRIPT_DIR/CMakeLists.txt")
if [ -z "$VERSION" ]; then
    echo "ERROR: 无法从 CMakeLists.txt 读取版本号"
    exit 1
fi

echo "=== SimplePicture 打包 (Linux) ==="
echo "项目目录: $SCRIPT_DIR"
echo "版本: $VERSION"
echo ""

# 检查依赖
MISSING_TOOLS=()
for tool in cmake dpkg-buildpackage debhelper dh; do
    command -v "$tool" &>/dev/null || MISSING_TOOLS+=("$tool")
done
if [ ${#MISSING_TOOLS[@]} -gt 0 ]; then
    echo "ERROR: 缺少以下工具: ${MISSING_TOOLS[*]}"
    echo "请执行: sudo apt-get install build-essential cmake ninja-build qtbase5-dev libqt5svg5-dev dpkg-dev debhelper"
    exit 1
fi

echo "[1/2] 使用 dpkg-buildpackage 构建 deb 包..."
cd "$SCRIPT_DIR"
dpkg-buildpackage -us -uc -b

echo ""
echo "[2/2] 复制 deb 到项目根目录..."
DEB_FILE=$(ls "$SCRIPT_DIR/../simplepicture_"*.deb 2>/dev/null | head -1 || true)
if [ -z "$DEB_FILE" ]; then
    echo "ERROR: 未找到生成的 deb 文件"
    exit 1
fi
cp "$DEB_FILE" "$SCRIPT_DIR/"
DEB_NAME="$(basename "$DEB_FILE")"

echo ""
echo "=== 打包完成 ==="
echo "安装包: $SCRIPT_DIR/$DEB_NAME"
echo ""
echo "安装方法: sudo dpkg -i $DEB_NAME"
