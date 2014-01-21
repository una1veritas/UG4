'''
Created on 2014/01/20

@author: sin
'''

class SuffixTrie:
    ''' 
    Trie like structure for the suffixes of a labeled string, 
    represented by a consistent set of labeled strings.
    '''
    master = ""
    labels = ""
    members = set()
    definition = (master, labels, members)
    
    def __init__(self, master, classlabels, indexes):
        '''
        instance initializer
        '''
        if len(master) == 0 :
            return
        '''ensures samples is a ordered list '''
        ''' the first gives the primary path '''
        self.master = str(master)
        self.labels = str(classlabels)
        tempsuffixes = list()
        for index in indexes :
            tmpstr = self.master[index:]
            tmplabel = self.labels[index:]
            flag = True
            tempsuffixes.append(tmpstr)
            print "suffix = ", tmpstr, ", set = ", tempsuffixes
            self.members.add(index)
        return

    def printOn(self, stream):
        stream += "SuffixTrie("
        stream += "master("
        stream += self.master
        stream += "), "
        stream += "labels("
        stream += self.labels
        stream += "), "
        stream += "member("
        for i in self.members :
            stream += str(i)
            stream += ", "
        stream += ") "
        stream += ")"
        return stream
