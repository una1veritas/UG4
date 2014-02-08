'''
#am.py
#automaton program
Originally created by Malyn
'''

class Automaton:
    '''
    class for deterministic finite automaton
    '''
    def __init__(self):
        self.states = set()
        self.work = {}
        self.alphabet = set()
        self.accepting = set()

    def deftrans(self, fst, onchar, tost, lbl):
        self.states.add(fst)
        self.states.add(tost)
        self.work[(fst,onchar)] = [tost, lbl]
        self.alphabet.add(onchar)
        if lbl == 1 :
            self.accepting.add(tost)
    
    def ontrans(self, fst, tost):
        for achar in self.alphabet :
            if self.work.has_key( (fst, achar) ) :
                return achar
        return None
    
    def nonaccepting(self):
        return self.states - self.accepting
    
    def __len__(self):
        return len(self.work)