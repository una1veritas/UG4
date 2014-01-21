'''
Created on 2014/01/20

@author: sin
'''

from automaton import Automaton
from suffixtrie import SuffixTrie

if __name__ == '__main__':
    pass

print "Hello world!"

master = "aababaabb"
labels = "0100110011"
transseq = "0a1a0b0a1b1a0a0b1b1"

'''automaton = Automaton(sequence)'''
automaton = Automaton(master, labels)
print "automaton's definition is ", automaton.definition, "\n"
automaton.current = automaton.initstate
print "Feeding", master, " to the automaton:\n ", automaton
print "in the state ", automaton.current
print "results in the state ", automaton.transfer(master[:5])
print "In another words, a predicate \"the automaton accepts the input\" is ", automaton.accept(master[:5])
print

tries = list()
indices = set(range(len(master), 0, -1))
while len(indices) > 0 :
    sortedindices = list(indices)
    sortedindices.sort(reverse=True)
    print sortedindices
    mytrie = SuffixTrie()
    index = sortedindices[0]
    indices.remove(index)
    sortedindices.pop(0)
    mytrie.add(master[index:], labels[index], index)
    for index in sortedindices :
        print index, " in ", sortedindices
        if mytrie.add(master[index:], labels[index:], index) :
            indices.remove(index)
    tries.append(mytrie)
    print mytrie
    ''' to the next iteration '''    
print
print "My tries are ", tries, "."

print repr([1,2,3,6])
