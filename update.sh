#!/bin/bash
echo "=== POK Robot Control Update ==="

git pull origin main --rebase

echo "Building..."
cd build 2>/dev/null || mkdir -p build && cd build

cmake .. && make -j2

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "Run: sudo ./pok_robot"
else
    echo "❌ Build failed"
fi