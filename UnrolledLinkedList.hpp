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

namespace std {
    
    typedef char byte;

    template <typename T, byte nodeSize = 5>
    class UnrolledLinkedList {
        
    public:
        
        UnrolledLinkedList();
        UnrolledLinkedList(unsigned int);
        ~UnrolledLinkedList();
        
        T& operator [] (unsigned int);
        const T& operator [] (unsigned int) const;
        
        UnrolledLinkedList& pushBack(const T&);
        T& peekBack() const;
        T& popBack();
        UnrolledLinkedList& pushFront(const T&);
        T& peekFront() const;
        T& popFront();
        
        unsigned int inline size() const;
        unsigned int inline nodeCount() const;
        void insert(unsigned int, const T&);
        void resize();
        T& remove(unsigned int);
        void clear();
        void bufferedClear();
        
        string toString() const;
        
    protected:
        
    private:
        
        struct Node {
            Node* _prev, * _next;
            T* _data;
            byte _size;
            
            Node();
            Node(Node*, Node*);
            
            ~Node() {
                delete [] _data;
            }
            void clear();
            
            inline void push(const T&);
            inline T& pop();
            inline T& peek() const;
            void insert(byte, const T&);
            T& remove(byte);
            
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
        Node* mostLeftNode();
        Node* mostRightNode();
        
        void deleteNode(Node*);
        Node* nodeAt(unsigned int);
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
    
    _IMPL(void) Node::clear() {
        for(byte i = 0; i < _size; i++) {
            ~_data[i]();
        }
        _size = 0;
    }
    
    // ULL class
    
    // private
    
    _IMPL(typename _ULL::Node*) popResource() {
        Node * temp = _cache;
        _cache = _cache -> _next;
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
        
//        Node *newNode = new Node;
//
//        if(!_head) {
//            _head = _tail = newNode;
//        } else if(!_tail -> _next) {
//            _tail -> _next = newNode;
//            newNode -> _prev = _tail;
//            _tail = newNode;
//        }
//
//        ++_nodeCount;
//        return newNode;
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
        
//        Node* newNode = new Node;
//
//        if(!_head) {
//            _head = _tail = newNode;
//        } else if(!_head -> _prev) {
//            newNode -> _next = _head;
//            _head -> _prev = newNode;
//            _tail = newNode;
//        }
//
//        ++_nodeCount;
//        return newNode;
    }
    
    // must be non-cached node, moves the head and tail pointer
    _IMPL(void) deleteNode(Node* node) {
        if(node -> _prev && node -> _next) {
            link(node -> _prev, node -> _next);
        } else {
            if(node -> _next) unlink(node, node -> _next);
            if(node -> _prev) unlink(node -> _prev, node);
        }

        
//        if(node -> _prev && node -> _next) { // fully unlink the node and link its neighbours
//            if(!node -> _prev) {
//                unlink(node, node -> _next);
//            } else if(!node -> _next) {
//                unlink(node -> _prev, node);
//            } else {
//                link(node -> _prev, node -> _next);
//            }
//        }
//
        if(_tail == node) _tail = _tail -> _prev; // move the tail and head pointer to a correct position
        if(_head == node) _head = _head -> _next;
            
        --_nodeCount; // decrease sizes
        _size -= node -> _size;
        
        if(_cache) // if theres some space for cached resources, dont destroy
            delete node;
        else {
            ~node();
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
    
    _IMPL(typename _ULL::Node*) mostLeftNode() {
        Node * current = _head;
        while(current -> _prev) {
            current = current -> _prev;
        }
        return current;
    }
    
    _IMPL(typename _ULL::Node*) mostRightNode() {
        Node * current = _tail;
        while(current -> _next) {
            current = current -> _next;
        }
        return current;
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
            appended -> push(obj);
            _tail = appended;
        } else {
            _tail -> push(obj);
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
    
        
    template <typename B, typename I>
    bool isInBounds(B offset, B bounds, I index) {
        return index > offset && index < offset + bounds;
    }
        
        
    template<typename T, byte nodeSize = 5>
    std::ostream& operator << (ostream& out, const UnrolledLinkedList<T, nodeSize> target) {
        out << target.toString();
        return out;
    }
}

#endif /* UnrolledLinkedList_hpp */
