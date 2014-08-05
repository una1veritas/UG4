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

    subscript (val1: Int, val2: Int) -> String {
            return self.bridgeToObjectiveC().substringWithRange(NSMakeRange(val1, val2))
    }
}




var seq  : String =  "aaaaaaaaaaaaaa"
var lab : String = "011000110011001"

var m0 = StateMachine(alphabet: ["a"])

println("example = \(seq), \(lab).")

/*
m0.defineDiagramBy(seq, labels: lab)
println("State machine:")
println(m0)
println()
*/
seq  =  "babbbaabababb"
lab  = "00011100001001"

var m1 = StateMachine(alphabet: ["a", "b"])

println("example = \(seq), \(lab).")

m1.defineDiagramBy(seq, labels: lab)
println("State machine:")
println(m1)
println()

println("Finished the execution.\n")


