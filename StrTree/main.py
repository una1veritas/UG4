from Tree.StrTree import *

print 'Hi.'
<<<<<<< HEAD
string = 'aaba'

tree = StrTree()
tree.add( (string[:1], 1) )
tree.add( (string[:2], 2) )
#for i in range(len(string),0,-1):
#    print string[(i-1):]
#    tree.add( (string[(i-1):], i-1) )
=======
string = 'abbabbaab'

tree = StrTree()
for index in range(0, len(string)) :
    suffix = string[index:]
    tree.add( (suffix, index) )
    print tree

print 'Final result:'
>>>>>>> @workplace
print tree
print
