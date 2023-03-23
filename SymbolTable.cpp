#include "SymbolTable.h"


inline Object SymbolTable::lookup_identifier(string identifier, int index, int scope, string instruction) {
    ObjectNode *head = &hashTable[index];
    ObjectNode *p = head;
    while (p != NULL) {
        if (p->data.identifier == identifier && p->data.scope == scope) {
            return p->data;
        }
        p = p->next;
    }
    if (scope == 0) {
        throw Undeclared(instruction);
    }
    return lookup_identifier(identifier, index, scope-1, instruction);
};

template<class T> void St<T>::push(T var) {
    Node<T>* temp = new Node<T>(var);
    temp->next = this->top;
    this->top = temp;
    this->count++;
}

template<class T> int St<T>::pop() {
    if (this->getSize() == 0) {
        return 0;
    }
    Node<T>* temp = this->top;
    this->top = temp->next;
    this->count--;
    delete temp;
    return 1;
}

template<class T> bool St<T>::isExist(T str) {
    Node<T>* p = this->top;
    while (p != NULL) {
        if (p->data == str) {
            return true;
        }
        p = p->next;
    }
    return false;
}

template<class T> T St<T>::get_top() {
    if (this->getSize() == 0) {
        return -1;
    }
    return this->top->data;
}

template<class T> bool St<T>::isEmpty() {
    if (this->getSize() == 0) {
        return true;
    }
    return false;
}

template<class T> int St<T>::getSize() {
    return this->count;
}

template<class T> Node<T>* St<T>::get_head() {
    return this->top;
}


