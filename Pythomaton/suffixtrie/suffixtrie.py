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
            orgindex = elem[2]
            print "compare ", (tstr, tlabs), ", ", (astring, labels)
            if tlabs[0] != labels[0] :
                conflag = False
                print "failed at head. "
                break
            for sufflen in range(1, min(len(tstr), len(astring)) ) :
                if astring[sufflen-1] != tstr[sufflen-1] :
                    ''' Branching off. Check the next string. '''
                    break
                if labels[sufflen] != tlabs[sufflen] :
                    ''' Labels are conflicting. This is non-consistent addition. '''
                    conflag = False
                    print "failed at ", orgindex , " by contradiction betw. '",astring[:sufflen-1], "': ", labels[:sufflen], ", '", tstr[:sufflen-1], "': ", tlabs[:sufflen] , " at label ", sufflen
                    break
            if not(conflag) :
                break
        else :
            ''' if loop exited by exhaustion, then it is consistent with all the members. '''
            self.members.add((astring, labels, idnum))
            return True
        return False

    def __str__(self):
        stream = "SuffixTrie("
        stream += repr(list(self.members))
        stream += ")"
        return stream

    def __repr__(self):
        return str(self)