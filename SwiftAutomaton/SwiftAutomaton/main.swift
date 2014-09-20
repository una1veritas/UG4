//
//  main.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/07/23.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation


extension String {

    subscript (i: Int) -> Character {
        return self[advance(self.startIndex, i)]
    }

    subscript (from: Int, before: Int) -> String {
        var range = Range<String.Index>(start: advance(self.startIndex, min(countElements(self), from)),
            end: advance(self.startIndex, min(countElements(self), before) ) )
        return self.substringWithRange(range)
    }
    
    // string[range] -> substring form start pos on the left to end pos on the right
    subscript(range: Range<Int>) -> String {
        return self[range.startIndex, range.endIndex]
    }
    
}



/*
var seq  : String =  "aaaaaaaaaaaaaa"
var lab : String = "011000110011001"

var m0 = StateMachine(alphabet: ["a"])

println("example = \(seq), \(lab).")

m0.defineDiagramBy(seq, labels: lab)
println("State machine:")
println(m0)
println()
*/

var seq  =  "baaababbbaaabba"
var lab  = "0010001000100001"

var m1 = StateMachine(alphabet: seq)

println("example = \(seq), \(lab).")

var achieve = m1.defineDiagramBy(seq, labels: lab, restriction: [1, 1, 2, 3])
if achieve <= countElements(seq) {
    println("\nfailed at \(achieve) of \(countElements(seq)), \(seq[0,achieve])^\(seq[achieve,countElements(seq)])")
} else {
    println("\ncompleted \(achieve) out of \(countElements(seq)), \(seq[0,achieve])^\(seq[achieve,countElements(seq)])")
}
println()
println(m1)
println()

println("Finished the execution.\n")


