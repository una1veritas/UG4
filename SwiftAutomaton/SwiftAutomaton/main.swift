//
//  main.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/07/23.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation

println("It's just only the begginig...")

class StateChar : Equatable, Hashable {
    var state : Int
    var char  : String
    
    init(state: Int, char: String) {
        self.state = state
        self.char = char
    }

    init(state: Int) {
        self.state = state
        char = "*"
    }

    var hashValue: Int {
        let codes = char.unicodeScalars
        let code = codes[codes.startIndex].value
        return (state<<8) + Int(code)
    }

}


func == (lhs: StateChar, rhs: StateChar) -> Bool {
    return (lhs.state == rhs.state) && (lhs.char == rhs.char)
}

var transition = [ StateChar: Int ]()

    transition[StateChar(state: 0, char: "*")] = 5
    transition[StateChar(state: 1)] = 3
    transition[StateChar(state: 2)] = 1
    transition[StateChar(state: 3)] = 4
    transition[StateChar(state: 4)] = 6
    transition[StateChar(state: 5)] = 2
    transition[StateChar(state: 6)] = nil

var q: Int = 0
while (transition[StateChar(state: q)] != nil) {
    print("State " + String(q) + " to ")
    if let next = transition[StateChar(state: q)] {
        q = next
        println(String(q) + ".")
    }
}

println("Transition ended.")