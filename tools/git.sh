#!/bin/sh

ROOT=$PWD

git add $ROOT
git commit -m "backup"
git push gitlab HEAD
git push notabug HEAD
