//
//  Stack.swift
//  SwiftAutomaton
//
//  Created by Sin Shimozono on 2014/07/30.
//  Copyright (c) 2014年 Sin Shimozono. All rights reserved.
//

import Foundation


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
    
    func peek() -> T? {
        if elements.endIndex == 0 { return nil }
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

