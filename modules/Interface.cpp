#include "Interface.hpp"
#include "LinkedList.hpp"

// Query class implementation
Query::Query(string word, string operation) : operation(operation)
{
    if (!word.empty())
        this->word.push_back(word);
}

Query Query::andQuery(initializer_list<Query> queries)
{
    Query q("", "AND");
    q.subQueries.insert(q.subQueries.end(), queries.begin(), queries.end());
    return q;
}

Query Query::orQuery(initializer_list<Query> queries)
{
    Query q("", "OR");
    q.subQueries.insert(q.subQueries.end(), queries.begin(), queries.end());
    return q;
}

Query Query::nearQuery(string word1, string word2, int dist)
{
    Query q("", "NEAR");
    q.word.push_back(word1);
    q.word.push_back(word2);
    q.dist = dist;
    return q;
}

Query Query::notQuery(Query query)
{
    Query q("", "NOT");
    q.subQueries.push_back(query);
    return q;
}

Query Query::wQuery(string word)
{
    return Query(word, "WORD");
}

LinkedList<IntNode> *Interface::intersectQuery(Query query, string *queryStr)
{
    LinkedList<LinkedList<IntNode>> *evaluated = new LinkedList<LinkedList<IntNode>>();
    LinkedList<IntNode> *merged = new LinkedList<IntNode>();

    *queryStr += "(";

    for (size_t i = 0; i < query.subQueries.size(); i++)
    {
        *queryStr += i == 0 ? "" : " " + query.operation + " ";
        LinkedList<IntNode> *result = evalQuery(query.subQueries[i], queryStr);

        if (query.operation == "OR")
        {
            if (result)
                merged = merged->merge(result, query.operation, compareIntNodes);
            continue;
        }

        if (!result)
            return nullptr;

        evaluated->addTail(result);
    }
    *queryStr += ")";

    if (query.operation == "OR")
        return merged;

    auto compareLinkedListLength = [](LinkedList<IntNode> *n1, LinkedList<IntNode> *n2) -> int {
        return n2->getLength() - n1->getLength();
    };
    evaluated->sort(compareLinkedListLength);

    Node<LinkedList<IntNode>> *cur = evaluated->getHead();
    merged = cur->data;

    for(cur = cur -> next; cur; cur = cur -> next)
        merged = merged->merge(cur->data, query.operation, compareIntNodes);

    return merged;
}

bool Interface::isKdistance(LinkedList<IntNode> *p1, LinkedList<IntNode> *p2, int distance)
{
    if (!p1 || !p2)
        return false;
    Node<IntNode> *c1 = p1->getHead(), *c2 = p2->getHead();
    while (c1 && c2)
    {
        int pos1 = c1->data->data, pos2 = c2->data->data;
        if (abs(pos1 - pos2) <= distance)
            return true;
        else if (pos1 < pos2)
            c1 = c1->next;
        else
            c2 = c2->next;
    }
    return false;
}

LinkedList<IntNode> *Interface::nearMerge(Node<IndexNode> *p1, Node<IndexNode> *p2, int distance)
{
    LinkedList<IntNode> *results = new LinkedList<IntNode>();

    while (p1 && p2)
    {
        if (p1->data->docId == p2->data->docId)
        {
            if (isKdistance(p1->data->positions, p2->data->positions, distance))
            {
                IntNode *newNode = new IntNode{p1->data->docId};
                results->addInOrder(newNode, compareIntNodes);
            }

            p1 = p1->next;
            p2 = p2->next;
        }
        else if (p1->data->docId < p2->data->docId)
            p1 = p1->next;
        else
            p2 = p2->next;
    }

    return results;
}

LinkedList<IntNode> *Interface::indexListToInt(LinkedList<IndexNode> *l1)
{
    LinkedList<IntNode> *newList = new LinkedList<IntNode>();
    for (Node<IndexNode> *cur = l1->getHead(); cur; cur = cur->next)
    {
        IntNode *newNode = new IntNode{cur->data->docId};
        newList->addTail(newNode);
    }

    return newList;
}

LinkedList<IntNode> *Interface::evalQuery(Query query, string *queryStr)
{
    if (query.operation == "NOT")
    {
        *queryStr += "(" + query.operation + " ";
        LinkedList<IntNode> *testando = evalQuery(query.subQueries[0], queryStr);
        // LinkedList<IntNode> *toReturn = database->complement(testando);
        *queryStr += ")";
        return testando;
    }
    if (query.operation == "WORD")
    {
        *queryStr += query.word[0];
        LinkedList<IndexNode> *result = database->queryWord(query.word[0]);
        return result ? this->indexListToInt(result) : nullptr;
    }
    if (query.operation == "NEAR")
    {
        *queryStr += "(" + query.operation;
        *queryStr += " (" + query.word[0] + ", " + query.word[1] + ", " + to_string(query.dist) + "))";
        LinkedList<IndexNode> *left = database->queryWord(query.word[0]);
        LinkedList<IndexNode> *right = database->queryWord(query.word[1]);
        if (!left || !right)
            return nullptr;

        return nearMerge(left->getHead(), right->getHead(), query.dist);
    }

    return intersectQuery(query, queryStr);
}

Interface::Interface(Trie *database) : database(database) {}

LinkedList<IntNode> *Interface::query(Query query, std::string *qryStr)
{
    string queryStr = "";
    LinkedList<IntNode> *result = evalQuery(query, &queryStr);
    if(qryStr)
        *qryStr = queryStr;
    cout << queryStr << ": ";
    auto p = [](IntNode *n)
    {
        std::cout << n->data;
    };

    if (result)
        result->print(p, true);
    else
        cout << "null" << endl;

    return result;
}

void Interface::listQueries(string fname, initializer_list<Query> queries) {
    FILE *fp;
    fp = fopen(fname.c_str(), "w");
    for (const auto &q : queries) {
        std::string s = "";
        LinkedList<IntNode> *result = query(q, &s);
        fprintf(fp, "%s: [", s.c_str());
        Node<IntNode> *cur = result->getHead();
        while(cur){
            fprintf(fp, "%i", cur->data->data);
            if(cur->next)
                fprintf(fp, ", ");
            cur = cur -> next;
        }
        fprintf(fp, "]\n");
    }

    fclose(fp);
}