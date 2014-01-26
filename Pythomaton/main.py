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

prefsample = PrefixSample('0b1a1a1b1a1b0a0a1a1b0b1a1b0a0a1a1b0a0b0a1', None)
print 'inline representation of a prefixsample: ', prefsample, '\n'
'''
for i in range(0,len(prefsample)+1) :
    print i,'th prefix = ',prefsample[:i], '\n'
print
'''

prefsample = prefsample[0:min(17,len(prefsample))]
print 'For sample ', prefsample, '.'
forest = list()
forest.append(SuffixTrie(prefsample[len(prefsample)]))
print "at the first, ", forest , '.'
print
for suffindex in reversed(range(0, len(prefsample))) : 
    print suffindex, ': ', prefsample[suffindex:]
    for atrie in forest :
        if atrie.add(prefsample[suffindex:]) :
            break;
    else:
        forest.append(SuffixTrie(prefsample[suffindex:]))
    tmp = ''
    for atrie in forest :
        tmp += str(atrie.names()) + ',  '
    print tmp +'\n'
print
for trie in forest :
    print trie.names()
    
else:
    print 'fin.'
