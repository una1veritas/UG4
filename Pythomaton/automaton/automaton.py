'''
Created on 2014/01/20

@author: sin
'''

class Automaton:
    '''
    classdocs
    '''
    initstate = 0
    current = 0
    states = set()
    finals = set()
    transfunc = dict()
    transfunc[(0,0)] = 0
    definition = (states, initstate, finals, transfunc)
    current = 0

    def __init__(self, sample, labels = ""):
        '''
        Constructor
        '''
        self.initstate = 0
        self.currentstate = self.initstate
        self.states.add(self.initstate)
        self.transfunc[(self.initstate,0)] = self.initstate
        if labels == "" :
            ''' inline format '''
            for pos in range(0, len(sample)) :
                if (pos & 1) == 0 :
                    newstate = pos>>1
                    self.states.add(newstate)
                    if sample[pos] == "1" :
                        self.finals.add(newstate)
                else :
                    newtrans = sample[pos]
                    self.transfunc[(newstate, newtrans)] = newstate+1
        else :
            ''' separated format '''
            if labels[0] == "1" :
                self.finals.add(self.initstate)
            
            for state in range(0, len(sample)) :
                nextate = state+1
                self.states.add(nextate)
                if labels[nextate] == "1" :
                    self.finals.add(nextate)
                newtrans = sample[state]
                self.transfunc[(state, newtrans)] = nextate

    def transfer(self, example):
        for ch in example :
            self.current = self.transfunc[(self.current, ch)]
        return self.current
        
    def accept(self, example):
        self.current = self.initstate
        if self.transfer(example) in self.finals :
            return True
        return False
        
    def __str__(self):
        stream = "Automaton("
        stream += "states{"
        for s in self.states:
            stream += str(s)
            stream += ", "
        stream += "}, "
        stream += "delta("
        for entry in sorted(self.transfunc.iteritems(), key=lambda arc: arc ) :
            stream += str(entry)
            stream += ", "
        stream += "), "
        stream += "finals{"
        tmp = list(self.finals)
        for index in range(0, len(tmp)) :
            stream += str(tmp[index])
            if index+1 < len(tmp) :
                stream += ", "
        stream += "}"
        stream += "; current = "
        stream += str(self.current)
        stream += ")"
        return stream

