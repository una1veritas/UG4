#am.py
#automaton program

import sys
import re
import math

from am_class import Am, Table

argvs = sys.argv
#print argvs

am = Am(0)

ex = argvs[1]
label = argvs[2]
i_ex = ex
i_label = label

#exlen = len(ex)
print len(ex) #exlen

#work = {}
i = len(ex) #exlen


while ex != "":
    if i > 0:
        am.work.update({(i-1,ex[i-1]):[i,int(label[i-1])]})
        i -= 1
        ex = ex[:-1]
        label = label[:-1]
    else:
        break
else:
    print "Input Dictionary:", am.work
    print



i_work = {}
for i in am.work.items():
    i_work.update({(i[0][0],i[1][0]):[i[0][1]]})
print i_work
print



for i in am.work.items():
    if i[1][1] == 0:
        if i[1][0] not in am.st_0:
            am.st_0.append(i[1][0])
    else:
        if i[1][0] not in am.st_1:
            am.st_1.append(i[1][0])
print "State 0:", am.st_0
print "State 1:", am.st_1
print

st = [[len(argvs[1])]]
print "Last State:",st


n = len(am.work)
print n
print

table = Table(n, argvs[1])





#make table
t = [["-" for i in range(n+1,0,-1)]for j in range(n+1,0,-1)]

print "     ",
for i in range(0,n+1): 
    if i > 10: 
        print " ",i,
    else: 
        print "  ",i,
print

#main
for i in range(n,-1,-1):
    for j in range(i-1,-1,-1):
        if i == n:
            if am.checkequal(i,j) == 1:
                t[i][j] = "O"
            else:
                t[i][j] = "X"
        else:
            r1 = am.checkequal(i,j)
            r2 = am.checkequal(i+1,j+1)
            if r1 == 1 and r2 == 1:
                if i_work[i,i+1] != i_work[j,j+1]:
                    t[i][j] = "Y"
                else:
                    t[i][j] = "O"
            elif r1 == 1:
                if i_work[i,i+1] != i_work[j,j+1]:
                    t[i][j] = "Y"
                else:
                    t[i][j] = "X"
            else:
                t[i][j] = "X"
    print '%3d'%i,"->",t[i]


print
print "Start Check"
table.unitcheck(n,st)

accept = [1]
#end state

