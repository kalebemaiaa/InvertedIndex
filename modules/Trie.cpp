#include "Trie.hpp"
#include <fstream>

// TrieNode

TrieNode::TrieNode() {
    this->leaf = false;
    this->frequency = 0;
    this->docsList = new LinkedList<IndexNode>;
    this->childrens = std::map<std::vector<unsigned char>, TrieNode *>();
}

void TrieNode::plusFrequency() {
    this->frequency++;
}

int TrieNode::getFrequency() {
    return this->frequency;
}

void TrieNode::setChild(std::vector<unsigned char> c, TrieNode *node) {
    childrens[c] = node;
}

TrieNode *TrieNode::getChild(const std::vector<unsigned char> &c) {
    auto it = childrens.find(c);
    if (it != childrens.end()) {
        return it->second;
    }
    return nullptr;
}

void TrieNode::setLeaf(bool value) {
    this->leaf = value;
}

bool TrieNode::isLeaf() {
    return this->leaf;
}

void TrieNode::setDocsList(LinkedList<IndexNode> *list) {
    this->docsList = list;
}

LinkedList<IndexNode> *TrieNode::getDocsList() {
    return this->docsList;
}

TrieNode::~TrieNode() {
    for (auto &pair : childrens)
        delete pair.second;
}

Trie::Trie() {
    this->root = new TrieNode();
    this->allDocsList = new LinkedList<IntNode>();
}

std::vector<std::vector<unsigned char>> Trie::wordToVector(const std::string &word) {
    std::vector<std::vector<unsigned char>> utf8vector;
    size_t i = 0;

    while (i < word.length()) {
        std::vector<unsigned char> utf8Char;
        unsigned char c = static_cast<unsigned char>(word[i]);

        if ((c & 0x80) == 0) {
            utf8Char.push_back(c);
            i++;
        } else if ((c & 0xE0) == 0xC0) {
            utf8Char.push_back(c);
            if (i + 1 < word.length()) {
                utf8Char.push_back(static_cast<unsigned char>(word[i + 1]));
                i += 2;
            } else {
                i++;
            }
        } else if ((c & 0xF0) == 0xE0) {
            utf8Char.push_back(c);
            if (i + 2 < word.length()) {
                utf8Char.push_back(static_cast<unsigned char>(word[i + 1]));
                utf8Char.push_back(static_cast<unsigned char>(word[i + 2]));
                i += 3;
            } else {
                i++;
            }
        } else if ((c & 0xF8) == 0xF0) {
            utf8Char.push_back(c);
            if (i + 3 < word.length()) {
                utf8Char.push_back(static_cast<unsigned char>(word[i + 1]));
                utf8Char.push_back(static_cast<unsigned char>(word[i + 2]));
                utf8Char.push_back(static_cast<unsigned char>(word[i + 3]));
                i += 4;
            } else {
                i++;
            }
        } else {
            i++;
        }

        if (!utf8Char.empty()) {
            utf8vector.push_back(utf8Char);
        }
    }

    return utf8vector;
}

std::string Trie::vectorToWord(const std::vector<unsigned char> &utf8char) {
    return std::string(utf8char.begin(), utf8char.end());
}

void Trie::toLowerCase(std::string &str) {
    for (char &c : str)
        if (c >= 'A' && c <= 'Z')
            c = c + ('a' - 'A');
}

std::string Trie::cleanWords(const std::string &word, std::string charsToRemove) {
    std::string cleanedWord;
    for (size_t i = 0; i < word.length(); i++) {
        char c = word[i];
        if (charsToRemove.find(c) == std::string::npos) {
            cleanedWord += c;
        } else {
            cleanedWord += ' ';
        }
    }

    // this->toLowerCase(cleanedWord);
    return cleanedWord;
}

TrieNode *Trie::getWord(const std::string &word) {
    std::string cleanedWord = this->cleanWords(word);
    std::vector<std::vector<unsigned char>> realWord = wordToVector(cleanedWord);
    TrieNode *cur = this->root;

    for (std::vector<unsigned char> c : realWord) {
        TrieNode *child = cur->getChild(c);
        if (!child)
            return child;
        cur = child;
    }

    return cur;
}

