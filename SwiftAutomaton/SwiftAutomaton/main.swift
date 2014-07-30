//
//  main.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/07/23.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation

println("Stack Test.")

class Stack<T> : Printable {
    var elements = [T]()
    
    init() {
    }
    
    func clear() {
        elements.removeAll()
    }
    
    func isEmpty() -> Bool {
        return countElements(elements) == 0
    }
    
    func push(item : T) {
        elements.append(item)
    }
    
    func pop() -> T {
        return elements.removeLast()
    }
    
    func peek() -> T {
        return elements[elements.endIndex-1]
    }

    var count : Int {
    get { return countElements(elements) }
    }
    //
    
    var description: String {
        var str: String = "Stack<T> ("
        for var i: Int = 0; i < count ; i++ {
            str +=  "\(elements[i])"
            if i+1 < count {
                str += ", "
            }
        }
        str += ")"
        return str
    }

}


println("It's just only the begginig...")


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
        if find(states, dept) == nil {
            states.append(dept)
        }
        if find(states, dest) == nil {
            states.append(dest)
        }
        if find(alphabet, via) == nil {
            alphabet.append(via)
        }
        transfunc[StateChar(state: dept, char: via)] = dest
    }
    
    func defineFinalState(state: Int) {
        if find(states, state) == nil {
            self.states.append(state)
        }
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

    func prefixExample(sequence: String, labels: String, length: Int) -> (String, Bool) {
        return (sequence[0,length], (labels[length] == "1" ? true : false) )
    }
    
    func defineShrinkedDiagram(sequence: String, labels: String) -> Bool? {
        
        if countElements(sequence)+1 != countElements(labels) {
            return nil
        }
        //
        if ( labels[0] == "1" ) {
            defineFinalState(initial)
        }
        println("label for empty string is \(labels[0]).\r\n")
        
        var currs : Int = self.initial
        var prevs : Int
        var lastchar : Character
        var probe = Stack<(Int,Int,Character,Bool)>()
        let tquad : (Int, Int, Character, Bool) = (0, self.initial, "\0", false)
        probe.push(tquad)
        while probe.count < countElements(sequence) {
            var lastquad = probe.peek()
            prevs = lastquad.0
            currs = lastquad.1
            lastchar = sequence[probe.count-1]
            println("currs, lastchar = \(currs), \(lastchar)")
            println("\(self.transfunc)")
            if let next = transfer(currs, char: lastchar) {
                let quad = (currs, next, lastchar, false)
                probe.push(quad)
                println("next --- \(next)")
            } else {
                println("undefined.")
                
                if true {
                    // if challenge another
                    // pop
                    let quad = (currs, 0, lastchar, true)
                    probe.push(quad)
                    define(quad.0, via: quad.2, dest: quad.1)
                } else {
                    // if creating a new state
                    let quad = (currs, maxElement(states)+1, lastchar, true)
                    probe.push(quad)
                    define(quad.0, via: quad.2, dest: quad.1)
                }
            }
            println(probe)

        }
        /*
        while !probe.isempty() && probe.count < countElements(sequence) {
            let i = probe.count
            let (lastchar, lastlabel) = (sequence[i], labels[i+1] == "1")
            println("an example (\(lastchar), \(lastlabel)).")
            if let tnexts : Int = transfer(currs, char: lastchar) {
                nexts = tnexts
                if ( lastlabel == accepting(nexts) ) {
                    println("OK. ")
                    let pair = ( currs, false )
                    probe.push(pair)
                    //
                    currs = nexts
                } else {
                    println("Contradiction! ")
                    // purge and back to the last decision
                    while let pair : (Int, Bool) = probe.peek() {
                        currs = pair.0
                        if pair.1 == true {
                            break
                        }
                        probe.pop()
                    }
                    continue
                }
            } else {
                println("Undefined transition encountered.")
                
                // trying a loop
                let pair : (Int, Bool) = probe.pop()
                
                
                // create a new state
                println("creating new state.")
                let pair : (Int, Bool) = (currs, true)
                probe.push(pair)
                nexts = i+1
                define(currs, via: lastchar, dest: nexts)
                if lastlabel { defineFinalState(nexts) }
                currs = nexts
                println(self.transfunc)
                
            }
            println(probe)
        }
*/
        probe.clear()
        // for each example str[0..i] and flag[0..i+1]
            //for each trial
            // determine the best among
            // revisiting old states and the new state (i+1)
        return true
    }
    
    func transfer(current : Int, char : Character) -> Int? {
        if let next = transfunc[StateChar(state: current, char: char)] {
            return next
        }
        return nil
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

m.defineShrinkedDiagram(seq, labels: lab)

println(m)

var index: Int = 0
var current = m.initial
while index < countElements(seq) {
    print("State \(String(current)) to ")
    m.transfer(current, char: seq[index])
    print("\(current), ")
    if m.accepting(current) {
        println("accept.")
    } else {
        println("reject.")
    }
    index++

}

println("Transition ended.")
