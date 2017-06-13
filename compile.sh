#!/bin/bash

echo "Compiling topzera and myPstree..."

echo "..."
g++ myPsTree.cpp proc_common.cpp -o myPsTree
echo "..."
g++ topzera.cpp proc_common.cpp -o topzera
echo "Done!"
