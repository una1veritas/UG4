'''
Created on 2014/01/20

@author: sin
'''

class SuffixTrie:
    ''' 
    Trie like structure for the suffixes of a labeled string, 
    represented by a consistent set of labeled strings.
    '''    
    def __init__(self):
        '''
        instance initializer
        '''
        self.members = set()
        self.definition = (self.members)
        
    def add(self, astring, labels, idnum):
        if len(astring) + 1 != len(labels) :
            return False
        conflag = True
        for elem in self.members :
            tstr = elem[0]
            tlabs = elem[1]
            if tlabs[0] != labels[0] :
                conflag = False
                print "failed at head. "
                break
            for index in range(0, min(len(tstr), len(astring)) ) :
                if astring[index] != tstr[index] :
                    ''' Branching off. Check the next string. '''
                    break
                if labels[index+1] != tlabs[index+1] :
                    ''' Labels are conflicting. This is non-consistent addition. '''
                    conflag = False
                    print "failed by confliction at label ", index+1
                    break
            if not(conflag) :
                break
        else :
            ''' if loop exited by exhaustion, then it is consistent with all the members. '''
            self.members.add((astring, labels, idnum))
            return True
        return False

    def __str__(self):
        stream = "SuffixTrie(set("
        stream += repr(list(self.members))
        stream += "])"
        return stream

    def __repr__(self):
        return str(self)