#!/bin/bash

# git config

# colorful
git config --global color.ui true

# alias
git config --global alias.ci 'commit'
git config --global alias.st 'status'
git config --global alias.co 'checkout'
git config --global alias.br 'branch -v'
git config --global alias.lo 'log --pretty=oneline --abbrev-commit --decorate'
git config --global alias.lg 'log --pretty=oneline --graph --abbrev-commit --decorate'
git config --global alias.fh 'fetch'
git config --global alias.pl 'pull'
git config --global alias.ps 'push'
git config --global alias.fu 'fetch upstream -v'
git config --global alias.mu 'merge upstream/master'

# 解决中文名称显示成"\xxx\xxx\xxx"的问题
git config --global core.quotepath false


