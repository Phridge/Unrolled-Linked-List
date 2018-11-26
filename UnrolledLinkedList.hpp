//
//  UnrolledLinkedList.hpp
//  UnrolledLinkedList
//
//  Created by Ulrich Sander on 18.11.18.
//  Copyright © 2018 Richard Förster. All rights reserved.
//

#ifndef UnrolledLinkedList_hpp
#define UnrolledLinkedList_hpp

#include <string>
#include <sstream>
#include <algorithm>
#include <utility>

namespace coll {
    
    typedef char byte;
    
    template<typename T, byte nodeSize = 5>
    class UnrolledLinkedList;
    
    const char
    * LIST_EMPTY_MSG = "List is empty",
    * ERROR_MSG = "Internal error",
    * OUT_OF_BOUNDS_MSG = "Index out of bounds";
    
    template <typename B>
    bool isInBounds(B offset, B bounds, B index) {
        return index >= offset && index < offset + bounds;
    }
    
    template<typename T, byte nodeSize = 5>
    std::ostream& operator << (std::ostream& out, const UnrolledLinkedList<T, nodeSize>& target) {
        out << target.toString();
        return out;
    }
    
    template <typename T, byte nodeSize>
    class UnrolledLinkedList {
        
    public:
        
        UnrolledLinkedList();
        UnrolledLinkedList(unsigned int);
        ~UnrolledLinkedList();
        
        T& operator [] (unsigned int);
        const T& operator [] (unsigned int) const;
        
        UnrolledLinkedList& pushBack(const T&);
        T& peekBack() const;
        T popBack();
        UnrolledLinkedList& pushFront(const T&);
        T& peekFront() const;
        T popFront();
        
        unsigned int inline size() const;
        unsigned int inline nodeCount() const;
        void insert(const unsigned int, const T&);
        void resize();
        T remove(unsigned int);
        void clear();
        void bufferedClear();
        
        std::string toString() const;
        
    protected:
        
    private:
        
        struct Node {
            Node* _prev, * _next;
            T* _data;
            byte _size;
            
            Node();
            Node(Node*, Node*);
            
            ~Node() {delete [] _data;}
            void clear();
            
            inline void push(const T&);
            inline T pop();
            inline T& peek() const;
            void insert(byte, const T&);
            T remove(byte);
            
            inline T& operator [] (byte);
            inline const T& operator [] (byte) const;
        };
        
        Node * _head, * _tail, * _cache;
        unsigned int _size, _nodeCount;
        
        Node * popResource();
        void pushResource(Node *);
        inline Node * newNode();
        
        Node* appendNewTailNode();
        Node* appendNewHeadNode();
        Node* insertNewNodeAt(unsigned int);
        
        void deleteNode(Node*);
        Node* nodeAt(unsigned int);
        void elementAt(const unsigned int, Node*&, byte&);
        Node* nodeAtIndex(unsigned int);
        inline bool isFull(Node*);
        inline bool isEmpty(Node*);
        
        inline void link(Node*, Node*);
        inline void unlink(Node*, Node*);
    };
    
    
#define _TEMPL template<typename T, byte nodeSize>
#define _ULL UnrolledLinkedList<T, nodeSize>
#define _IMPL(ret) template<typename T, byte nodeSize> ret UnrolledLinkedList<T, nodeSize>::
    
    // Node struct
    
    _TEMPL _ULL::Node::Node()
    :   _prev(nullptr),
        _next(nullptr),
        _size(0),
        _data(new T[nodeSize]){
    }
    
    _TEMPL _ULL::Node::Node(Node* prev, Node* next)
    :   _prev(prev),
        _next(next),
        _size(0),
        _data(new T[nodeSize]) {
        prev -> _next = this;
        next -> _prev = this;
    }
    
    _IMPL(inline void) Node::push(const T& obj) {
        _data[_size++] = obj;
    }
    
    _TEMPL inline T& UnrolledLinkedList<T, nodeSize>::Node::peek() const {
        return _data[_size -1];
    }
    
    _TEMPL inline T UnrolledLinkedList<T, nodeSize>::Node::pop() {
        return _data[--_size];
    }
    
    _TEMPL void UnrolledLinkedList<T, nodeSize>::Node::insert(byte index, const T& data) {
        if(index >= _size)
            push(data);
        else {
            move_backward(_data + _size -1, _data + index, _data + _size);
            _data[index] = data;
            
            ++_size;
        }
    }
    
