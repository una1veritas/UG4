//
//  main.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/07/23.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation

println("It's just only the begginig...")

class StateMachine {

    class StateChar : Equatable, Hashable, Printable {
        var state : Int
        var char  : Character
        let dontcare : Character = "*"
        
        init(state: Int, char: Character) {
            self.state = state
            self.char = char
        }
        
        init(state: Int) {
            self.state = state
            char = dontcare
        }
        
        var hashValue: Int {
        let codes = String(char).unicodeScalars
            let code = codes[codes.startIndex].value
            return (state<<8) + Int(code)
        }
        
        var description: String {
        return "StateChar: (\(state), \(char))"
        }
    }
    
    
    var alphabet : [Character]
    var states : [Int]
    var transfer : [StateChar: Int]
    
    init(alphabet: [Character], states: [Int]) {
        self.states = states
        self.alphabet = alphabet
        self.transfer = Dictionary<StateChar, Int>()
    }
    
    func define(dept: Int, via: Character, dest: Int) {
        transfer[StateChar(state: dept, char: via)] = dest
    }
    
}

func == (lhs: StateMachine.StateChar, rhs: StateMachine.StateChar) -> Bool {
    return (lhs.state == rhs.state) && (lhs.char == rhs.char)
}


var m = StateMachine(alphabet: ["a", "b"], states: [0, 1, 2, 3])

m.define(0, via: "a", dest: 0)
m.define(0, via: "b", dest: 1)

var q: Int = 0
var index: Int = 0
var str : String = "abbabab"

while let t = m.transfer[StateMachine.StateChar(state: q, char: str[advance(str.startIndex, index)])] {
    print("State ")
    print(StateMachine.StateChar(state: q, char: str[advance(str.startIndex, index)]))
    print(" to ")
    q = t
    print(q)
    println(".")
    index++

}

println("Transition ended.")