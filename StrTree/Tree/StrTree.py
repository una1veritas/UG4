
class StrNode:
    
    def __init__(self):
        self.edgedict = dict()
        return

    def __repr__(self):
        return self.__str__()
    
    def __str__(self):
        tmp = 'StrNode('
        if len(self.edgedict.items()) > 0 :
            tmp += str(self.edgedict)
        tmp += ')'
        return tmp

    def add(self, labelstr):
        if self.edgedict.has_key(labelstr[0]) :
            print self.edgedict[labelstr[0]][0]
            return
        emptyNode = StrNode()
        self.edgedict[labelstr[0]] = [labelstr, emptyNode]

class StrTree:
    '''
    Class Tree
    '''
    
    def __init__(self):
        self.root = StrNode()
        return
    
    def __str__(self):
        return str(self.root)
    
    def add(self, aString):
        self.root.add(str(aString))
        return
