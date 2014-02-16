'''
#am.py
#automaton program
Originally created by Malyn
'''

class Automaton:
    '''
    class for deterministic finite automaton
    '''
    def __init__(self, exstr = None, labstr = None, inilabel = None):
        self.states = set()
        self.work = dict()
        self.alphabet = set()
        self.accepting = set()
        self.groups = dict()
        if isinstance(exstr,str) and isinstance(labstr,str) :
            if len(exstr) == len(labstr) :
                inilabel = int(inilabel)
            elif len(exstr)+1 == len(labstr) :
                inilabel = int(labstr[0])
            if inilabel == 1 :
                self.accepting.add(0)
            for i in range(len(exstr)) :
                self.deftrans(i, exstr[i], i+1)
                self.deflabel(i+1, labstr[i])
                #print i, exstr[i], i+1, labstr[i]

    def deftrans(self, src, char, dst):
        self.states.add(src)
        self.states.add(dst)
        self.work[(src, char)] = dst
        self.alphabet.add(char)
        self.groups[src] = {src}
        self.groups[dst] = {dst}
    
    def deflabel(self, tst, lbl):
        tst = int(tst)
        lbl = int(lbl)
        if lbl == 1 :
            self.accepting.add(tst)
    
    def ontrans(self, fst, tost):
        for achar in self.alphabet :
            if self.work.has_key( (fst, achar) ) :
                return achar
        return None

    def isequiv(self, s, t):
        if (s not in self.accepting and t not in self.accepting) or (s in self.accepting and t in self.accepting) :
            return True
        else:
            return False
        
    def rejecting(self):
        return self.states - self.accepting
    
    def stateslist(self):
        tmp = list()
        for val in self.groups.viewvalues() :
            tmp.append(sorted(list(val)))
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
