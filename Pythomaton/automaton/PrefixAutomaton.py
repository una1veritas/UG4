'''
Created on 2014/01/20

@author: sin
'''

class PrefixSample:
    '''
    The class for Prefix samples in inline representation
    '''
    
    def __init__(self, string = '', labels = None, name = 0) :
        '''
        print 'debug init: ', string, ',',labels
        '''
        if string != '' and labels == None:
            self.labels = ''
            self.string = ''
            for ix in range(0, len(string)) :
                if (ix % 2) == 0 :
                    self.labels += string[ix]
                else:
                    self.string += string[ix]
        elif string == '' and len(labels) == 1 :
            self.string = ''
            self.labels = labels
        elif string == '' and labels == None :
            self.string = ''
            self.labels = '0'
        else:
            self.string = string
            self.labels = labels
        self.name = name
        return

    def __getitem__(self, index):
        if isinstance(index, slice) :
            indexslice = index
        else:
            indexslice = slice(index, index+1, None)
        
        labelslice = slice(indexslice.start, indexslice.stop+1, indexslice.step)
        '''print labelslice'''
        newname = indexslice.start
        '''print self.string.__getitem__(index), ' ',self.labels.__getitem__(labelslice)'''
        return PrefixSample(self.string.__getitem__(indexslice), self.labels.__getitem__(labelslice), newname)

        
    def label(self, index):
        return self.labels[index]
    
    def at(self, index):
        return self.string[index]

    def __len__(self):
        return len(self.string)
    
    def __repr__(self) :
        tmp = 'PrefixSample('
        tmp += str(self)
        tmp += ')'
        return tmp

    def __str__(self) :
        tmp = str(self.name) + ':\'' + self.labels[0]
        for i in range(0, len(self) ) :
            tmp += self.string[i]
            tmp += self.labels[i+1]
        tmp += '\'' 
        return tmp


class Trie:
    ''' 
    Acyclic automaton for the suffixes of a labeled string, 
    represented by a consistent set of labeled strings.
    '''    
    def __init__(self, sample = None):
        '''
        instance initializer
        '''
        self.members = set()
        self.definition = (self.members)
        if isinstance(sample, PrefixSample) :
            self.members.add(sample)
        else:
            self.members.add(PrefixSample(''))

    def consistency(self, psample):
        '''check for all the members '''
        for path in self.members :
            '''the root label must be shared.'''
            if psample.label(0) != path.label(0) :
                return 0
            for i in range(0, min(len(psample), len(path)) ) :
                if psample.label(i+1) != path.label(i+1) and psample.at(i) == path.at(i) :
                    return 0
        return 1
    
    def addPath(self, prefsample):
        self.members.add(prefsample)
        
    def names(self):
        ''' names of members '''
        names = set()
        for memb in self.members :
            names.add(memb.name)
        return sorted(names, reverse=True)

    def __str__(self):
        stream = "Trie(["
        memberlist = sorted(self.members)
        for i in range(0,len(memberlist)-1) :
            stream += str(memberlist[i])
            stream += ', '
        else:
            stream += str(memberlist[-1])
        stream += ")]"
        return stream

    def __repr__(self):
        stream = "Trie(["
        stream += repr(list(self.members))
        stream += "])"
        return stream
    
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

