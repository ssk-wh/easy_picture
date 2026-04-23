#!/bin/bash
set -e

BUILD_DIR="build"
BUILD_TYPE="Release"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Parse arguments
for arg in "$@"; do
    case "$arg" in
        debug)    BUILD_TYPE="Debug" ;;
        release)  BUILD_TYPE="Release" ;;
        clean)    rm -rf "$BUILD_DIR"; echo "Cleaned."; exit 0 ;;
        --clean)
            rm -f "$BUILD_DIR/CMakeCache.txt"
            rm -rf "$BUILD_DIR/CMakeFiles"
            echo "CMake cache cleared."
            ;;
        *) echo "Unknown option: $arg"; exit 1 ;;
    esac
done

echo "================================================"
echo "  SimplePicture Build ($BUILD_TYPE)"
echo "================================================"

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Configure
echo "[1/2] CMake configure..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DBUILD_TESTS=OFF

# Build
echo "[2/2] Building (-j$JOBS)..."
cmake --build "$BUILD_DIR" -j "$JOBS"

if [ -f "$BUILD_DIR/SimplePicture" ]; then
    SIZE=$(stat -c%s "$BUILD_DIR/SimplePicture" 2>/dev/null || stat -f%z "$BUILD_DIR/SimplePicture" 2>/dev/null || echo "?")
    echo "================================================"
    echo "  Build succeeded: $BUILD_DIR/SimplePicture"
    echo "  Size: $SIZE bytes"
    echo "================================================"
else
    echo "[ERROR] Build completed but executable not found!"
    exit 1
fi
