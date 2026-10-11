#!/usr/bin/bash

git_root="$(git rev-parse --show-toplevel)"

git config --local core.hooksPath "$git_root/scripts"
