//
//  UnrolledLinkedList.hpp
//  BrainfuckInterpreter
//
//  Created by Ulrich Sander on 18.11.18.
//  Copyright © 2018 Richard Förster. All rights reserved.
//

#ifndef UnrolledLinkedList_hpp
#define UnrolledLinkedList_hpp

#include <string>

namespace std {
    
    typedef char byte;
    
    template <typename B, typename I>
    bool isInBounds(B offset, B bounds, I index) {
        return index > offset && index < offset + bounds;
    }
    
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
            
            inline void push(const T&);
            inline T& pop();
            inline T& peek() const;
            void insert(byte, const T&);
            T& remove(byte);
            
            inline T& operator [] (byte);
            inline const T& operator [] (byte) const;
        };
        
        Node* _head, * _tail;
        unsigned int _size, _nodeCount;
        
        Node* appendNewTailNode();
        Node* appendNewHeadNode();
        Node* insertNewNodeAt(unsigned int);
        
        void deleteNode(Node*);
        Node* nodeAt(unsigned int);
        Node* nodeAtIndex(unsigned int);
        inline bool isFull(Node*);
        inline bool isEmpty(Node*);
        
        inline void link(Node*, Node*);
        inline void unlink(Node*, Node*);
    };
    
    template<typename T, byte nodeSize = 5>
    std::ostream& operator << (ostream& out, const UnrolledLinkedList<T, nodeSize> target) {
        out << target.toString();
        return out;
    }
}

#endif /* UnrolledLinkedList_hpp */
