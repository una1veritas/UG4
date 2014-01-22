'''
Created on 2014/01/20

@author: sin
'''

from automaton import Automaton
from suffixtrie import SuffixTrie

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

print "\"abdckgb\"[:3] is ", "abdckgb"[:3]
print "My tries: \n"

tries = list()
indices = set(range(len(master), 0, -1))
print "for ", master, " with labels ", labels
while len(indices) > 0 :
    sortedindices = list(indices)
    sortedindices.sort(reverse=True)
    mytrie = SuffixTrie()
    index = sortedindices[0]
    mytrie.add(master[index:], labels[index:], index)
    print index, " added as origin path. ", mytrie
    addedindices = set()
    addedindices.add(index)
    for index in sortedindices :
        if index in addedindices :
            continue
        if mytrie.add(master[index:], labels[index:], index) :
            print index, " added.", mytrie
            addedindices.add(index)
    print mytrie
    tries.append(mytrie)
    print indices, ", ", addedindices
    for elem in addedindices:
        indices.remove(elem)
    ''' to the next iteration '''    

