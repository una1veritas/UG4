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
print am
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
#def find_liststate(alist, i):
#    for elem in alist:
#        if i in elem:
#            return elem
#    else:
#        return None


def uniquearc(atm, s, tgt):
    nxtstates = set()
    nxtstates.add(s+1)
    tch = atm.ontrans(s,s+1)
    for elem in tgt:
        if elem < len(atm) and atm.ontrans(elem, elem+1) == tch :
            nxtstates.add(elem+1)
    for s in atm.identifydict.viewvalues() :
        if isinstance(s, list): 
            if set(nxtstates).issubset(s) :
                return True
    else: 
        return False

#change list
def change(group,i,j):
    print 'In group', group, 'merge', i, 
    tgt = find_liststate(group,j)
    print "to target", tgt
    if True or uniquearc(group, i, tgt) :
        tgt.append(i)
    else:
        t = list()
        t.append(i)
        group.append(t)
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
def unit(table,llist,olist):
    if llist == []:
        print "not unit"
        return am.identifydict

    col = len(exstring)
    am.identifydict[col] = [col]
    col -= 1
    while col > -1:
        print
        print 'check col=',col
        for row in range(len(exstring), -1, -1) :
            print 'check row=', row, 
            if table[row][col] == '-':
                am.identifydict[col] = [col]
                break
            if table[row][col] == 'X':
                print 'X'
                continue
            '''
            ambiguity check for merging col to row
            before transition conditions
            '''
            tgtstates = am.identifydict.get(row)
            if isinstance(tgtstates, int) :
                tgtstates = am.identifydict.get(tgtstates)
            if (tgtstates == None) or not uniquearc(am, col, tgtstates) :
                print 'failed ambiguity check.'
                continue
            '''
            proceed to test transition conditions if arc uniqueness check passed.
            '''
            if table[row][col] == 'Y' or table[row][col] == '*':
                print 'Y/*',
#                if table[row][col+1] == 'O':
#                    if table[col+1][col] == 'Y':
                am.identifydict[col] = row # change(st, col, row)
                am.identifydict[row].append(col)
                break
#                elif table[row][col+1] == '-':
#                    print "Y,-"
#                    change(st,col,row)
#                    break
#                elif (table[row][col+1] == 'X') or (table[row][col+1] == 'Y'):
#                    print "Y,X"
#                    change(st,col,row)
#                    break
                
            elif table[row][col] == 'O':
                print 'O,',
#                if row == len(exstring) or table[row][col+1] == 'X':
#                    print 'X'
                am.identifydict[col] = row #change(st, col, row)
                am.identifydict[row].append(col)
                break
#                elif table[row][col+1] == 'O':
#                    print 'O'
#                    if table[col+1][col] == 'O':
#                        change(st, col, row)
#                        break
#                elif table[row][col+1] == '*':
#                    print '*'
#                    if table[col+1][col] == '*':
#                        change(st,col,row)
#                        break
#            elif table[row][col] == '*':
#                print '*',
#                if table[row][col+1] == 'O':
#                    print "*,O"
#                    if table[col+1][col] == '*':
#                        change(st,col,row)
#                        break

        print 'col',col,'task finished.'
        print am.identifydict
        col = col-1
    
    print

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


def unitcheck(table,n):
    l = []
    olist = []
    for i in range(n+1):
        olist = olist+[[n-i,makeolist(pickup(table,n-i,i),n-i)]]
        l = l+pickup(table,n-i,i)
    print "O_Table List:",l
    print
    print "Olist",olist
    print
    unit(table,l,olist)
    print am.stateslist()
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
unitcheck(t,n)

#accept = [1] #end state

