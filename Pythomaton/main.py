'''
Created on 2014/01/20

@author: sin
'''

from automaton.PrefixAutomaton import *

if __name__ == '__main__':
    pass

print "Hello world!"

master = "babbbaabababb"
labels = "00011100001001"
transseq = "0b0a0b1b1b1a0a0b0a0b1a0b0b1"

'''automaton = Automaton(sequence)'''
automaton = Automaton(master, labels)
print "automaton's definition is ", automaton.definition, "\n"
automaton.current = automaton.initstate
print "Feeding", master, " to the automaton:\n ", automaton
print "in the state ", automaton.current
print "results in the state ", automaton.transfer(master[:5])
print "In another words, a predicate \"the automaton accepts the input\" is ", automaton.accept(master[:5])
print

<<<<<<< HEAD
'''
prefsample = PrefixSample('0b1a1a1b1a1b0a0a1a1b0b1a1b0a0a1a1b0a0b0a1', None)
'''
prefsample = PrefixSample('0a0b0b0a1b0b0a0a1b0a0b0b1b0a0b0a0a1a1b0b0a0')
print 'inline representation of a prefixsample: ', prefsample, '\n'
print len(prefsample)
'''
for i in range(0,len(prefsample)+1) :
    print i,'th prefix = ',prefsample[:i], '\n'
print
'''
samplelen = 21
prefsample = prefsample[0:min(samplelen,len(prefsample))]
print 'For sample ', prefsample, '.'
forest = list()
forest.append(Trie(prefsample[len(prefsample)]))
print "at the first, ", forest , '.'
print
for suffindex in reversed(range(0, len(prefsample))) : 
    print prefsample[suffindex:]
    for atrie in forest :
        if atrie.add(prefsample[suffindex:]) :
            break;
    else:
        forest.append(Trie(prefsample[suffindex:]))
    tmp = ''
    for atrie in forest :
        print atrie, ', '
        '''
        tmp += str(atrie.names()) + ',  '
        '''
    print
print
print prefsample
for trie in forest :
    sample = (sorted(trie.members, reverse=True))[0]
    print sample.label(0), trie.names()
else:
    print 'fin.'
=======
print "\"abdckgb\"[:3] is ", "abdckgb"[:3]
print "My tries: \n"

forest = list()
print "for ", master, " with labels ", labels
for sufindex in range(len(master), 0, -1) :
    '''sortedindices = list(indices)
    sortedindices.sort(reverse=True)
    '''
    mytrie = SuffixTrie()
    '''
    index = sortedindices[0]
    '''
    mytrie.add(master[sufindex:], labels[sufindex:], sufindex)
    '''
    print index, " added as origin path. ", mytrie
    addedindices = set()
    addedindices.add(index)
    for index in sortedindices :
        if index in addedindices :
            continue
        if mytrie.add(master[index:], labels[index:], index) :
            print index, " added.", mytrie
            addedindices.add(index)
    '''
    print "\n", mytrie, "\n"
    forest.append(mytrie)
    '''print "\nAs a forest: \n", forest '''
    '''
    print indices, ", ", addedindices
    for elem in addedindices:
        indices.remove(elem)
        '''
    ''' to the next iteration '''    

>>>>>>> origin/@work
