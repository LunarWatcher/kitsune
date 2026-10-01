#!/usr/bin/bash

VIOLET_EXE=${VIOLET_EXE:-violet}

echo "Using $VIOLET_EXE"

git_root=$(git rev-parse --show-toplevel)
cd "$git_root/docs"

rm -rf pages
time "$VIOLET_EXE" generate

if [[ $? != 0 ]]; then
    echo "Generation failed!"
    exit 1
fi

cd pages
git init
git checkout -b pages
git remote add cb git@codeberg.org:LunarWatcher/kitsune

git add -A
git commit -m "Generate page"

git push -f cb pages
