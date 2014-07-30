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
    var transfunc : [StateChar: Int]
    var finalStates : [Int]
    
    var initialState : Int {
    get { return states[self.states.startIndex] }
    set { states[self.states.startIndex] = newValue}
    }
    
    init(alphabet: [Character]) {
        self.states = [ 0 ]
        self.alphabet = alphabet
        self.transfunc = Dictionary<StateChar, Int>()
        self.finalStates = []
    }
    
    init(alphabet: [Character], states: [Int]) {
        self.states = states
        self.alphabet = alphabet
        self.transfunc = Dictionary<StateChar, Int>()
        self.finalStates = []
    }

    func define(dept: Int, via: Character, dest: Int) {
        defineState(dept)
        defineState(dest)
        if find(alphabet, via) == nil {
            alphabet.append(via)
        }
        transfunc[StateChar(state: dept, char: via)] = dest
    }
    
    func undefine(dept: Int, via: Character, dest: Int) {
        transfunc.removeValueForKey(StateChar(state: dept, char: via))
    }
    
    func defineState(state: Int) {
        if find(states, state) == nil {
            states.append(state)
        }
    }
    
    func defineFinalState(state: Int) {
        if find(finalStates, state) == nil {
            self.finalStates.append(state)
        }
        
    }
    
    func accepting(state: Int) -> Bool {
        if find(finalStates, state) == nil {
            return false
        }
        return true
    }
    
    func defineChainDiagram(sequence: String, labels: String) {
        if countElements(sequence)+1 != countElements(labels) {
            return
        }
        //
        if labels[0] == "1" {
            m.defineFinalState(0)
        }
        for var i = 0; i < countElements(sequence); ++i {
            m.define(i, via: sequence[i], dest: i+1)
            if labels[i+1] == "1" {
                m.defineFinalState(i+1)
            }
        }

    }

//    func prefixExample(sequence: String, labels: String, length: Int) -> (String, Bool) {
//       return (sequence[0,length], (labels[length] == "1" ? true : false) )
//    }
    
    func defineDiagramBy(sequence: String, labels: String) -> Bool {
        // check correspondence between an example string and class label string
        if countElements(sequence)+1 != countElements(labels) {
            return false
        }
        
        //    setting the label for the initial state.
        if ( labels[labels.startIndex] == "1" ) {
            defineFinalState(initialState)
        } // else the initial state is not final state.
        
        println("DEBUG: after set the initial label: \n\(self).\n")
        
        var current : Int
        var next : Int
        var prefix : String
        var lastChar : Character
        var prefixLabel : Bool
        var lastLabel : Bool
        var searchProbe = Stack<(Int, Int, Character, Int)>()
        
        var exlength = 1
        while exlength <= countElements(sequence) {
            prefix = sequence[0, exlength - 1]
            lastChar = sequence[exlength-1]
            lastLabel = labels[exlength] == "1" ? true : false
            
            current = self.transfer(initialState, sequence: prefix)!
            next = current // dummy!!
            if transferIsDefined(current, char: lastChar) && (lastLabel == accepting(next)) {
                print("transition has already been defined. ")
                next = transfer(current, char: lastChar)!
                println("and, fine, no contradiction.")
                println()
                ++exlength
            } else {
                var i : Int
                if transferIsDefined(current, char: lastChar) {
                    // We are here because a contradiction has been occurred.
                    println("Mmm, it's a contradiction!!")
                    println("I don't remember the last decision! But you must roll back the last decisions!!")
                    println()
                    let tup = searchProbe.pop()
                    self.undefine(tup.1, via: tup.2, dest: tup.3)
                    exlength = tup.0
                    prefix = sequence[0, exlength - 1]
                    lastChar = sequence[exlength-1]
                    lastLabel = labels[exlength] == "1" ? true : false
                    for i = 0; states[i] != tup.3;  ++i { }
                    ++i
                    next = states[i]
                } else {
                    i = 0
                    next = states[0]
                }
                print("requires a new transition. ")
                // tries a new transition to the existing states.
                for ; i < countElements(states); ++i {
                    if lastLabel == accepting(states[i]){
                        next = states[i]
                        println("Ok it's fine, no contradiction.\n")
                        break
                    } else {
                        println("Mmm, it makes a contradiction. Skip it.")
                        continue
                    }
                }
                if i < countElements(states) {
                    next = states[i]
                    define(current, via: lastChar, dest: next) // by prefix + lastChar
                    let tup = (exlength, current, lastChar, next)
                    searchProbe.push(tup)
                } else {
                    // simply add a new transition to a newly created states.
                    next = maxElement(states) + 1
                    println("Now we need a new transition and a new state \(next). \n")
                    define(current, via: lastChar, dest: next) // by prefix + lastChar
                    let tup = (exlength, current, lastChar, next)
                    searchProbe.push(tup)
                }
                ++exlength
            }
            println("Stack contents \(searchProbe)")
            println("\(current) -> \(prefix):\(lastChar), \(lastLabel) -> \(next),")
            println("Go to the next example of length \(exlength).\n")
        }
        searchProbe.clear()
        
        return true
    }
    
    func transfer(current : Int, char : Character) -> Int? {
        if let next = transfunc[StateChar(state: current, char: char)] {
            return next
        }
        return nil
    }
    
    func transferIsDefined(current : Int, char : Character) -> Bool {
        return transfunc[StateChar(state: current, char: char)] ? true : false
    }
    
    func transfer(current: Int, sequence: String) -> Int? {
        var i :Int
        var tstate : Int = current
        for i = 0; i < countElements(sequence) ; ++i {
            if let next = transfunc[StateChar(state: tstate, char: sequence[i])] {
                tstate = next
                continue;
            } else {
                break
            }
        }
        if i < countElements(sequence) {
            return nil
        }
        return tstate
    }
    
    var description: String {
    return "StateMachine(alphabet: \(alphabet), states: \(states), transfer: \(transfunc), finalStates: \(finalStates))"
    }
    
}

func == (lhs: StateMachine.StateChar, rhs: StateMachine.StateChar) -> Bool {
    return (lhs.state == rhs.state) && (lhs.char == rhs.char)
}

var m = StateMachine(alphabet: ["a", "b"])


var seq  : String =  "abbabab"
var lab : String = "01001101"

println("example = \(seq), \(lab).")
//m.defineChainDiagram(seq, labels: lab)

println(m)
println()

m.defineDiagramBy(seq, labels: lab)

println(m)
