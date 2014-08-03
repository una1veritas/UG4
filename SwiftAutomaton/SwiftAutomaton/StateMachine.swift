//
//  StateMachine.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/08/02.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation

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
            defineFinalState(0)
        }
        for var i = 0; i < countElements(sequence); ++i {
            define(i, via: sequence[i], dest: i+1)
            if labels[i+1] == "1" {
                defineFinalState(i+1)
            }
        }
        
    }
    
    //    func prefixExample(sequence: String, labels: String, length: Int) -> (String, Bool) {
    //       return (sequence[0,length], (labels[length] == "1" ? true : false) )
    //    }
    
    func defineDiagramBy(seq: String, labels: String) -> Bool {
        let msg : Bool = false
        
        // check correspondence between an example string and class label string
        if countElements(seq)+1 != countElements(labels) {
            return false
        }
        
        var current : Int
        var nextIndex : Int
        var lastChar : Character
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
        while exlen <= countElements(seq) {
            //            prefix = sequence[0, exlen - 1]
            lastChar = seq[exlen-1]
            lastLabel = (labels[exlen] == "1")
            
            var consistent : Bool? // as a tristate variable
            if let next = transfer(current, char: lastChar) {
                // transfer has already been defined.
                nextIndex = id(next)
                consistent = (lastLabel == accepting(next) )
            } else {
                if searchProbe.peek()?.0 == exlen {
                    // we are here that the last decision should be purged
                    // the state which has been tried last
                    // must be current == searchProbe.peek()!.1
                    // no need to remove the same thing twice: self.undefine(current, via: lastChar, dest: searchProbe.peek()!.2)
                    nextIndex = id(searchProbe.peek()!.2)+1
                    // just set a candidate next state (possibly not exists)
                    searchProbe.pop()
                    // the previous trial is removed.
                    if msg { println("updated probe: \(searchProbe).") }
                } else {
                    // if searchProbe.isEmpty() || searchProbe.peek().0 != exlen {
                    // new transition should be defined.
                    // try from the oldest to newer
                    nextIndex = states.startIndex
                }
                consistent = nil
            }
            
            
            /*if consistent {
                // go along with the existing definition
            } else */
            if consistent == nil { //!transferIsDefined(current, char: lastChar) {
                // try existing states.
                for ; nextIndex < countElements(states) ; ++nextIndex {
                    if lastLabel == accepting(states[nextIndex]) { break }
                }
                if nextIndex < countElements(states) {
                    if msg { println("\(exlen) Adding a new transition btw. existing states.") }
                    define(current, via: lastChar, dest: states[nextIndex]) // by prefix + lastChar
                } else {
                    // simply add a new transition to a newly created states.
                    if msg { println("Needs a new transition and a new state \(exlen).") }
                    states.append(exlen) // exlen be the name of new state (because always larger than or equal to the number of all the states)
                    nextIndex = id(exlen)
                    define(current, via: lastChar, dest: exlen) // by prefix + lastChar
                    if lastLabel { defineFinalState(exlen) }
                    //let triple = (exlen, current, states[nextIndex])
                }
                let triple = (exlen, current, states[nextIndex])
                searchProbe.push(triple)
                // current = states[nextIndex]
                // ++exlen
            } else if !(consistent!) { //!consistent {
                // transferIsDefined(current, char: lastChar) && not consistent
                // We are here because a contradiction with the transition has been found.
                print("\npurge on \(exlen), ")
                    let triple = searchProbe.peek()!
                exlen = triple.0
                current = triple.1
                self.undefine(current, via: seq[exlen - 1], dest: triple.2)
                if msg { println("Encountered contradiction!! Purge and back to \(exlen).") }
                print("\(current) -\(lastChar)-> \(triple.2); back to \(exlen)")
                continue
            }
            current = states[nextIndex]
            ++exlen
            print(".")
            
            if msg { println("Probe \(searchProbe)") }
            if msg { println("Machine \(self)") }
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
    
    func transfer(current: Int, seq: String) -> Int? {
        var i :Int
        var tstate : Int = current
        for i = 0; i < countElements(seq) ; ++i {
            if let next = transfunc[StateChar(state: tstate, char: seq[i])] {
                tstate = next
                continue;
            } else {
                break
            }
        }
        if i < countElements(seq) {
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
