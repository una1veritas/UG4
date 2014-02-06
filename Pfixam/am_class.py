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
            

    def checkequal(self, s1,s2):
        if (s1 in self.st_0 and s2 in self.st_0) or (s1 in self.st_1 and s2 in self.st_1):
            return 1
        else:
            return -1

