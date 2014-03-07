
class StrNode:
    '''
    StrNode class
    
    strlabel
    edgedict
    '''    
    
    def __init__(self, label = None):
        if label != None :
            self.strlabel = str(label)
        else:
            self.strlabel = None
        self.edgedict = dict()
        return

    def __repr__(self):
        return self.__str__()
    
    def __str__(self):
        if self.strlabel != None :
            tmp = self.strlabel
            tmp += '('
        else:
            tmp = '('
        if len(self.edgedict.items()) > 0 :
            for eachval in self.edgedict.values():
                tmp += str(eachval)
                tmp += ', '
        tmp += ')'
        return tmp

    def label(self):
        return self.strlabel
    
    def add(self, labelstr):
        if len(labelstr) == 0 :
            self.edgedict[0] = StrNode(0)
            return
        if self.edgedict.has_key(labelstr[0]) :
            brother = self.edgedict[labelstr[0]]
            commprefixend = self.commonprefixlen(brother.label(), labelstr)
            print brother.label(), labelstr
            if commprefixend == 0 :
                print 'commpn prefix end == 0', brother.label(), labelstr
                return
            brother.add(brother.label()[commprefixend:])
            brother.strlabel = brother.label()[:commprefixend]
            brother.add(labelstr[commprefixend:])
            print 'done for ', labelstr[:commprefixend]
            return
        else:
            self.edgedict[labelstr[0]] = StrNode(labelstr)
            return

    def commonprefixlen(self, a, b):
        maxlen = min(len(a), len(b))
        l = 0
        while l < maxlen :
            if a[l] != b[l] :
                break
            l = l+1
        return l

class StrTree:
    '''
    Class Tree
    '''
    
    def __init__(self):
        self.root = StrNode()
        return
    
    def __str__(self):
        tmp = 'StrTree'
        tmp += str(self.root)
        return tmp
    
    def add(self, aString):
        self.root.add(str(aString))
        return
