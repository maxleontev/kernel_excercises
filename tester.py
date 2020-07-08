#!/usr/bin/env python3
# coding=utf-8


import os

fd = os.open("/dev/solution_node", os.O_RDWR | os.O_NOCTTY)
test = open(fd, "wb+", buffering=0)

#test.write(b"hello satan ho ho ho ")
#test.seek(0)

#data = test.read()
#print(data)



test.seek(0,0)
data = test.read()
print(data)
test.seek(0,2)
test.write(b"RTegX")
test.seek(-6,2)
data = test.read()
print(data)
test.seek(6,0)
test.write(b"kJgTFXb")
test.seek(-12,2)
test.write(b"s")
test.seek(13,0)
test.write(b"kNBSdOC")
test.seek(0,0)
data = test.read()
print(data)


