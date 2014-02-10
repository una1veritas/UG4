#am.py
#automaton program
'''
    main based on am.py rev. 0206
    written by Malynn
'''

import sys
#import re
#import math
from am_class import *

'''
ex2
0a0b0b0a1b0b0a0a1b0a0b0b1b0a0b0a0a1a1b0b0a0
 abbabbaababbbabaaabba
0000100010001000011000
ex1
abbabbaaba
(0)00010001000
'''

(exstring, exlabels) = sys.argv[1:3]
#print argvs
if len(exstring) == len(exlabels) :
    ex = exstring
    label = exlabels
    initlabel = 0
    print ex, initlabel, label
elif len(exstring)+1 == len(exlabels):
    ex = exstring
    label = exlabels[1:]
    initlabel = exlabels[0]
    print ex, initlabel, label
else:
    print 'Definition string format error.'
    sys.exit()


am = Automaton(ex, label, initlabel)
#print 'work =', sorted(am.work)
#print 'State 0:', sorted(am.rejecting())
#print 'State 1:', sorted(am.accepting)
print sorted(am.work.iteritems())
print sorted(am.accepting)
'''
work = {}
exlen = len(ex)
print exlen
i_ex = ex
i_label = label
i = exlen

while ex != "":
    if i > 0:
        am.deftrans(i-1, ex[i-1], i, int(label[i-1]))
        i -= 1
        ex = ex[:-1]
        label = label[:-1]
    else:
        break
else:
    print "Input Dictionary:", sorted(am.work.iteritems(), key=lambda arc: arc )
    print
'''
'''
i_work is computed on am.work,
st_0 and st_1 are specified by am.accepting
print "State 0:", st_0
print "State 1:", st_1
print
'''

'''
declare st, the list of states_list, with the group including the final state
'''
st = [[len(exstring)]]
print "Last State:",st


n = len(am)
print n
print


def checkequal(s1,s2):
#    if (s1 in st_0 and s2 in st_0) or (s1 in st_1 and s2 in st_1):
    if (s1 not in am.accepting and s2 not in am.accepting) or (s1 in am.accepting and s2 in am.accepting) :
        return 1
    else:
        return -1

#find state
def find_liststate(alist,i):
    x = 0
#    print alist,i
    for elem in alist:
        if i in elem:
            return x
        x+=1
    else:
        return -1

#changing
def change_list(alist,i,j):
    alist[j].append(i)
    return alist


#change list
def change(group,i,j):
    antiundeterministic = True
    print 'In group', group, 'merge', i, 
    x = find_liststate(group,j)
    print "to target", group[x]
    if antiundeterministic :
        tstates = list(group[x])
        tstates.append(i)
        nxstates = set()
        tchar = am.ontrans(i, i+1)
        for s in tstates :
            if s < len(am) and am.ontrans(s, s+1) == tchar:
                nxstates.add(s+1)
        print 'letter', tchar, 'leads to next state', nxstates, 
        for s in group:
            if set(nxstates).issubset(s) :
                group = change_list(group,i,x)
                print ', passed test.'
                break
        else:
            t = list()
            t.append(i)
            group.append(t)
            print ', avoiding ambiguous arc.'
    else:
        group = change_list(group,i,x)
    print 'added', i, ', resulting', group 

#pickup "O"
def pickup(table,n,m):
    r = []
    j = n+m-1
    while j > 0:
        if table[j][n] == "O":
            r.append([table[j][n],[j,n]])
        else:
            pass
        j = j-1
    return r

#search automaton
def unit(table,llist,st,olist):
    col = len(exstring)-1
#    ol = len(olist)-1
    if llist == []:
        print "not unit"
    else:
        while col > -1:
#            flag = 0
            print
            print 'column =', col, 
#            row = len(exstring)
#            while i > -1:
            for row in range(len(exstring), -1, -1) :
#                print ', row =', row
                if table[row][col] == 'Y':
                    if table[row][col+1] == 'O':
#                        print '[row,col],[row,col+1]=', "Y,O"
                        if table[col+1][col] == 'Y':
                            change(st, col, row)
                            break
#                        else:
#                            i = i-1
                    elif table[row][col+1] == '-':
                        print "Y,-"
                        change(st,col,row)
                        break
                    elif table[row][col+1] == 'X':
                        print "Y,X"
                        change(st,col,row)
                        break
#                   else:
#                       i = i-1
                elif table[row][col] == 'O':
                    if table[row][col+1] == 'X':
                        print "O,X"
                        change(st, col, row)
                        break
                    elif table[row][col+1] == 'O':
                        print "O,O"
                        if table[col+1][col] == 'O':
                            change(st, col, row)
                            break
#                       else:
#                           i = i-1
                    elif table[row][col+1] == '*':
                        print "O,*"
                        if table[col+1][col] == '*':
                            change(st,col,row)
                            break
#                      else:
#                          i = i-1
#                    else:
#                        i = i-1
                elif table[row][col] == '*':
                    if table[row][col+1] == 'O':
                        print "*,O"
                        if table[col+1][col] == '*':
                            change(st,col,row)
                            break
#                       else:
#                           i = i-1
#                   else:
#                       i = i-1
#                else:
#                    print 'all the checks failed.'
#                   i = i-1
            else:
#               print 'append',[l],'as a new state.'
                st = st+[[col]]
            col = col-1
    print
    return st

#make "O"list
def makeolist(llist,n):
    l = len(llist)
    olist = []
    if llist == []:
        return olist
    else:
        while l > 0:
            olist.append(llist[0][1][0])
            llist.pop(0)
            l = l-1
    return olist


def unitcheck(table,n,st):
    l = []
    olist = []
    for i in range(n+1):
        olist = olist+[[n-i,makeolist(pickup(table,n-i,i),n-i)]]
        l = l+pickup(table,n-i,i)
    print "O_Table List:",l
    print
    print "Olist",olist
    print
    st = unit(table,l,st,olist)
    print st
    print

#make table
t = [["-" for i in range(n+1,0,-1)]for j in range(n+1,0,-1)]

print "     ",
for i in range(0,n+1): 
    if i > 10: 
        print " ",i,
    else: 
        print "  ",i,
print

#main_0120
for i in range(n,-1,-1):
    for j in range(i-1,-1,-1):
        if i == n:
            if checkequal(i,j) == 1:
                t[i][j] = "O"
            else:
                t[i][j] = "X"
        else:
            r1 = checkequal(i,j)
            r2 = checkequal(i+1,j+1)
            if r1 == 1 and r2 == 1:
                if am.ontrans(i,i+1) != am.ontrans(j,j+1):
                    t[i][j] = "*"
                else:
                    t[i][j] = "O"
            elif r1 == 1:
                if am.ontrans(i,i+1) != am.ontrans(j,j+1):
                    t[i][j] = "Y"
                else:
                    t[i][j] = "X"
            else:
                t[i][j] = "X"
    print '%3d'%i,"->",t[i]


print
print "Start Check"
unitcheck(t,n,st)

#accept = [1] #end state

