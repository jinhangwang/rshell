# cs100 - hw0 - rshell

# Program Overview
This program implements the basic function of rshell, which contains the acquirement of commands, the method of handling the commands with its' proper arguements , and the logical operations of ```&&```, ```||``` and ```;```.

# How to use
With the Makefile, the program can be executed by steps below:

```
git clone https://github.com/jinhangwang/rshell.git
cd rshell
git checkout hw0
make
bin/rshell
```
# Provided examples
```
ls
ls -a
ls -a -l
ls && pwd
ls && wrongcommand
wrongcommand && ls
ls -a && ls -a -l
ls || ls
ls || wrongcommand
wrongcommand || ls
ls;ls
ls;;;;;;;;;;ls
```
