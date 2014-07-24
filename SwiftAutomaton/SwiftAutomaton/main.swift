//
//  main.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/07/23.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation

println("It's just only the begginig...")


extension String {
    subscript (i: Int) -> Character {
        return self[advance(self.startIndex, i)]
    }
}

extension Character {
    var unicode : Int {
        get {
        let codes = String(self).unicodeScalars
        return Int(codes[codes.startIndex].value)
        }
    }
}

class StateMachine : Printable {

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
            /*
            let codeArray = String(char).unicodeScalars
            let code = codeArray[codeArray.startIndex].value
            */
            return (state<<8) + Int(char.unicode)
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
    
    var current : Int {
    get { return currentState }
    set { currentState = newValue }
    }

    var initial : Int {
    get { return states[self.states.startIndex] }
    set { states[self.states.startIndex] = newValue}
    }

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
    
    func accepting() -> Bool {
        if find(finalStates, currentState) == nil {
            return false
        }
        return true
    }
    
    func defineLinearChain(str: String, flag: String) {
        if countElements(str)+1 != countElements(flag) {
            return
        }
        //
        if flag[0] == "1" {
            m.acceptingState(0)
        }
        for var i = 0; i < countElements(str); ++i {
            m.define(i, via: str[i], dest: i+1)
            if flag[i+1] == "1" {
                m.acceptingState(i+1)
            }
        }

    }
    
    func transit(char: Character) -> Bool {
        if let next = transfer[StateChar(state: current, char: char)] {
            currentState = next
            return true
        }
        return false
    }
    
    var description: String {
    return "StateMachine(alphabet: \(alphabet), states: \(states), transfer: \(transfer), finalStates: \(finalStates))"
    }
    
}

func == (lhs: StateMachine.StateChar, rhs: StateMachine.StateChar) -> Bool {
    return (lhs.state == rhs.state) && (lhs.char == rhs.char)
}

var m = StateMachine(alphabet: ["a", "b"])


var ex  : String =  "abbabab"
var accflags : String = "01001101"

m.defineLinearChain(ex, flag: accflags)

println(m)

var index: Int = 0
m.current = m.initial
while index < countElements(ex) {
    print("State \(String(m.current)) to ")
    m.transit(ex[index])
    print("\(m.current), ")
    if m.accepting() {
        println("accept.")
    } else {
        println("reject.")
    }
    index++

}

println("Transition ended.")
