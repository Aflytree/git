#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import os
#import commands
import subprocess
#test function  

log_file = open("message.log", "w")
sys.stdout = log_file

#print("in python function:")  
def add(a,b):  
    print("in python function add")  
    print("a = " + str(a))  
    print("b = " + str(b))  
    print("ret = " + str(a+b))  
  
def foo(a):  
    print("in python function foo")  
    print("a = " + str(a))  
    print("ret = " + str(a * a))  

def multiply(a,b):
    print("I Will compute", a, "times", b)
    c = 0
    for i in range(0, a):
        c = c + b
    print("result = ", c)  
    return c

def compile_proj():
    ret, back = subprocess.getstatusoutput('ls -l')
    print(ret)
    if ret != 0:
        print("error in ls -l")
    ret, back = subprocess.getstatusoutput("pwd")
    print(back)
    os.chdir("../../fprintf_back/build/")
    cmd = "./build.sh"
    ret, back = subprocess.getstatusoutput(cmd)
    if ret != 0:
        print("error in fprintf_back build")
    print(ret)
    print(back)
    return 1

if __name__ == '__main__':
    multiply(3, 3)
    add(4,7)
    compile_proj()
    # print(__name__)


#class guestlist:  
#    def __init__(self):  
#        print("aaaa")  
#    def p():  
#      print("bbbbb")  
#    def __getitem__(self, id):  
#      return "ccccc"  
#def update():  
#    guest = guestlist()  
#    print(guest['aa'])  
#  
#update()
