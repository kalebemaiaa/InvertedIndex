#include "LinkedList.hpp"

// Implementação da classe Node
template <typename T>
Node<T>::Node(T *newNodeData)
    : data(new T(*newNodeData)), next(nullptr) {}

template <typename T>
Node<T>::~Node()
{
    delete data;
}

template <typename T>
Node<T>::Node(const Node<T> &other)
    : data(new T(*other.data)), next(other.next ? new Node<T>(*other.next) : nullptr) {}

template <typename T>
Node<T> &Node<T>::operator=(const Node<T> &other)
{
    if (this != &other)
    {
        delete data;
        delete next;
        data = new T(*other.data);
        next = other.next ? new Node<T>(*other.next) : nullptr;
    }
    return *this;
}

// Implementação da classe LinkedList
template <typename T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), length(0) {}

template <typename T>
LinkedList<T>::~LinkedList()
{
    Node<T> *cur = head;
    while (cur)
    {
        Node<T> *next = cur->next;
        delete cur;
        cur = next;
    }
}

template <typename T>
void LinkedList<T>::addTail(T *value)
{
    Node<T> *newNode = new Node<T>(value);

    if (!head)
    {
        head = newNode;
        tail = newNode;
        length++;
        return;
    }

    if (!tail)
    {
        std::cout << "Deu ruim, nao tem cauda" << std::endl;
        delete newNode;
        return;
    }

    tail->next = newNode;
    tail = newNode;
    length++;
}

template <typename T>
bool LinkedList<T>::addInOrder(T *value, int (*compare)(T *, T *))
{
    Node<T> *newNode = new Node<T>(value);
    if (!head)
    {
        head = newNode;
        tail = newNode;
        length++;
        return true;
    }

    Node<T> *pre = nullptr;
    Node<T> *cur = head;
    int comparation;

    while (cur)
    {
        comparation = compare(cur->data, newNode->data);
        if (comparation == 0)
        {
            return false;
        }
        if (comparation < 0)
        {
            newNode->next = cur;
            if (!pre)
            {
                head = newNode;
            }
            else
            {
                pre->next = newNode;
            }
            length++;
            return true;
        }
        pre = cur;
        cur = cur->next;
    }

    pre->next = newNode;
    tail = newNode;
    length++;
    return true;
}

template <typename T>
void LinkedList<T>::print(void (*print)(T *), bool printArrow) const
{
    Node<T> *cur = head;
    while (cur)
    {
        print(cur->data);
        if (cur->next && printArrow)
        {
            std::cout << " -> ";
        }
        cur = cur->next;
    }
    std::cout << std::endl;
}

template <typename T>
Node<T> *LinkedList<T>::getHead() const
{
    return head;
}

template <typename T>
int LinkedList<T>::getLength() const
{
    return length;
}

template <typename T>
LinkedList<T> *LinkedList<T>::merge(LinkedList<T> *listToMerge, const std::string &operation, int (*compare)(T *, T *)) const
{
    if (!listToMerge)
        return operation == "AND" ? new LinkedList<T>() : new LinkedList<T>(*this);

    Node<T> *p1 = head;
    Node<T> *p2 = listToMerge->getHead();
    LinkedList<T> *mergedList = new LinkedList<T>();

    while (p1 && p2)
    {
        int comparation = compare(p1->data, p2->data);
        if (comparation == 0)
        {
            mergedList->addTail(p1->data);
            p1 = p1->next;
            p2 = p2->next;
        }
        else if (comparation > 0)
        {
            if (operation == "OR")
            {
                mergedList->addTail(p1->data);
            }
            p1 = p1->next;
        }
        else
        {
            if (operation == "OR")
            {
                mergedList->addTail(p2->data);
            }
            p2 = p2->next;
        }
    }

    if (operation == "OR")
    {
        while (p1)
        {
            mergedList->addTail(p1->data);
            p1 = p1->next;
        }
        while (p2)
        {
            mergedList->addTail(p2->data);
            p2 = p2->next;
        }
    }

    return mergedList;
}

template <typename T>
Node<T> *auxiliarMerge(Node<T> *l1, Node<T> *l2, int (*compare)(T *, T *), bool reverse = true)
{
    Node<T> *newHead = nullptr, *cur = nullptr;

    // Determine the initial node of the merged list
    if (!l1) return l2;
    if (!l2) return l1;

    int comparation = compare(l1->data, l2->data);
    comparation = reverse ? -comparation : comparation;

    if (comparation <= 0) // Use <= to handle equal values
    {
        newHead = l1;
        l1 = l1->next;
    }
    else
    {
        newHead = l2;
        l2 = l2->next;
    }
    cur = newHead;
    cur->next = nullptr;

    // Merge the remaining nodes
    while (l1 && l2)
    {
        comparation = compare(l1->data, l2->data);
        comparation = reverse ? -comparation : comparation;
        if (comparation <= 0) // Use <= to handle equal values
        {
            cur->next = l1;
            l1 = l1->next;
        }
        else
        {
            cur->next = l2;
            l2 = l2->next;
        }
        cur = cur->next;
        cur->next = nullptr;
    }

    // Append the remaining nodes
    if (l1)
        cur->next = l1;
    if (l2)
        cur->next = l2;

    return newHead;
}


template <typename T>
void LinkedList<T>::sort(int (*compare)(T *, T *))
{
    if (!this->head || !this->head->next) return; // No need to sort if the list is empty or has a single element

    Node<T> *cur = this->head;
    int count = 0;
    while (count < this->length / 2 - 1) // Find the middle of the list
    {
        cur = cur->next;
        count++;
    }

    Node<T> *r = cur->next;
    cur->next = nullptr;

    // Recursively sort the two halves
    this->head = auxiliarMerge(sortList(this->head, compare), sortList(r, compare), compare);
}

template <typename T>
Node<T> *LinkedList<T>::sortList(Node<T> *head, int (*compare)(T *, T *))
{
    if (!head || !head->next) return head;

    Node<T> *cur = head;
    int count = 0;
    while (cur->next)
    {
        cur = cur->next;
        count++;
    }

    cur = head;
    for (int i = 0; i < count / 2; ++i)
        cur = cur->next;

    Node<T> *r = cur->next;
    cur->next = nullptr;

    Node<T> *left = sortList(head, compare);
    Node<T> *right = sortList(r, compare);

    return auxiliarMerge(left, right, compare);
}

// Funções auxiliares
int compareIntNodes(IntNode *n1, IntNode *n2)
{
    return n2->data - n1->data;
}

int compareIndexNodes(IndexNode *n1, IndexNode *n2)
{
    return n2->docId - n1->docId;
}

int advancedCompareIndex(IndexNode *n1, IndexNode *n2)
{
    if(n1->docId == n2->docId) {
        IntNode *newNodeInt = new IntNode{n2->positions->getHead()->data->data};
        n1->positions->addInOrder(newNodeInt, compareIntNodes);
    }
    return n2->docId - n1->docId;
}

// Instanciar templates
template class Node<IntNode>;
template class LinkedList<IntNode>;
template class Node<IndexNode>;
template class LinkedList<IndexNode>;
template class Node<LinkedList<IntNode>>;
template class LinkedList<LinkedList<IntNode>>;