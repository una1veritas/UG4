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
    
    var initial : Int {
    get { return states[0] }
    set { states[0] = newValue}
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
    
    func id(state: Int) -> Int {
        for var i = 0; i < countElements(states); ++i {
            if states[i] == state {
                return i
            }
        }
        return -1
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
        
        println("DEBUG: after set the initial label: \n\(self).\n")
        
        var current : Int
        var next : Int
        var prefix : String
        var lastChar : Character
        var prefixLabel : Bool
        var lastLabel : Bool
        var searchProbe = Stack<(Int, Int, Int)>()
        var exlen : Int
        
        //    setting the label for the initial state.
        if ( labels[0] == "1" ) {
            defineFinalState(initial)
        } // else the initial state is not final state.

        current = initial
        exlen = 1
        // start from the initial state with the prefix of length 1
        while exlen <= countElements(sequence) {
            prefix = sequence[0, exlen - 1]
            lastChar = sequence[exlen-1]
            lastLabel = (labels[exlen] == "1")

            var consistency : Bool = false
            if transferIsDefined(current, char: lastChar) {
                next = transfer(current, char: lastChar)!
                consistency = (lastLabel == accepting(next) )
            } else {
                if searchProbe.isEmpty() || searchProbe.peek().0 != exlen {
                    next = initial
                } else {
                    next = searchProbe.peek().2 // this is the last state tried out
                    self.undefine(current, via: lastChar, dest: next)
                    next = states[id(next)+1]
                    searchProbe.pop()
                }
            }
            
            if consistency {
                print("transition has already been defined. ")
                println("and, it's fine, no contradiction.")
                println()
                // println("a1) probe = \(searchProbe).")
                // going down with the definition
                current = next
                ++exlen
            } else {
                if transferIsDefined(current, char: lastChar) {
                    // We are here because a contradiction has been occurred.
                    println("Mmm, it's a contradiction!!")
                    println("Recall the last decision! Roll back the last decisions!!")
                    println()
                    println("b) probe = \(searchProbe).")
                    let triple = searchProbe.peek()
                    exlen = triple.0
                    current = triple.1
                    next = triple.2
                    lastChar = sequence[exlen - 1]
                    self.undefine(current, via: lastChar, dest: next)
                    // next = states[id(next)+1] next candidate will be advanced in the next iteration.
                    println("b) probe = \(searchProbe).")
                    continue
                }
                print("requires a new transition. ")
                // tries a new transition to the existing states.
                var i = id(next)
                for ; i < countElements(states); ++i {
                    if lastLabel == accepting(states[i]){
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
                    let triple = (exlen, current, next)
                    searchProbe.push(triple)
                } else {
                    // simply add a new transition to a newly created states.
                    states.append(exlen)
                    next = exlen
                    println("Now we need a new transition and a new state \(next). \n")
                    define(current, via: lastChar, dest: next) // by prefix + lastChar
                    if lastLabel { defineFinalState(next) }
                    let triple = (exlen, current, next)
                    searchProbe.push(triple)
                }
                println("probe = \(searchProbe).")
                current = next
                ++exlen
            }
            println("Stack contents \(searchProbe)")
            println("Machine \(self)")
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

println("\nFinished the execution.\n")
println(m)


