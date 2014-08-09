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
        var range = Range<String.Index>(start: advance(self.startIndex, from), end: advance(self.startIndex, before))
        return self.substringWithRange(range)
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

var seq  =  "babbbaabababb"
var lab  = "00011100001001"

var m1 = StateMachine(alphabet: seq)

println("example = \(seq), \(lab).")

var achieved = m1.defineDiagramBy(seq, labels: lab, restriction: [1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
println("\nachieved \(achieved)")
println()
println("State machine:")
println(m1)
println()

println("Finished the execution.\n")


