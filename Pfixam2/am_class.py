'''
#am.py
#automaton program
Originally created by Malyn
'''

class Automaton:
    '''
    class for deterministic finite automaton
    '''
    def __init__(self, exstr = None, labstr = None, initlab = None):
        self.states = set()
        self.work = dict()
        self.alphabet = set()
        self.accepting = set()
        
        self.identifydict = dict()

        if isinstance(exstr,str) and isinstance(labstr,str) :
            if len(exstr) == len(labstr) :
                initlab = int(initlab)
            elif len(exstr)+1 == len(labstr) :
                initlab = int(labstr[0])
            if initlab == 1 :
                self.accepting.add(0)
            for i in range(len(exstr)) :
                self.deftrans(i, exstr[i], i+1)
                self.deflabel(i+1, labstr[i])
                #print i, exstr[i], i+1, labstr[i]

    def deftrans(self, fst, onchar, tost):
        fst = int(fst)
        tost = int(tost)
        self.states.add(fst)
        self.states.add(tost)
        self.work[(fst,onchar)] = tost
        self.alphabet.add(onchar)
    
    def deflabel(self, tst, lbl):
        tst = int(tst)
        lbl = int(lbl)
        if lbl == 1 :
            self.accepting.add(tst)
    
    def ontrans(self, fst, tost):
        for achar in self.alphabet :
            if self.work.has_key( (int(fst), achar) ) :
                return achar
        return None
        
    def rejecting(self):
        return self.states - self.accepting
    
    def stateslist(self):
        tmp = list()
        for val in self.identifydict.viewvalues() :
            if not isinstance(val, int) :
                tmp.append(val)
        return tmp
    
    def __len__(self):
        return len(self.work)
    
    def __str__(self):
        tmp = 'Automaton('
        tmp += str(self.alphabet) + ', '
        tmp += str(self.work) + ', '
        tmp += str(self.accepting)
        tmp += ')'
        return tmp
