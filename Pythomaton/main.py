'''
Created on 2014/01/20

@author: sin
'''

import sys
from automaton.PrefixAutomaton import *

if __name__ == '__main__':
    pass

print "Hello world!"
if len(sys.argv) > 1 :
    prefsample = PrefixSample(sys.argv[1])
    if len(sys.argv) > 2 :
        samplelen = int(sys.argv[2])
    else:
        samplelen = len(prefsample)
else:
    prefsample = PrefixSample('0a0b0b0a1b0b0a0a1b0a0b0b1b0a0b0a0a1a1b0b0a0')
    samplelen = len(prefsample)

print 'inline representation of a prefixsample: ', prefsample
print 'length ', samplelen

'''
master = "babbbaabababb"
labels = "00011100001001"
transseq = "0b0a0b1b1b1a0a0b0a0b1a0b0b1"

automaton = Automaton(sequence)
automaton = Automaton(master, labels)
print "automaton's definition is ", automaton.definition, "\n"
automaton.current = automaton.initstate
print "Feeding", master, " to the automaton:\n ", automaton
print "in the state ", automaton.current
print "results in the state ", automaton.transfer(master[:5])
print "In another words, a predicate \"the automaton accepts the input\" is ", automaton.accept(master[:5])
print
'''


'''
for i in range(0,len(prefsample)+1) :
    print i,'th prefix = ',prefsample[:i], '\n'
print
'''

if samplelen > 0 :
    prefsample = prefsample[:samplelen]
else:
    prefsample = prefsample[samplelen:]
print 'For sample ', prefsample, '.'
forest = list()
forest.append(Trie(prefsample[len(prefsample)]))
print "at the first, ", forest , '.'
print
for suffindex in reversed(range(0, len(prefsample))) : 
    print prefsample[suffindex:]
    maxtrie = None
    for atrie in forest :
        consval = atrie.consistency(prefsample[suffindex:])
        if consval :
            maxtrie = atrie
            break
    if maxtrie != None:
        maxtrie.addPath(prefsample[suffindex:])
    else:
        forest.append(Trie(prefsample[suffindex:]))
    print 'Forest: '
    for atrie in forest :
        print atrie, ', '
    print
print
print prefsample
for trie in forest :
    sample = (sorted(trie.members, reverse=True))[0]
    print sample.label(0), trie.names()
else:
    print 'fin.'

