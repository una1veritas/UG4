'''
Created on 2014/01/20

@author: sin
'''
from automaton import Automaton
from suffixtrie import SuffixTrie


if __name__ == '__main__':
    pass

print "Hello world!"

'''automaton = Automaton("0a1a0b0a1b1a0a0b1b1")'''
automaton = Automaton("aababaabb", "0100110011")
print "automaton's definition is ", automaton.definition, "\n"
automaton.current = automaton.initstate
print "Feeding", "aababa", " to the automaton:\n ", automaton.printOn("")
print "in the state ", automaton.current
print "results in the state ", automaton.transfer("aababa")
print "In another words, a predicate \"the automaton accepts the input\" is ", automaton.accept("aababa")
print

mytrie = SuffixTrie("abracadabra", "011010010111", [0, 3, 5])
print
print "My trie is ", mytrie.printOn(""), "."