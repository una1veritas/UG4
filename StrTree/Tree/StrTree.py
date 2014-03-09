
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
        else:
            tmp = ''
        if len(self.edgedict.items()) > 0 :
            tmp += '('
            values = self.edgedict.values()
            tmp += str(values[0])
            for ix in range(1, len(values)) :
                tmp += ', '
                tmp += str(values[ix])
            tmp += ')'
        return tmp

    def label(self):
        return self.strlabel
    
    def add(self, labelstr):
        if len(labelstr) == 0 :
            self.edgedict[len(self.edgedict)] = StrNode('$'+str(len(self.edgedict)))
            return
        if self.edgedict.has_key(labelstr[0]) :
            child = self.edgedict[labelstr[0]]
            commprefixend = self.commonprefixlen(child.label(), labelstr)
            if commprefixend == len(child.label()) :
                return child.add(labelstr[commprefixend:])
            print 'add to child =', child, ' label =', labelstr, ', with common prefix =', labelstr[:commprefixend]
            newnode = StrNode(labelstr[:commprefixend])
            self.edgedict[labelstr[0]] = newnode
            newnode.add(labelstr[commprefixend:])
            print 'newnode =', newnode
            child.strlabel = child.strlabel[commprefixend:]
            newnode.edgedict[child.strlabel[0]] = child
            print 'after:newnode =', newnode
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
