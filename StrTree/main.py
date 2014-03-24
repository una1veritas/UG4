from Tree.StrTree import *

print 'Hi.'
string = 'abbabbaab'

tree = StrTree()
for index in range(0, len(string)) :
    suffix = string[index:]
    tree.add( (suffix, index) )
    print tree

print 'Final result:'
print tree
print