    _TEMPL T UnrolledLinkedList<T, nodeSize>::Node::remove(byte index) {
        if(index >= _size -1) {
            return pop();
        } else {
            T oldData = std::is_move_assignable<T>()? move(_data[index]) : _data[index];
            move(_data + index +1, _data + _size, _data + index);
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
    
    _IMPL(void) Node::clear() {
        if(std::is_destructible<T>::value)
            for(byte i = 0; i < _size; i++) {
                _data[i].~T();
            }
        _size = 0;
    }
    
    // ULL class
    
    // private
    
    _IMPL(typename _ULL::Node*) popResource() {
        Node * temp = _cache;
        _cache = _cache -> _next;
        temp -> _next = nullptr;
        return temp;
    }
    
    _IMPL(void) pushResource(Node * res) {
        if(_cache) {
            res -> _next = _cache;
        }
        _cache = res;
    }
    
    _IMPL(inline typename _ULL::Node *) newNode() {
        return _cache? popResource() : new Node;
    }
    
    _IMPL(typename _ULL::Node*) appendNewTailNode() {
        ++_nodeCount;
        
        Node * append = newNode();
        
        if(!_tail) { // case of complete emptyness
            return _head = _tail = append;
        } else {
            link(_tail, append);
            return _tail = append;
        }
    }
    
    _IMPL(typename _ULL::Node*) appendNewHeadNode() {
        ++_nodeCount;
        
        Node * append = newNode();
        
        if(!_head) { // emptyness
            return _head = _tail = append;
        } else {
            link(append, _head);
            return _head = append;
        }
    }
    
    // must be non-cached node, moves the head and tail pointer
    _IMPL(void) deleteNode(Node* node) {
        if(node -> _prev && node -> _next) {
            link(node -> _prev, node -> _next);
            node -> _prev = node -> _next = nullptr;
        } else {
            if(_tail == node) {_tail = _tail -> _prev;} // move the tail and head pointer to a correct position
            if(_head == node) {_head = _head -> _next;} // move before untangle
            
            if(node -> _next) {unlink(node, node -> _next);}
            if(node -> _prev) {unlink(node -> _prev, node);}
        }
            
        --_nodeCount; // decrease sizes
        _size -= node -> _size;
        
        if(_cache) // if theres some space for cached resources, dont destroy
            delete node;
        else {
            node -> clear();
            pushResource(node);
        }
    }
    
    _IMPL(typename _ULL::Node *) insertNewNodeAt(unsigned int at) {
        if(_nodeCount == 0) {
            
            return _head = _tail = newNode();
            
        } else if(at == 0) {
            
            return appendNewHeadNode();
            
        } else if(at >= _nodeCount) {
            
            return appendNewTailNode();
            
        } else {
            Node* target = nodeAt(at), * insert = newNode();
            link(insert, insert -> _next); // link right
            link(insert -> _prev, insert); // link left
            return insert;
        }
        
        ++_nodeCount;
    }
    
    _TEMPL typename _ULL::Node* _ULL::nodeAt(unsigned int at) {
        if(isInBounds(0, _nodeCount, at)) {
            Node* current = _head;
            for(unsigned int i = 0; i < at; i++) {
                current = current -> _next;
            }
            return current;
        }
    }
    
    _IMPL(typename _ULL::Node *) nodeAtIndex(unsigned int index) {
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
    
    _IMPL(void) elementAt(const unsigned int index, Node * & container, byte& offset) {
        Node * current = _head;
        unsigned int i = 0;
        while(current) {
            if(i + current -> _size > index)
                break;
            i += current -> _size;
            current = current -> _next;
        }
        container = current;
        offset = index - i;
    }
    
    _IMPL(inline bool) isFull(Node * node) {
        return node == nullptr || node -> _size == nodeSize;
    }
    
    _IMPL(inline bool) isEmpty(Node * node) {
        return node != nullptr && node -> _size == 0;
    }
    
    _IMPL(inline void) link(Node * left, Node * right) {
        left -> _next = right;
        right -> _prev = left;
    }
    
    _IMPL(inline void) unlink(Node * left, Node * right) {
        left -> _next = right -> _prev = nullptr;
    }
    
    // public members
    
    _TEMPL UnrolledLinkedList<T, nodeSize>::UnrolledLinkedList()
    :   _head{nullptr},
        _tail(nullptr),
        _cache(nullptr) {
    }
    
    _TEMPL UnrolledLinkedList<T, nodeSize>::UnrolledLinkedList(unsigned int initialCapacity)
    :   _head(nullptr),
        _tail(nullptr) {
        if(initialCapacity > 0) {
            for(unsigned int i = 0, stop = (initialCapacity / nodeSize) +1; i < stop; i++)
                pushResource(new Node);
        }
    }
    
    _TEMPL _ULL::~UnrolledLinkedList<T, nodeSize>() { // like clear(), just cheaper
        if(_head) { // clearing node snake
            Node* current = _head;
            
            while(current) {
                Node* temp = current;
                current = current -> _next;
                delete temp;
            }
        }
        
        while(_cache) { // clearing cache
            Node * temp = _cache;
            _cache = _cache -> _next;
            delete temp;
        }
    }
    
    _IMPL(_ULL&) pushBack (const T& obj) {
        if(isFull(_tail))
            appendNewTailNode();
        _tail -> push(obj);
        
        ++_size;
        return *this;
    }
    
    _TEMPL T& UnrolledLinkedList<T, nodeSize>::peekBack() const {
        if(_size > 0)
            return _tail -> peek();
    }
    
    _TEMPL T UnrolledLinkedList<T, nodeSize>::popBack () {
        if(_size > 0) {
            T value = _tail -> pop();
            
            if(isEmpty(_tail))
                deleteNode(_tail);

            --_size;
            return value;
        }
        throw LIST_EMPTY_MSG;
    }
    
    _TEMPL UnrolledLinkedList<T, nodeSize>& _ULL::pushFront(const T& item) {
        if(isFull(_head))
            appendNewHeadNode();
        _head -> insert(0, item);
        ++_size;
        return *this;
    }
    
    _TEMPL T& _ULL::peekFront() const {
        if(_size > 0)
            return _head[0];
        throw LIST_EMPTY_MSG;
    }
    
    _TEMPL T UnrolledLinkedList<T, nodeSize>::popFront() {
        if(_size > 0) {
            T& data = _head -> remove(0);
            if(isEmpty(_head))
                deleteNode(_head);
            --_size;
            return data;
        }
        throw LIST_EMPTY_MSG;
    }
    
    _TEMPL T& UnrolledLinkedList<T, nodeSize>::operator [] (unsigned int index) {
        if(isInBounds((unsigned int) 0, _size, index)) {
            Node * current = nullptr;
            byte off = 0;
            elementAt(index, current, off);
            return current -> operator[](off);
        }
        throw OUT_OF_BOUNDS_MSG;
    }
    
    _TEMPL const T& _ULL::operator [] (unsigned int index) const {
        return operator[] (index);
    }
    
    _IMPL(T) remove(unsigned int index) {
        if(_size == 0) throw LIST_EMPTY_MSG;
        if(index >= _size) throw OUT_OF_BOUNDS_MSG;
        
        Node * target = nullptr; byte offset;
        elementAt(index, target, offset);
        
        T rem = target -> remove(offset);
        if(isEmpty(target)) {
            deleteNode(target);
        }
        
        --_size;
        return rem;
    }
    
    _IMPL(void) insert(const unsigned int index, const T & data) {
        if(index >= _size) {
            pushBack(data);
        } else if(index == 0) {
            pushFront(data);
        } else {
            Node * target = nullptr; byte off;
            elementAt(index, target, off);
            if(isFull(target)) { // oh fuck...
                if(off == 0) { // insert before
                    Node * insert;
                    if(isFull(target -> _prev)) { // new Node
                        insert = newNode(); // never _head, this case is handled somewhere above
                        link(target -> _prev, insert);
                        link(insert, target);
                        ++_nodeCount;
                    } else { // use _prev
                        insert = target -> _prev;
                    }
                    insert -> push(data);
                } else { // insert in the middle of the node :(
                    Node * insert;
                    byte shiftLen = nodeSize - off;
                    if(target -> _next && target -> _next -> _size + shiftLen <= nodeSize) {
                        // in this case, the right node of target has enough space to shift right and take up some data from target
                        insert = target -> _next;
                        // shift dest
                        move_backward(insert -> _data -1,
                                      insert -> _data + insert -> _size -1,
                                      insert -> _data + insert -> _size + shiftLen -1);
                        // move items from target to insert
                        move(target -> _data + off, target -> _data + nodeSize, insert -> _data);
                        // set the new sizes, data is appended below
                        target -> _size -= shiftLen;
                        insert -> _size += shiftLen;
                    } else { // target is _tail node or target -> _next is full or has not enough space to shift right and take up target's data
                        // data is moved into a new node
                        insert = newNode();
                        if(target == _tail) { // right-end
                            link(_tail, insert);
                            _tail = insert;
                        } else { // somewhere in between
                            link(insert, target -> _next);
                            link(target, insert);
                        }
                        // move the items from target into the new node
                        move(target -> _data + off, target -> _data + nodeSize, insert -> _data);
                        // set the new sizes, data is appended below
                        target -> _size -= shiftLen;
                        insert -> _size += shiftLen;
                        ++_nodeCount;
                    }
                    target -> push(data); // data is appended in target not in insert
                }
            } else {
                target -> insert(off, data);
            }
            ++_size;
        }
    }
    
    // clears cache as well
    _IMPL(void) clear() {
        bufferedClear();
        while(_cache) {
            Node * temp = _cache;
            _cache = _cache -> _next;
            delete temp;
        }
    }
    
    _IMPL(void) bufferedClear() {
        for(Node * current = _head; current; ) {
            Node * temp = current;
            current = current -> _next;
            delete temp;
        }
        
        _head = _tail = nullptr;
        
        _nodeCount = 0;
        _size = 0;
    }
    
    _IMPL(std::string) toString() const {
        std::stringstream builder;
        builder << "{";
        
        Node* current = _head;
        
        for(unsigned int nodeI = 0; current; ++nodeI, current = current -> _next) {
            if(nodeI > 0) builder << ", ";
            builder << "[";
            for(byte b = 0; b < current -> _size; ++b) {
                if(b != 0) builder << ", ";
                builder << &current[b];
            }
            builder << "]";
        }
        
        builder << "}";
        return builder.str();
    }
        
#undef _IMPL
#undef _TEMPL
#undef _ULL
}

#endif /* UnrolledLinkedList_hpp */
