#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

struct Object {
    Object(): identifier(""), type(""), value(""), scope(0) {}
    Object(string identifier, string type, string value, int scope): identifier(identifier), type(type), value(value), scope(scope) {}

    string identifier;
    string type;
    string value;
    int scope;

};

struct ObjectNode {
    ObjectNode(): next(NULL), data(Object()) {}
    ObjectNode(string identifier, string type, string value, int scope): next(NULL), data(Object(identifier, type, value, scope)) {}

    ObjectNode* next;
    Object data;
};

template<class T>
struct Node {
    Node<T>(): next(NULL), data() {}
    Node<T>(T var): next(NULL), data(var) {}

    Node<T>* next;
    T data;
};

template<class T>
class St {
    private:
        Node<T>* top;
        int count;
    public:
        St() {
            count = 0;
            top = nullptr;
        }
        ~St() {
            while (top) {
                Node<T> *temp = top;
                top = top->next;
                delete temp;
            }
        }
        void push(T);
        T get_top();
        Node<T>* get_head();
        int pop();
        bool isExist(T);
        bool isEmpty();
        int getSize();
};


class SymbolTable {
    private:
        ObjectNode *hashTable;
        St<string> var_collision;
    public:
        SymbolTable() {
            hashTable = new ObjectNode[2069];
        };
        ~SymbolTable() {
            Node<string>* p = var_collision.get_head();
            while (p != NULL) {
                int i = hash<std::string>{}(p->data) % 2069;
                ObjectNode* head = &hashTable[i];
                head = head->next;
                while (head != NULL) {
                    ObjectNode* temp = head;
                    head = head->next;
                    delete temp;
                }
                p = p->next;
            }
            delete[] hashTable;
        };
        Object lookup_identifier(string, int, int, string);
        void run(string filename);
};


#endif