#!/bin/bash 

SRC_FILES=(
	CMakeLists.txt
	codeGen.h
	graph.c
	logAnalyzer.c
	opList.c
	opList.h
	print.c
	print.h
	tags
	util.h
)

for f in ${SRC_FILES[@]}; do
  echo "touch ${f}"
  touch ${f}

  echo "git add ${f}"
  git add ${f}

  echo "git commit -m \"Adding file ${f}\""
  git commit -m "Adding file ${f}"
done

git push -u origin main


