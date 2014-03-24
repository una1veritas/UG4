
class StrNode:
    '''
    StrNode class
    
    strlabel
    edgedict
    '''    
    
    def __init__(self, label = None, index = None):
        if label != None :
            self.strlabel = str(label)
        else:
            self.strlabel = None
        if index != None :
            self.index = int(index)
        else:
            self.index = None
        self.edgedict = dict()
        return

    def __repr__(self):
        return self.__str__()
    
    def __str__(self):
        if self.strlabel != None :
            tmp = self.strlabel
            tmp += ','
            tmp += str(self.index)
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
    
    def add(self, string, index):
        if len(string) == 0 :
            self.edgedict[len(self.edgedict)] = StrNode('$'+str(index))
            return
        if self.edgedict.has_key(string[0]) :
            child = self.edgedict[string[0]]
            commprefixend = self.commonprefixlen(child.label(), string)
            if commprefixend == len(child.label()) :
                return child.add(string[commprefixend:], index)
            print 'add to child =', child, ' label =', string, ', with common prefix =', string[:commprefixend]
            newnode = StrNode(string[:commprefixend])
            self.edgedict[string[0]] = newnode
            newnode.add(string[commprefixend:], index)
            print 'newnode =', newnode
            child.strlabel = child.strlabel[commprefixend:]
            newnode.edgedict[child.strlabel[0]] = child
            print 'after:newnode =', newnode
            return
        else:
            self.edgedict[string[0]] = StrNode(string, index)
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
    
    def add(self, astring):
        if isinstance(astring, tuple) :
            self.root.add(str(astring[0]), astring[1])
        else:
            self.root.add(str(astring), 0)
        return
