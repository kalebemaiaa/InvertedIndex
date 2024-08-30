#include "LinkedList.hpp"
#include <iostream>
#include <vector>
#include <map>

class TrieNode
{
private:
    bool leaf;
    int frequency;
    LinkedList<IndexNode> *docsList;
    std::map<std::vector<unsigned char>, TrieNode *> childrens;

public:
    TrieNode();
    void plusFrequency();
    int getFrequency();
    void setChild(std::vector<unsigned char> c, TrieNode *node);
    TrieNode *getChild(const std::vector<unsigned char> &c);
    void setLeaf(bool value);
    bool isLeaf();
    void setDocsList(LinkedList<IndexNode> *list);
    LinkedList<IndexNode> *getDocsList();
    ~TrieNode();
};

class Trie {
private:
    TrieNode *root;
    LinkedList<IntNode> *allDocsList;

public:
    std::vector<std::vector<unsigned char>> wordToVector(const std::string &word);
    std::string vectorToWord(const std::vector<unsigned char> &utf8char);
    void toLowerCase(std::string &str);
    std::string cleanWords(const std::string &word, std::string charsToRemove = "\".,;:!?«»()[]");
    TrieNode *getWord(const std::string &word);
    bool isLimitador(char c);
    void toUpperCase(std::string &str);
    void addWord(const std::string &word, int docId, int position);
    int indexLine(std::string txt, int docId, int posInicial);
    int extractNumberFromFilename(const std::string &filepath);
    void indexFile(std::string &fname);
    void indexByFile(std::string &fname, std::string path);
    LinkedList<IndexNode> *queryWord(const std::string &word);
    Trie();
    ~Trie();
};
