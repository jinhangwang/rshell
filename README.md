# cs100 - hw0 - rshell

# Program Overview
This program implements the basic function of rshell, which contains the acquirement of commands, the method of handling the commands with its' proper arguements , and the logical operations of ```&&```, ```||``` and ```;```.

# How to use
for rshell:
```
git clone https://github.com/jinhangwang/rshell.git
cd rshell
git checkout hw0
make
bin/rshell
```
for ls:
```
git clone  http://github.com/yourusername/rshell.git
cd rshell
git checkout hw1
make
bin/ls
```

# Provided examples
for rshell:
```
// blank line
ls
ls -a
ls -a -l
ls -a -l -R
ls -fjai
ls       -a
            ls
			ls
			ls      -a
||
&&
||||
&&&&
;
;;;;

ls&&pwd
ls && pwd
ls && adsjofqiej
ajsfdoi && ls

ls||ajdfoiqew
ls || pwd
ls || asjdfiqe
asdjfoiw || ls

ls;ls
ls ; ls
ls;;;;;;ls
ls;ls;ls;ls;ls;
ls;ajsdfi
asfjo;ls
ls -a;ls
ls;ls -a
ls -a ; ls
ls ; ls -a


ls #ls
ls#ls
ls#asjfioa
#
#ls

ls #asdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfi
asdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfiasdjfoiajsdfoweqijoijfoasfi

ls -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l -l 
...
```
for ls:
bin/ls
bin/ls -a
bin/ls -l
bin/ls -R
bin/ls -al
bin/ls -aR
bin/ls -la
bin/ls -lR
bin/ls -Ra
bin/ls -Rl
bin/ls -alR
bin/ls -a -l
bin/ls -a -l -R
bin/ls -l -a
bin/li -l -a -R
...
```
