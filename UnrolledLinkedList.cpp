//
//  UnrolledLinkedList.cpp
//  BrainfuckInterpreter
//
//  Created by Ulrich Sander on 18.11.18.
//  Copyright © 2018 Richard Förster. All rights reserved.
//

#include "UnrolledLinkedList.hpp"
#include <sstream>

#define _TEMPL template<typename T, byte nodeSize>
#define _ULL UnrolledLinkedList<T, nodeSize>
#define _IMPL(ret) template<typename T, byte nodeSize> ret UnrolledLinkedList<T, nodeSize>::

using namespace std;

// Node struct

_TEMPL UnrolledLinkedList<T, nodeSize>::Node::Node()
:   _prev(nullptr),
    _next(nullptr),
    _data(new T[nodeSize]) {
}

_TEMPL UnrolledLinkedList<T, nodeSize>::Node::Node(Node* prev, Node* next)
:   _prev(prev),
    _next(next),
    _data(new T[nodeSize]) {
        prev -> _next = this;
        next -> _prev = this;
}

_TEMPL inline void UnrolledLinkedList<T, nodeSize>::Node::push(const T& obj) {
    _data[_size++] = obj;
}

_TEMPL inline T& UnrolledLinkedList<T, nodeSize>::Node::peek() const {
    return _data[_size -1];
}

_TEMPL inline T& UnrolledLinkedList<T, nodeSize>::Node::pop() {
    return _data[--_size];
}

_TEMPL void UnrolledLinkedList<T, nodeSize>::Node::insert(byte index, const T& data) {
    if(index >= _size)
        push(data);
    else {
        for(byte i = _size; i > index; i--) {
            _data[i] = _data[i - 1]; // TODO make this better using std::move() or some stuff
        }
        _data[index] = data;
        
        ++_size;
    }
}

_TEMPL T& UnrolledLinkedList<T, nodeSize>::Node::remove(byte index) {
    if(index >= _size) {
        return pop();
    } else {
        T& oldData = _data[index];
        for(byte i = index +1; i < _size; i++) {
            _data[i - 1] = _data[i];
        }
        --_size;
        return oldData;
    }
}

_TEMPL inline T& UnrolledLinkedList<T, nodeSize>::Node::operator [] (byte index) {
    return _data[index];
}

_TEMPL inline const T& UnrolledLinkedList<T, nodeSize>::Node::operator [] (byte index) const {
    return _data[index];
}

// ULL class

_TEMPL typename UnrolledLinkedList<T, nodeSize>::Node* _ULL::appendNewTailNode() {
    Node *newNode = new Node;
    
    if(!_head) {
        _head = _tail = newNode;
    } else if(!_tail -> _next) {
        _tail -> _next = newNode;
        newNode -> _prev = _tail;
        _tail = newNode;
    }
    
    ++_nodeCount;
    return newNode;
}

_TEMPL typename UnrolledLinkedList<T, nodeSize>::Node* UnrolledLinkedList<T, nodeSize>::appendNewHeadNode() {
    Node* newNode = new Node;
    
    if(!_head) {
        _head = _tail = newNode;
    } else if(!_head -> _prev) {
        newNode -> _next = _head;
        _head -> _prev = newNode;
        _tail = newNode;
    }
    
    ++_nodeCount;
    return newNode;
}

_TEMPL void UnrolledLinkedList<T, nodeSize>::deleteNode(Node* node) {
    if(node -> _prev && node -> _next) { // fully unlink the node and link its neighbours
        if(!node -> _prev) {
            unlink(node, node -> _next);
        } else if(!node -> _next) {
            unlink(node -> _prev, node);
        } else {
            link(node -> _prev, node -> _next);
        }
    }
    
    if(_tail == node) _tail = _tail -> _prev; // move the tail and head pointer to a correct position
    if(_head == node) _head = _head -> _next;
    --_nodeCount; // decrease sizes
    _size -= node -> _size;
    delete node;
}

_TEMPL typename UnrolledLinkedList<T, nodeSize>::Node* UnrolledLinkedList<T, nodeSize>::insertNewNodeAt(unsigned int at) {
    if(_nodeCount == 0) {
        return _head = _tail = new Node;
    } else if(at == 0) {
        return appendNewHeadNode();
    } else if(at >= _nodeCount) {
        return appendNewTailNode();
    } else {
        Node* target = nodeAt(at);
        return new Node{target -> prev, target};
    }
    
    ++_nodeCount;
}

_TEMPL typename UnrolledLinkedList<T, nodeSize>::Node* _ULL::nodeAt(unsigned int at) {
    if(isInBounds(0, _nodeCount, at)) {
        Node* current = _head;
        for(unsigned int i = 0; i < at; i++) {
            current = current -> _next;
        }
        return current;
    }
}

_TEMPL typename UnrolledLinkedList<T, nodeSize>::Node* _ULL::nodeAtIndex(unsigned int index) {
    if(isInBounds(0, _size, index)) {
        
        Node* current = _head;
        unsigned int i = index;
        
        while(true) {
            unsigned int check = i - current -> _size;
            
            if(check <= 0)
                return current;
            
            i = check;
        }
    }
}

