#!/usr/bin/env sh
cmake -B build . && make -C build

echo
echo --------------------
echo

./build/bin/Lancer-bot