inline void SymbolTable::run(string filename)
{
    ifstream fs;
    fs.open(filename);
    char line[100];
    St<int> scope_st;
    St<Object> variable_st;
    int scope = 0;
    while (!fs.eof()) {
        fs.getline(line, 100);
        string str = line;
        string instruction = line;
        int pos = 0;
        if (str == "BEGIN") {
            scope++;
            scope_st.push(scope);
        } else if (str == "END") {
            if (scope_st.isEmpty()) {
                throw UnknownBlock();
            } else {
                int top = scope_st.get_top();
                if (scope_st.getSize() > 1) {
                    scope_st.pop();
                }
                if (scope_st.get_top() != -1) {
                    Node<Object>* head = variable_st.get_head();
                    while (head != NULL) {
                        if (head->data.scope == top) {
                            int i = hash<std::string>{}(head->data.identifier) % 2069;
                            ObjectNode* h = &hashTable[i];
                            ObjectNode* p = h;
                            while (p != NULL) {
                                if (p->data.scope == top) {
                                    p->data = Object();
                                    break;
                                } else if (p->next->data.scope == top) {
                                    ObjectNode* temp = p->next;
                                    p->next = p->next->next;
                                    delete temp;
                                    break;
                                }
                                p = p->next;
                            }
                            variable_st.pop();
                            head = variable_st.get_head();
                        }
                        else {
                            break;
                        }
                    }
                    if (scope_st.get_top() == top) {
                        scope_st.pop();
                    }
                }
                scope--;
            }
        } else if (str == "RPRINT" || str == "PRINT") {
            Node<Object> *head = variable_st.get_head();
            if (head != NULL) {
                string res = "";
                while (head != NULL) {
                    if (head->data.scope == scope) {
                        char scope = (head->data.scope + '0');
                        string temp(1, scope);
                        if (str == "RPRINT") {
                            res.append(head->data.identifier + "//" + temp + " ");
                        }
                        else {
                            res.insert(0, head->data.identifier + "//" + temp + " ");
                        }
                    } else if (head->data.scope != scope && scope != 0) {
                        int pos = res.find(head->data.identifier);
                        bool check = false;
                        if (pos == -1) {
                            check = true;
                        }
                        else {
                            check = true;
                            while (pos != -1) {
                                string sub = res.substr(pos, res.find('/', pos) - pos);
                                if (pos == 0 && sub == head->data.identifier) {
                                    check = false;
                                }
                                else {
                                    if (sub == head->data.identifier && res[pos - 1] == ' ') {
                                        check = false;
                                    }
                                }
                                pos = res.find(head->data.identifier, pos+1);
                            }
                            
                        }
                        if (check) {
                            char scope = (head->data.scope + '0');
                            string temp(1, scope);
                            if (str == "RPRINT") {
                                res.append(head->data.identifier + "//" + temp + " ");
                            }
                            else {
                                res.insert(0, head->data.identifier + "//" + temp + " ");
                            }
                        }
                    }
                    head = head->next;
                }
                if (!res.empty()) {
                    res.pop_back();
                    cout << res << endl;
                }
            }      
        } else if ((pos = str.find(' ')) != -1) {
            string ACTION = str.substr(0, pos);
            str.erase(0, pos+1);
            if (ACTION == string("INSERT")) {
                string identifier = str.substr(0, str.find(' '));
                str.erase(0, str.find(' ') + 1);
                regex format("[a-z]+[\\w]*");
                if (!regex_match(identifier, format)) {
                    throw InvalidInstruction(instruction);
                }
                string type = str;
                int i = hash<std::string>{}(identifier) % 2069;
                if (hashTable[i].data.identifier == "") {
                    hashTable[i].data = Object(identifier, type, "", scope);
                    variable_st.push(hashTable[i].data);
                    cout << "success" << endl;
                } else if (hashTable[i].data.identifier != "" && hashTable[i].data.identifier == identifier) {
                    ObjectNode *head = &hashTable[i];
                    if (!this->var_collision.isExist(identifier)) {
                        this->var_collision.push(identifier);
                    }
                    if (head->next == NULL) {
                        if (head->data.identifier == identifier && head->data.scope == scope) {
                            throw Redeclared(instruction);
                        } else {
                            ObjectNode* newObject = new ObjectNode(identifier, type, "", scope);
                            head->next = newObject;
                            variable_st.push(newObject->data);
                            cout << "success" << endl;
                        }
                    } else {
                        ObjectNode *p = head;
                        while (p->next != NULL) {
                            if (p->data.identifier == identifier && p->data.scope == scope) {
                                throw Redeclared(instruction);
                            }
                            p = p->next;
                        }
                        if (p->data.identifier == identifier && p->data.scope == scope) {
                            throw Redeclared(instruction);
                        }
                        ObjectNode* newObject = new ObjectNode(identifier, type, "", scope);
                        p->next = newObject;
                        variable_st.push(newObject->data);
                        cout << "success" << endl;
                    }
                } else {
                    throw Redeclared(instruction);
                }
            } else if (ACTION == "ASSIGN") {
                string identifier = str.substr(0, str.find(' '));
                str.erase(0, str.find(' ') + 1);
                regex format("[a-z]+[\\w]*");
                if (!regex_match(identifier, format)) {
                    throw InvalidInstruction(instruction);
                }
                string value = str;
                int i = hash<std::string>{}(identifier) % 2069;
                if (regex_match(value, format)) {
                    int j = hash<std::string>{}(value) % 2069;
                    Object obj_value = lookup_identifier(value, j, scope, instruction);
                    Object obj_identifier = lookup_identifier(identifier, i, scope, instruction);
                    if (obj_identifier.type == obj_value.type) {
                        cout << "success" << endl;
                    }
                    else {
                        throw TypeMismatch(instruction);
                    }
                }
                else {
                    if (hashTable[i].data.identifier == identifier) {
                        Object obj = lookup_identifier(identifier, i, scope, instruction);
                        string type = obj.type;
                        if (type == "number") {
                            int len = value.length();
                            for (int j = 0; j <= len / 2; j++) {
                                if (value[j] < 48 || value[j] > 57 || value[len - j - 1] < 48 || value[len - j - 1] > 57)
                                {
                                    throw TypeMismatch(instruction);
                                }
                            }
                            obj.value = value;
                            cout << "success" << endl;
                        }
                        else if (type == "string") {
                            unsigned int len = value.length();
                            if (value[0] == 39 && value[len - 1] == 39) {
                                for (unsigned int j = 1; j <= (len - 1) / 2; j++) {
                                    if (((value[j] >= 97 && value[j] <= 122) ||
                                        (value[j] >= 65 && value[j] <= 90) ||
                                        (value[j] >= 48 && value[j] <= 57) ||
                                        value[j] == 32
                                        ) &&
                                        ((value[len - j - 1] >= 97 && value[len - j - 1] <= 122) ||
                                            (value[len - j - 1] >= 65 && value[len - j - 1] <= 90) ||
                                            (value[len - j - 1] >= 48 && value[len - j - 1] <= 57) ||
                                            value[len - j - 1] == 32
                                            ))
                                    {
                                        continue;
                                    }
                                    else {
                                        throw TypeMismatch(instruction);
                                    }
                                }
                                obj.value = value;
                                cout << "success" << endl;
                            }
                            else {
                                throw TypeMismatch(instruction);
                            }
                        }
                    }
                    else {
                        throw Undeclared(instruction);
                    }
                }

            } else if (ACTION == string("LOOKUP")) {
                string identifier = str;
                regex format("[a-z]+[\\w]*");
                if (!regex_match(identifier, format)) {
                    throw InvalidInstruction(instruction);
                }
                int i = hash<std::string>{}(identifier) % 2069;
                Object obj = lookup_identifier(identifier, i, scope, instruction);
                cout << obj.scope << endl;
            } else {
                throw InvalidInstruction(instruction);
            }
        } else {
            throw InvalidInstruction(instruction);
        }
    }
    if (!scope_st.isEmpty()) {
        throw UnclosedBlock(scope_st.get_top());
    }
    
}