_TEMPL inline bool _ULL::isFull(Node * node) {
    return node == nullptr || node -> _size == nodeSize;
}

_TEMPL inline bool _ULL::isEmpty(Node * node) {
    return node != nullptr && node -> _size == 0;
}

_TEMPL inline void UnrolledLinkedList<T, nodeSize>::link(Node * left, Node * right) {
    left -> _next = right;
    right -> _prev = left;
}

_TEMPL inline void UnrolledLinkedList<T, nodeSize>::unlink(Node * left, Node * right) {
    left -> _next = right -> _prev = nullptr;
}

// public members

_TEMPL UnrolledLinkedList<T, nodeSize>::UnrolledLinkedList()
:   _head{nullptr},
    _tail(nullptr) {
}

_TEMPL UnrolledLinkedList<T, nodeSize>::UnrolledLinkedList(unsigned int initialCapacity) {
    if(initialCapacity > 0) {
        for(unsigned int i = 0, stop = (initialCapacity / nodeSize) +1; i < stop; i++)
            appendNewTailNode();
    }
}

_TEMPL _ULL::~UnrolledLinkedList<T, nodeSize>() {
    if(_head != nullptr) {
        Node* current = _head -> _prev? _head -> _prev : _head;
        
        while(current != nullptr) {
            Node* temp = current;
            current = current -> _next;
            delete temp;
        }
    }
}

_TEMPL UnrolledLinkedList<T, nodeSize>& UnrolledLinkedList<T, nodeSize>::pushBack (const T& obj) {
    if(isFull(_tail)) {
        Node* appended = _tail -> _next? _tail -> _next : appendNewTailNode(); // use dead tail if possible
        appended -> append(obj);
        _tail = appended;
    } else {
        _tail -> append(obj);
    }
    
    ++_size;
    
    return *this;
}

_TEMPL T& UnrolledLinkedList<T, nodeSize>::peekBack() const {
    if(_size > 0)
        return _tail -> peek();
}

_TEMPL T& UnrolledLinkedList<T, nodeSize>::popBack () {
    if(_size > 0) {
        T& value = _tail -> pop();
        
        if(_tail -> _size == 0) {
            Node* newTail = _tail -> _prev;
            Node* deadTail = _tail -> _next;
            
            if(deadTail) {
                deleteNode(deadTail);
                deadTail = nullptr;
            }
            
            if(newTail) {
                _tail = newTail;
            }
        }
        
        return value;
    }
}

_TEMPL UnrolledLinkedList<T, nodeSize>& _ULL::pushFront(const T& item) {
    if(isFull(_head)) {
        Node* appended = _head -> _prev? _head -> _prev : appendNewHeadNode();
        appended -> insert(0, item);
        _head = appended;
    } else {
        _head -> insert(0, item);
    }
    
    ++_size;
    
    return *this;
}

_TEMPL T& _ULL::peekFront() const {
    if(_size > 0)
        return _head[0];
}

_TEMPL T& UnrolledLinkedList<T, nodeSize>::popFront() {
    if(_size > 0) {
        
    }
}

_TEMPL T& UnrolledLinkedList<T, nodeSize>::operator [] (unsigned int index) {
    if(isInBounds(0, _size, index)) {
        Node* current = _head;
        for(unsigned int i = 0; !(i >= index && i < i + current -> _size); i += current -> _size, current = current -> _next) {
            return current -> operator[](i - index);
        }
    }
}

_TEMPL const T& _ULL::operator [] (unsigned int index) const {
    return operator[] (index);
}

_IMPL(void) clear() {
    if(_head != nullptr) {
        Node * current = _head -> _prev? _head -> _prev : _head;
        
        while(current != nullptr) {
            Node* temp = current;
            current = current -> _next;
            delete temp;
        }
        
        _nodeCount = _size = 0;
        _head = _tail = nullptr;
    }
}

_IMPL(void) bufferedClear() {
    if(_head) {
        Node * keep, * current = _head;
        
        if(_head -> _prev) {
            keep = _head -> _prev;
            unlink(keep, _head);
        } else if(_tail -> _next) {
            keep = _tail -> _next;
            unlink(_tail -> keep);
        }
        
        while(current) {
            Node * temp = current;
            current = current -> _next;
            delete temp;
        }
        
        _size = 0;
        _nodeCount = 1;
        
        _head = _tail = keep;
    }
}

_IMPL(string) toString() const {
    stringstream builder;
    builder << "{";
    
    Node* current = _head;
    unsigned int itemCount = 0;
    
    if(current) {
        if(current -> _prev) {
            builder << "[0], ";
        }
        
        for(; current && current -> _size > 0; current = current -> _next) {
            builder << "[";
            if(itemCount > 0) builder << ", ";
            for(byte b; b < current -> _size; ++b, ++itemCount) {
                if(b != 0) builder << ", ";
                builder << itemCount;
            }
            builder << "]";
        }
        
        if(current && current -> _prev) {
            builder << ", [0]";
        }
    }

    
    builder << "}";
    return builder.str();
}

#undef _IMPL
#undef _TEMPL
#undef _ULL