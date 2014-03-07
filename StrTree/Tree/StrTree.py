
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
            lbl = self.edgedict[labelstr[0]][0]
            cpfx = self.commonprefixlen(labelstr, lbl)
            newstr = labelstr[cpfx:]
            print cpfx, labelstr[:cpfx], newstr
            self.edgedict[labelstr[0]][0] = lbl[:cpfx]
            return
        emptyNode = StrNode()
        self.edgedict[labelstr[0]] = [labelstr, emptyNode]

    def commonprefixlen(self, a, b):
        for l in range(0, min(len(a), len(b)) ) :
            if a[l] != b[l] :
                break
        return l

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
