#am.py
#automaton program

import sys
import re
import math

argvs = sys.argv

print argvs


ex = argvs[1]
label = argvs[2]
i_ex = ex
i_label = label

exlen = len(ex)
print exlen

work = {}
i = exlen


while ex != "":
    if i > 0:
        work.update({(i-1,ex[i-1]):[i,int(label[i-1])]})
        i -= 1
        ex = ex[:-1]
        label = label[:-1]
    else:
        break
else:
    print "Input Dictionary:",work
    print



i_work = {}
for i in work.items():
    i_work.update({(i[0][0],i[1][0]):[i[0][1]]})
print i_work
print


st_0 = []
st_1 = [0]
for i in work.items():
    if i[1][1] == 0:
        if i[1][0] not in st_0:
            st_0.append(i[1][0])
    else:
        if i[1][0] not in st_1:
            st_1.append(i[1][0])
print "State 0:", st_0
print "State 1:", st_1
print

st = [[len(argvs[1])]]
print "Last State:",st


n = len(work)
print n
print



def checkequal(s1,s2):
    if (s1 in st_0 and s2 in st_0) or (s1 in st_1 and s2 in st_1):
        return 1
    else:
        return -1

#find state
def find_liststate(alist,i):
    x = 0
    print alist,i
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
    x = find_liststate(group,j)
    print "list state:",x
    group = change_list(group,i,x)

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

def lastchecking(i,j,olist,table):
    ol = len(olist)-1
    l = len(olist[1])
    flag = 0
    print "Olist:",i,olist[ol-i]
    if olist[ol-i][1] !=[]:
        while l > 0:
	    if table[olist[ol-i][1][l-2]+1][i+1] == 'X':
	        flag = 1
            l = l-1
    else:
        print "non"

    return flag

#search automaton
def unit(table,llist,st,olist):
    l = len(argvs[1])-1
    ol = len(olist)-1
    if llist == []:
        print "not unit"
    else:
        while l > -1:
	    flag = 0
	    print
	    print l
            i = len(argvs[1])
            while i > -1:
	        if table[i][l] == 'Y':
	            if table[i][l+1] == 'O':
		        change(st,l,i)
			break
		    else:
		        if table[i-1][l] =="-":
			    change(st,l,i)
			    break
			else:
			    i = i-1
                elif table[i][l] == 'O':
		    if table[i][l+1] != 'O':
		        flag = lastchecking(l,i,olist,table)
		        if flag == 0:
                            change(st,l,i)
			    break
			else:
			    i = i-1
		    elif i < exlen:
		        if table[i+1][l+1] != 'X':
		            flag = lastchecking(l,i,olist,table)
		            if flag == 0:
                                change(st,l,i)
			        break
			    else:
			        i = i-1
		        else:
		            i = i-1
		    else:
                        i = i-1
		else:
		    i = i-1
	    else:
	        st = st+[[l]]
	    l = l-1
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
unitcheck(t,n,st)

accept = [1] #end state

