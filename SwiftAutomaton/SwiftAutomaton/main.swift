//
//  main.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/07/23.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation

println("It's just only the begginig...")

class StateMachine : Printable {

    let initialState : Int = 0

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
            let codeArray = String(char).unicodeScalars
            let code = codeArray[codeArray.startIndex].value
            return (state<<8) + Int(code)
        }
        
        var description: String {
            return "(\(state), \(char))"
        }
    }
    
    
    var alphabet : [Character]
    var states : [Int]
    var transfer : [StateChar: Int]
    var finalStates : [Int]
    var currentState : Int
    
    init(alphabet: [Character]) {
        self.states = [ 0 ]
        self.alphabet = alphabet
        self.transfer = Dictionary<StateChar, Int>()
        self.finalStates = []
        
        currentState = self.states[self.states.startIndex]
    }
    
    init(alphabet: [Character], states: [Int]) {
        self.states = states
        self.alphabet = alphabet
        self.transfer = Dictionary<StateChar, Int>()
        self.finalStates = []
        
        currentState = self.states[self.states.startIndex]
    }
    
    func define(dept: Int, via: Character, dest: Int) {
        if find(states, dept) == nil {
            states.append(dept)
        }
        if find(states, dest) == nil {
            states.append(dest)
        }
        if find(alphabet, via) == nil {
            alphabet.append(via)
        }
        transfer[StateChar(state: dept, char: via)] = dest
    }
    
    func acceptingState(state: Int) {
        if find(states, state) == nil {
            self.states.append(state)
        }
        if find(finalStates, state) == nil {
            self.finalStates.append(state)
        }
        
    }
    
    func trans(dept: Int, via: Character) -> Int {
        if let dst = transfer[StateChar(state: dept, char: via)] {
            return dst
        }
        return -1
    }
    
    func read(char: Character) -> Bool {
        var next : Int = trans(currentState, via: char)
        if next != -1 {
            currentState = next
            return true
        }
        return false
    }
    
    var description: String {
    return "StateMachine(alphabet: \(alphabet), states: \(states), transfer: \(transfer), finalStates: \(finalStates))"
    }
    
    var current : Int {
    get { return currentState }
    set { currentState = newValue }
    }
}

func == (lhs: StateMachine.StateChar, rhs: StateMachine.StateChar) -> Bool {
    return (lhs.state == rhs.state) && (lhs.char == rhs.char)
}


var m = StateMachine(alphabet: ["a", "b"])


var q: Int = 0
var index: Int = 0
var str : String = "abbabab"

m.define(0, via: "a", dest: 1)
m.define(1, via: "b", dest: 2)
m.define(2, via: "b", dest: 3)
m.define(3, via: "a", dest: 4)
m.define(4, via: "b", dest: 5)
m.define(5, via: "a", dest: 6)
m.define(6, via: "b", dest: 7)

m.acceptingState(7)

println(m)

while index < countElements(str) {
    print("State ")
    print(String(m.current) )
    print(" to ")
    m.read(str[advance(str.startIndex, index)])
    print(String(m.current) )
    println(".")
    index++

}

println("Transition ended.")
