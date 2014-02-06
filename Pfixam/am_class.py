'''
#am.py
#automaton program
Originally created by Malyn
'''

import sys

class Am:
    '''
    am.py functions
    originally written by Malynn
    '''
    
    def __init__(self, inilab):
        if inilab == 0 :
            self.st_0 = [0]
            self.st_1 = []
        else:
            self.st_0 = []
            self.st_1 = [0]
        self.work = {}

    def checkequal(self, s1,s2):
        if (s1 in self.st_0 and s2 in self.st_0) or (s1 in self.st_1 and s2 in self.st_1):
            return 1
        else:
            return -1

class Table:
    '''
    a class for tables
    '''
    
    def __init__(self, n, ex):
        self.table = [["-" for i in range(n+1,0,-1)]for j in range(n+1,0,-1)]
        self.example = ex

    #pickup "O"
    def pickup(self, n, m):
        r = []
        j = n+m-1
        while j > 0:
            if self.table[j][n] == "O":
                r.append([self.table[j][n],[j,n]])
            else:
                pass
            j = j-1
        return r
    
#find state
    def find_liststate(self,alist,i):
        x = 0
        print alist,i
        for elem in alist:
            if i in elem:
                return x
            x+=1
        else:
            return -1
    
#changing
    def change_list(self, alist,i,j):
        alist[j].append(i)
        return alist
    
    
#change list
    def change(self, group,i,j):
        x = self.find_liststate(group,j)
        print "list state:",x
        group = self.change_list(group,i,x)

#search automaton
    def unit(self, llist, st, olist):
        l = len(self.example)-1
#        ol = len(olist)-1
        if llist == []:
            print "not unit"
        else:
            while l > -1:
                flag = 0
                print
                print l
                i = len(self.example)
                while i > -1:
                    if self.table[i][l] == 'Y':
                        if self.table[i][l+1] == 'O':
                            self.change(st,l,i)
                            break
                        else:
                            if self.table[i-1][l] =="-":
                                self.change(st,l,i)
                                break
                            else:
                                i = i-1
                    elif self.table[i][l] == 'O':
                        if self.table[i][l+1] != 'O':
                            flag = self.lastchecking(l,i,olist, self.table)
                            if flag == 0:
                                self.change(st,l,i)
                                break
                            else:
                                i = i-1
                        elif i < len(self.example):
                            if self.table[i+1][l+1] != 'X':
                                flag = self.lastchecking(l,i,olist, self.table)
                                if flag == 0:
                                    self.change(st,l,i)
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
    def makeolist(self, llist,n):
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
    
    
    def unitcheck(self,n,st):
        l = []
        olist = []
        for i in range(n+1):
            olist = olist+[[n-i,self.makeolist(self.pickup(n-i,i),n-i)]]
            l = l + self.pickup(n-i,i)
        print "O_Table List:",l
        print
        print "Olist",olist
        print
        st = self.unit(l,st,olist)
        print st
        print

    def lastchecking(self, i,j,olist):
        ol = len(olist)-1
        l = len(olist[1])
        k = len(self.example)-1
        flag = 0
        print "Olist:",i,olist[ol-i]
        if i == len(self.example)-1:
            return flag
        elif olist[ol-i][1] !=[]:
            while l > 0:
                if self.table[olist[ol-i][1][l-2]+1][i+1] == 'X':
                    flag = 1
                    l = l-1
        else:
            while k > 0:
                print k,i, self.table[k][i]
                if self.table[k][i] == 'Y':
                    flag = 0
                    break
                else:
                    flag = 1
                k = k-1
            print "non"
    
        return flag
