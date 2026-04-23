#!/bin/bash
set -e

# ===============================================
# SimplePicture - Linux Pack Script
# Auto-calls build_on_linux.sh release at the start.
# Usage: ./pack_on_linux.sh  (no need to run build manually)
# Requires: dpkg-dev debhelper qtbase5-dev libqt5svg5-dev
#   sudo apt-get install build-essential cmake ninja-build qtbase5-dev libqt5svg5-dev dpkg-dev debhelper
# ===============================================

cd "$(dirname "$0")"

BUILD_DIR="build"

echo "================================================"
echo "  SimplePicture Linux Pack"
echo "================================================"

# Step 0: Build Release
echo "[0/2] Building Release..."
./build_on_linux.sh release || { echo "[ERROR] Build failed! Aborting pack."; exit 1; }

if [ ! -f "$BUILD_DIR/SimplePicture" ]; then
    echo "[ERROR] $BUILD_DIR/SimplePicture not found after build."
    exit 1
fi

# Step 1: Build deb package
echo "[1/2] Building deb package..."
dpkg-buildpackage -us -uc -b

# Step 2: Copy deb to project root
echo "[2/2] Collecting output..."
DEB_FILE=$(ls ../simplepicture_*.deb 2>/dev/null | head -1 || true)
if [ -n "$DEB_FILE" ]; then
    cp "$DEB_FILE" .
fi

echo ""
echo "================================================"
echo "  SUCCESS: Pack completed!"
echo "================================================"
echo ""
echo "Package location:"
ls -l simplepicture_*.deb 2>/dev/null || ls -l ../simplepicture_*.deb 2>/dev/null || echo "  (check parent directory)"
echo ""