LinkedList<IndexNode> *Trie::queryWord(const std::string &word) {
    TrieNode *w = this->getWord(word);
    if(w) {
        auto p = [] (IndexNode *n1) {
            // std::cout << n1->data;
            auto x = [] (IntNode *n) {
                std::cout << n->data;
            };
            std::cout << n1->docId << ": ";
            n1->positions->print(x, true);
            std::cout << std::endl;
        };
        w->getDocsList()->print(p);
        // w->getDocsList()->getHead()->data->positions->print(p, true);
    }
    return w ? w->getDocsList() : nullptr;
}

bool Trie::isLimitador(char c) {
    return c == '!' || c == '?' || c == '.' || c == ',' || c == ' ';
}

void Trie::toUpperCase(std::string &str) {
    for (char &c : str)
        if (c >= 'a' && c <= 'z')
            c = c - ('a' - 'A');
}

void Trie::addWord(const std::string &word, int docId, int position) {
    TrieNode *cur = this->root;
    std::vector<std::vector<unsigned char>> realWord = this->wordToVector(word);

    for (std::vector<unsigned char> c : realWord) {
        TrieNode *child = cur->getChild(c);
        if (child == nullptr) {
            child = new TrieNode();
            cur->setChild(c, child);
        }
        cur = child;
    }

    LinkedList<IntNode> *positions = new LinkedList<IntNode>;
    IntNode *newNodeInt = new IntNode{position};
    positions->addInOrder(newNodeInt, compareIntNodes);
    IndexNode *newNodeIndex = new IndexNode{positions, 1, docId};

    if (cur->isLeaf()) {
        LinkedList<IndexNode> *docsList = cur->getDocsList();
        if (docsList->addInOrder(newNodeIndex, advancedCompareIndex)) {
            IntNode *docIdNode = new IntNode{docId};
            this->allDocsList->addInOrder(docIdNode, compareIntNodes);
        }
    } else {
        LinkedList<IndexNode> *newList = new LinkedList<IndexNode>;
        newList->addInOrder(newNodeIndex, compareIndexNodes);

        cur->setLeaf(true);
        cur->setDocsList(newList);

        IntNode *docIdNode = new IntNode{docId};
        this->allDocsList->addInOrder(docIdNode, compareIntNodes);
    }
    cur->plusFrequency();
}

int Trie::indexLine(std::string txt, int docId, int posInicial) {
    std::string cleanedTxt = cleanWords(txt);
    std::string token;

    for (size_t i = 0; i < cleanedTxt.length();) {
        size_t count = i;
        while (i < cleanedTxt.length() && !this->isLimitador(cleanedTxt[i]))
            i++;
        token = cleanedTxt.substr(count, i - count);
        this->addWord(token, docId, posInicial);
        posInicial++;
        while (i < cleanedTxt.length() && this->isLimitador(cleanedTxt[i]))
            i++;
    }

    return posInicial;
}

int Trie::extractNumberFromFilename(const std::string &filepath) {
    // Encontrar a última barra ("/") para extrair o nome do arquivo
    size_t lastSlashPos = filepath.find_last_of("/\\");
    std::string filename = (lastSlashPos == std::string::npos) 
                            ? filepath 
                            : filepath.substr(lastSlashPos + 1);

    // Encontrar o ponto (".") para remover a extensão
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        filename = filename.substr(0, dotPos);
    }

    // Converter a parte restante para um inteiro
    int number = std::stoi(filename);

    return number;
}

void Trie::indexFile(std::string &fname) {
    std::ifstream file(fname);
    if (!file.is_open()) {
        std::cout << "O arquivo " << fname << " nao pode ser aberto!" << std::endl;
        return;
    }
    std::cout << "Indexidando as " << extractNumberFromFilename(fname) << ": " << fname << std::endl;
    std::string line;
    int pos = 0;
    while (getline(file, line)) {
        pos = this->indexLine(line, extractNumberFromFilename(fname), pos);
    }
}

void Trie::indexByFile(std::string &fname, std::string path) {
    std::ifstream file(fname);
    if (!file.is_open()) {
        std::cout << "O arquivo " << fname << " nao pode ser aberto!" << std::endl;
        return;
    }

    std::string line;
    int pos = 0;
    while (getline(file, line)) {
        std::string r = path + "\\" + line;
        this->indexFile(r);
    }
}
