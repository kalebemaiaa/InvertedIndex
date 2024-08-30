#ifndef NODE_LINKEDLIST_HPP
#define NODE_LINKEDLIST_HPP

#include <iostream>
#include <string>

// Definição da classe Node
template <typename T>
class Node {
public:
    T *data;
    Node<T> *next;

    Node(T *newNodeData);
    ~Node();
    Node(const Node<T> &other); // Construtor de cópia
    Node<T>& operator=(const Node<T> &other); // Operador de atribuição de cópia
};

template <typename T>
class LinkedList {
private:
    Node<T> *head;
    Node<T> *tail;
    int length;

public:
    // Construtor e destrutor
    LinkedList();
    ~LinkedList();

    // Métodos públicos
    void addTail(T *value);
    bool addInOrder(T *value, int (*compare)(T *, T *));
    void print(void (*print)(T *), bool printArrow = false) const;
    Node<T> *getHead() const;
    int getLength() const;
    LinkedList<T> *merge(LinkedList<T> *listToMerge, const std::string &operation, int (*compare)(T *, T *)) const;
    
    // Métodos para ordenação
    Node<T> *sortList(Node<T> *head, int (*compare)(T *, T *));
    void sort(int (*compare)(T *, T *));
};

// Estruturas de dados
struct IntNode {
    int data;
};

struct IndexNode {
    LinkedList<IntNode> *positions;
    int frequencyAtDoc;
    int docId;
};

template <typename T>
struct LinkedNode {
    LinkedList<T> *data;
};

// Funções auxiliares
int compareIntNodes(IntNode *n1, IntNode *n2);
int advancedCompareIndex(IndexNode *n1, IndexNode *n2);
int compareIndexNodes(IndexNode *n1, IndexNode *n2);

#endif // NODE_LINKEDLIST_HPP
