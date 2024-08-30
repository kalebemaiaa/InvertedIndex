#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <vector>
#include <string>
#include <iostream>
#include "Trie.hpp"

using namespace std;

class Query
{
public:
    vector<string> word;
    string operation;
    int dist;
    vector<Query> subQueries;

    Query(string word = "", string operation = "");

    static Query andQuery(initializer_list<Query> queries);
    static Query orQuery(initializer_list<Query> queries);
    static Query nearQuery(string word1, string word2, int dist);
    static Query notQuery(Query query);
    static Query wQuery(string word);
};

class Interface
{
private:
    Trie *database;

    void swap(vector<int> &vec, int i, int j);
    int partition(vector<LinkedList<IntNode> *> &listas, vector<int> &vec, int lower, int upper);
    void quickSort(vector<LinkedList<IntNode> *> &listas, vector<int> &vec, int lower, int upper);
    vector<int> sortLL(vector<LinkedList<IntNode> *> listas);
    LinkedList<IntNode> *intersectQuery(Query query, string *queryStr);
    bool isKdistance(LinkedList<IntNode> *p1, LinkedList<IntNode> *p2, int distance);
    LinkedList<IntNode> *nearMerge(Node<IndexNode> *p1, Node<IndexNode> *p2, int distance);
    LinkedList<IntNode> *evalQuery(Query query, string *queryStr);
    LinkedList<IntNode> *indexListToInt(LinkedList<IndexNode> *l1);

public:
    Interface(Trie *database);
    LinkedList<IntNode> *query(Query query,  std::string *qryStr = nullptr);
    void listQueries(string fname, initializer_list<Query> queries);
    void writeTest(string fname, Query query, string f_input, string path);
};

#endif // INTERFACE_HPP
