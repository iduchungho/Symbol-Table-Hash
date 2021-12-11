#include "SymbolTable.h"


bool HashTable::const_number(const string& number) {
    int length = (int)number.length();
    for (int i = 0; i < length; i++) {
        if (number[i] < '0' || number[i] > '9') return false;
    }
    return true;
}

int HashTable::__sto_number(const string& number)
{
    if (number == "") return 0;
    int res = 0;

    for (size_t i = 0; i < number.length(); i++)
    {
        res *= 10;
        res += (int)number[i] - 48;
    }
    return res;
}

bool HashTable::const_string(const string& string) {
    int length = (int)string.length();
    if (string[0] == 39 && string[length - 1] == 39) {
        for (int j = 1; j < length - 1; ++j) {
            if (string[j] == 32) continue;
            else if (string[j] < 48 || (string[j] > 57 && string[j] < 65)
                || (string[j] > 90 && string[j] < 97) || string[j] > 122) {
                return false;
            }
        }
    }
    else return false;
    return true;
}

bool HashTable::cVariable(const string& var)
{
    regex re("([a-zA-Z_]+\\w*)");
    if (regex_match(var, re)) return true;
    return false;
}

string HashTable::cutString(string& line, const string& char_pos) {
    size_t f = line.find(char_pos);
    if (f != string::npos) {
        string s = line.substr(0, f);
        line.erase(0, f + 1);
        return s;
    }
    else {
        string _l = line;
        line.clear();
        return _l;
    }
}

void HashTable::__repush(const int& idx)
{
    this->DATA[idx].var = "";
    this->DATA[idx].level = 0;
    this->DATA[idx].numofParameter = -1;
    this->DATA[idx].type = "";
    this->DATA[idx].retType = "";
    this->DATA[idx].data = "";
    this->DATA[idx].key = -1;
}

int HashTable::__hashFunction(const long long& key, method& _method, int& count)
{
    int idx = key % _method.m;

    if (idx >= this->capacity) return idx;

    Node* dataAtidx = __data_at_idx(idx);

    if (dataAtidx->key == -1) return idx;
    else
    {
        count = 0;
        Node* correct = nullptr;
        while (!correct)
        {
            if (idx >= this->capacity || count >= this->capacity) return idx;

            if (_method.type == 1) {
                correct = LinearSearch(_method, count, idx, key);
            }
            else if (_method.type == 2) {
                correct = QuadraticSearch(_method, count, idx, key);
            }
            else if (_method.type == 3) {
                correct = DoubleMethod(_method, count, idx, key);
            }
            if ((correct && correct->key == key) || (correct && correct->key == -1))
                return idx;
            else correct = nullptr;
            count++;
        }
        return idx;
    }
    return 0;
}

Node* HashTable::__data_at_idx(const int& idx)
{
    return &this->DATA[idx];
}

int HashTable::Search(const long long& key, method& _method, int& count)
{
    int idx = key % _method.m;

    Node* correct = __data_at_idx(idx);

    if (correct && correct->key == -1) return -1;
    else if (correct && correct->key == key) return idx;

    correct = nullptr;

    count = 0;
    while (!correct)
    {
        if (idx >= this->capacity || count >= this->capacity) return -1;

        if (_method.type == 1) {
            correct = LinearSearch(_method, count, idx, key);
        }
        else if (_method.type == 2) {
            correct = QuadraticSearch(_method, count, idx, key);
        }
        else if (_method.type == 3) {
            correct = DoubleMethod(_method, count, idx, key);
        }

        if (correct && correct->key == -1) return -1;
        else if (correct && correct->key == key) return idx;
        else correct = nullptr;

        count++;
    }
    return -1;
}

int HashTable::_capacity()
{
    return this->capacity;
}

void HashTable::__push_type(const int& idx, const string& type, const string& retType)
{
    this->DATA[idx].type = type;
    this->DATA[idx].retType = retType;
}

void HashTable::__push_data(const int& idx, const string& data)
{
    this->DATA[idx].data = data;
}

void HashTable::__push_parameter(const int& idx, const int& num)
{
    this->DATA[idx].numofParameter = num;
}

void HashTable::__push_key(const int& idx, const long long& key)
{
    this->DATA[idx].key = key;
}

void HashTable::__push_block(string var, string data, string type, string retType, int idx)
{
    this->DATA[idx].var = var;
    this->DATA[idx].data = data;
    this->DATA[idx].type = type;
    this->DATA[idx].retType = retType;
}

void HashTable::__push_level(int idx, int level)
{
    this->DATA[idx].level = level;
}

int HashTable::tableSize(const string& line, method& _method)
{
    string src = line;

    string sm = cutString(src, " ");
    string sc1 = cutString(src, " ");
    string sc2 = cutString(src, " ");

    _method.m = __sto_number(sm);
    _method._const1 = __sto_number(sc1);
    _method._const2 = __sto_number(sc2);

    this->DATA = new Node[_method.m];
    this->capacity = _method.m;
    return _method._const1;
}

void HashTable::clear()
{
    if(this->DATA) 
        delete[] this->DATA;
    this->DATA = nullptr;
    this->capacity = 0;
}

int SymbolTable::function(HashTable* hash, const string& line, const string& error, const int& level, method& _method)
{
    string _line = line;
    string identifier_name = HashTable::cutString(_line, " ");
    string data = _line;
    string identifierFunction = HashTable::cutString(_line, "(");
    string parameter = HashTable::cutString(_line, ")");
    int lev = level;
    int _COUNT = 0;

    ///////// identifier name of function ///////////
    int idx_identifier_function_name = -1;

    //int idxk = HashTable::enCode(identifierFunction, lev) % _method.m;
    int count = 0;
    while (lev >= 0) {
        count = 0;
        idx_identifier_function_name = hash->Search(HashTable::enCode(identifierFunction, lev), _method, count);
        _COUNT += count;
        if (idx_identifier_function_name != -1) break;
        else
            if (_COUNT > 0)_COUNT--;
        --lev;
    }

    ///////// not found , throw error ////////////
    if (idx_identifier_function_name == -1)
    {
        hash->clear();
        delete hash;
        throw Undeclared(identifierFunction);
    }

    Node* dataOfIdentifierFunction = hash->__data_at_idx(idx_identifier_function_name);

    if (dataOfIdentifierFunction->type != "function")
    {
        hash->clear();
        delete hash;
        throw TypeMismatch(error);
    }// khong phai kieu ham hoac da co kieu tra ve == void

    // check parameter in function
    string res = "";
    int numberOfParameter = 0;
    string check = parameter;

    ////////// check type /////////////////
    while (!check.empty()) {
        string c = HashTable::cutString(check, ",");
        ++numberOfParameter;
        if (HashTable::const_number(c))
            res += "number";
        else if (HashTable::const_string(c))
            res += "string";
        else {
            bool isfunction = 0;
            regex re("\\w+");
            if (regex_match(c, re) && !HashTable::const_number(c)) isfunction = 1;
            if (isfunction) {

                int l = level, idx = -1;
                while (l >= 0) {
                    count = 0;
                    idx = hash->Search(HashTable::enCode(c, l), _method, count);
                    _COUNT += count;
                    if (idx != -1) break;
                    else
                        if (_COUNT > 0)_COUNT--;
                    --l;
                }

                if (idx != -1) {
                    Node* var_node = hash->__data_at_idx(idx);
                    res += var_node->data;
                }
            }
        }
    }

    if (dataOfIdentifierFunction->data != res && dataOfIdentifierFunction->data != "") {
        hash->clear();
        delete hash;
        throw TypeMismatch(error);
    }

    ////////////// check parameter //////////////////
    check = parameter;
    res = "";
    numberOfParameter = 0;

    while (!check.empty()) {
        string c = HashTable::cutString(check, ",");
        ++numberOfParameter;
        if (HashTable::const_number(c))
            res += "number";
        else if (HashTable::const_string(c))
            res += "string";
        else {
            bool isfunction = 0;
            regex re("\\w+");
            if (regex_match(c, re) && !HashTable::const_number(c)) isfunction = 1;
            if (isfunction) {

                int l = level, idx = -1;
                while (l >= 0) {
                    count = 0;
                    idx = hash->Search(HashTable::enCode(c, l), _method, count);
                    _COUNT += count;
                    if (idx != -1) break;
                    else
                        if (_COUNT > 0)_COUNT--;
                    --l;
                }

                if (idx == -1) {
                    hash->clear();
                    delete hash;
                    throw Undeclared(c);
                }

                Node* var_node = hash->__data_at_idx(idx);

                if (var_node->type == "") {
                    hash->clear();
                    delete hash;
                    throw TypeCannotBeInferred(error);
                }

                if (var_node->type == "function") {
                    hash->clear();
                    delete hash;
                    throw TypeMismatch(error);
                }
                else
                    res += var_node->data;
            }
        }
    }

    if (numberOfParameter != dataOfIdentifierFunction->numofParameter) {
        hash->clear();
        delete hash;
        throw TypeMismatch(error);
    }

    if (dataOfIdentifierFunction->data != res) {
        if (dataOfIdentifierFunction->data == "") {
            hash->__push_data(idx_identifier_function_name, res);
        }
        else {
            hash->clear();
            delete hash;
            throw TypeMismatch(error);
        }
    }

    /*if (dataOfIdentifierFunction->retType == "") {
        delete hash;
        throw TypeCannotBeInferred(error);
    }*/

    //////// identifier name ///////////////////////
    Node* dataOfIdentifierName = nullptr;

    //////////////// find data identifier name /////////
    int idxOfDataIdentifier = -1;
    lev = level;
    count = 0;
    long long k = 0;
    //idxk = HashTable::enCode(identifier_name, lev) % _method.m;
    for (int i = lev; i >= 0; i--)
    {
        count = 0;
        k = HashTable::enCode(identifier_name, i);
        idxOfDataIdentifier = hash->Search(k, _method, count);
        _COUNT += count;
        if (idxOfDataIdentifier != -1) break;
        else
            if (_COUNT > 0)_COUNT--;
    }

    //////// not found , throw error /////////////
    if (idxOfDataIdentifier == -1)
    {
        hash->clear();
        delete hash;
        throw Undeclared(identifier_name);
    }

    dataOfIdentifierName = hash->__data_at_idx(idxOfDataIdentifier);
    /////// identifier name is a function ////////
    if (dataOfIdentifierName->type == "function" || dataOfIdentifierFunction->retType == "void") {
        hash->clear();
        delete hash;
        throw TypeMismatch(error);
    }
    ////////////////////////////////////////////////

    /////// identifier name & function ////////////

    ////// Gan gia tri tra ve ///////

    if (dataOfIdentifierName->type == "" && dataOfIdentifierFunction->retType == "") {
        hash->clear();
        delete hash;
        throw TypeCannotBeInferred(error);
    }
    else if (dataOfIdentifierFunction->retType == "") {
        hash->__push_type(idx_identifier_function_name, "function", dataOfIdentifierName->type);
    }
    else if (dataOfIdentifierName->type == "") {
        hash->__push_type(idxOfDataIdentifier, dataOfIdentifierFunction->retType, "");
    }

    return _COUNT;
}

HashTable::HashTable()
{
    this->DATA = nullptr;
    this->capacity = 0;
}

Node* HashTable::LinearSearch(method& _method, const int& count, int& idx, const long long& key)
{
    idx = ((key % _method.m) + (count * _method._const1)) % _method.m;

    Node* __data = __data_at_idx(idx);
    return __data;

}

Node* HashTable::QuadraticSearch(method& _method, const int& count, int& idx, const long long& key)
{
    idx = ((key % _method.m) + _method._const1 * count + _method._const2 * count * count) % _method.m;

    Node* __data = __data_at_idx(idx);
    return __data;
}

Node* HashTable::DoubleMethod(method& _method, const int& count, int& idx, const long long& key)
{
    idx = ((key % _method.m) + count * _method._const1 * (1 + (key % (_method.m - 2)))) % _method.m;

    Node* __data = __data_at_idx(idx);
    return __data;
}

long long HashTable::enCode(const string& line, const int& level)
{
    /*  xB//1 - 48
    *   return enCode level-var
    *   1-72-18
    *   1  x  B
    */
    string s = line;
    string var = cutString(s, " ");
    long long res = level;
    //int var_l = (int)var.length();
    //int enCode[100];

    for (size_t i = 0; i < var.length(); i++)
    {
        int n = (int)var[i] - (int)48;
        //enCode[i] = n;
        if (n >= 10 && n < 100) res *= 100;
        else if (n >= 100) res *= 1000;
        else if (n < 10) res *= 10;

        res += n;
    }

    //delete[] enCode;
    return res;
}

int SymbolTable::INSERT(const string& line, HashTable* hash, const int& level, method& _method, bool& f)
{
    //int idx = 0;
    string _line = line;
    string identifier_name = HashTable::cutString(_line, " ");
    string num_of_parameter = HashTable::cutString(_line, " ");

    int _COUNT = 0;
    long long key = hash->enCode(line, level);
    //idx = key % _method.m;
    int idx_hash = hash->__hashFunction(key, _method, _COUNT);

    int n_num_of_parameter;
    if (num_of_parameter != "")
        n_num_of_parameter = HashTable::__sto_number(num_of_parameter);
    else{
        n_num_of_parameter = -1;
    }

    Node* curr = nullptr;
    if (idx_hash < hash->_capacity())
        curr = hash->__data_at_idx(idx_hash);

    if (curr) {
        if (curr->key == key) {
            hash->clear();
            delete hash;
            throw Redeclared(identifier_name);
        }
    }

    if (n_num_of_parameter != -1 && level != 0) {
        hash->clear();
        delete hash;
        throw InvalidDeclaration(identifier_name);
    }

    if (idx_hash >= hash->_capacity() || _COUNT >= hash->_capacity()) {
        hash->clear();
        delete hash;
        throw Overflow("INSERT " + line);
    }

    if (n_num_of_parameter != -1) {
        hash->__push_block(identifier_name, "", "function", "", idx_hash);
        hash->__push_parameter(idx_hash, n_num_of_parameter);
        hash->__push_key(idx_hash, key);
        hash->__push_level(idx_hash, level);
    }
    else {
        hash->__push_block(identifier_name, "", "", "", idx_hash);
        hash->__push_parameter(idx_hash, n_num_of_parameter);
        hash->__push_key(idx_hash, key);
        hash->__push_level(idx_hash, level);
    }

    f = 1;
    return _COUNT;
}

int SymbolTable::ASSIGN(const string& line, HashTable* hash, const int& level, method& _method, bool& f)
{
    string _line = line;
    string identifier_name = HashTable::cutString(_line, " ");
    string value = _line;
    string error = "ASSIGN " + line;
    int _COUNT = 0,
        count = 0;


    size_t flag = line.find("(");
    if (flag != string::npos) return function(hash, line, error, level, _method);
    else
    {
        //////////// value //////////////
        //int hashValue = 0;
        Node* dataOfValue = nullptr;
        int idxOfDataValue = -1;
        if (!HashTable::const_string(value) && !HashTable::const_number(value))
        {

            //////////////// find data value ////////////////////
            int lev = level;
            long long k = 0;
            //idxkey = HashTable::enCode(value, lev) % _method.m;
            for (int i = lev; i >= 0; i--)
            {
                count = 0;
                k = HashTable::enCode(value, i);
                idxOfDataValue = hash->Search(k, _method, count);
                _COUNT += count;
                if (idxOfDataValue != -1) break;
                else
                    if (_COUNT > 0) _COUNT--;
            }

            ///// not found ,throw error ///
            if (idxOfDataValue == -1) {
                hash->clear();
                delete hash;
                throw Undeclared(value);
            }

            dataOfValue = hash->__data_at_idx(idxOfDataValue);

            ///////// tham so la ham /////////
            if (dataOfValue->type == "function") {
                hash->clear();
                delete hash;
                throw TypeMismatch(error);
            }
            ////////////////////////////////////////////////

        }

        /////////// identifier name ///////////
        Node* dataOfIdentifierName = nullptr;

        //////////////// find data identifier name /////////
        int lev = level, idxOfDataIdentifier = 0;
        long long k = 0;
        //idxk = HashTable::enCode(identifier_name, lev) % _method.m;
        count = 0;
        for (int i = lev; i >= 0; i--)
        {
            count = 0;
            k = HashTable::enCode(identifier_name, i);
            idxOfDataIdentifier = hash->Search(k, _method, count);
            _COUNT += count;
            if (idxOfDataIdentifier != -1) break;
            else
                if (_COUNT > 0)_COUNT--;
        }

        //////// not found , throw error /////////////
        if (idxOfDataIdentifier == -1)
        {
            hash->clear();
            delete hash;
            throw Undeclared(identifier_name);
        }

        dataOfIdentifierName = hash->__data_at_idx(idxOfDataIdentifier);
        ////////////////////////////////////////////////


        if (dataOfIdentifierName->type == "function") {
            hash->clear();
            delete hash;
            throw TypeMismatch(error);
        }


        //////// check variable ////////
        if (dataOfValue) {

            if (dataOfIdentifierName->type != dataOfValue->type && dataOfIdentifierName->type != "") {
                hash->clear();
                delete hash;
                throw TypeMismatch(error);
            }

            if (dataOfIdentifierName->type == "" && dataOfValue->type == "") {
                hash->clear();
                delete hash;
                throw TypeCannotBeInferred(error);
            }

            if (dataOfIdentifierName->type == "" && dataOfValue->type != "") {
                hash->__push_data(idxOfDataIdentifier, dataOfValue->data);
                hash->__push_type(idxOfDataIdentifier, dataOfValue->type, "");
            }

            return _COUNT;
        }
        /////// check const string or number //////////

        if (HashTable::const_number(value)) {
            hash->__push_data(idxOfDataIdentifier, "number");
            hash->__push_type(idxOfDataIdentifier, "number", "");
        }
        else if (HashTable::const_string(value)) {
            hash->__push_data(idxOfDataIdentifier, "string");
            hash->__push_type(idxOfDataIdentifier, "string", "");
        }
        return _COUNT;
    }
    return -1;
}

int SymbolTable::CALL(const string& line, HashTable* hash, const int& level, method& _method)
{
    string _line = line;
    string identifierFunction = HashTable::cutString(_line, "(");
    string parameter = HashTable::cutString(_line, ")");
    int lev = level;
    int _COUNT = 0, idx_of_func = -1, count = 0;

    ///////// find identifier function ////////
    //////// boi vi ham luon khai bao o muc 0 len level = 0///

    //int idx = HashTable::enCode(identifierFunction, level) % _method.m;
    while (lev >= 0) {
        count = 0;
        idx_of_func = hash->Search(HashTable::enCode(identifierFunction, lev), _method, count);
        _COUNT += count;
        if (idx_of_func != -1) break;
        else
            if(_COUNT > 0) _COUNT--;
        --lev;
    }


    //////// error if idx_of_func == -1 //////
    if (idx_of_func == -1 || idx_of_func >= hash->_capacity()) {
        hash->clear();
        delete hash;
        throw Undeclared(identifierFunction);
    }

    Node* funcNode = hash->__data_at_idx(idx_of_func);
    if (funcNode->key == -1) {
        hash->clear();
        delete hash;
        throw Undeclared(identifierFunction);
    }
    int idx_funcNode = idx_of_func;

    ///////// if func is not function //////////
    if (funcNode->type != "function") {
        hash->clear();
        delete hash;
        throw TypeMismatch("CALL " + line);
    }


    //////// check function in function ////////
    string res = "";
    string _check = parameter;
    int numberOfparameter = 0;

    ///////// check type /////////////
    while (!_check.empty()) {
        string c = HashTable::cutString(_check, ",");
        ++numberOfparameter;
        if (HashTable::const_number(c))
            res += "number";
        else if (HashTable::const_string(c))
            res += "string";
        else {
            bool isfunction = 0;
            regex re("\\w+");
            if (regex_match(c, re) && !HashTable::const_number(c)) isfunction = 1;
            if (isfunction) {

                int l = level, idx = -1;
                while (l >= 0) {
                    count = 0;
                    idx = hash->Search(HashTable::enCode(c, l), _method, count);
                    _COUNT += count;
                    if (idx != -1) break;
                    else
                        if (_COUNT > 0)_COUNT--;
                    --l;
                }

                if (idx != -1) {
                    Node* var_node = hash->__data_at_idx(idx);
                    res += var_node->data;
                }
            }
        }
    }

    if (funcNode->data != res && funcNode->data != "") {
        hash->clear();
        delete hash;
        throw TypeMismatch("CALL " + line);
    }
    //////// check parameter///////////
    res = "";
    _check = parameter;
    numberOfparameter = 0;

    while (!_check.empty()) {
        string c = HashTable::cutString(_check, ",");
        ++numberOfparameter;
        if (HashTable::const_number(c))
            res += "number";
        else if (HashTable::const_string(c))
            res += "string";
        else {
            bool isVariable = 0;
            Node* var_node = nullptr;
            regex re("\\w+");
            if (regex_match(c, re) && !HashTable::const_number(c)) isVariable = 1;
            if (isVariable) {

                int l = level, idx = -1;
                while (l >= 0) {
                    count = 0;
                    idx = hash->Search(HashTable::enCode(c, l), _method, count);
                    _COUNT += count;
                    if (idx != -1) break;
                    else
                        if (_COUNT > 0)_COUNT--;
                    --l;
                }

                if (idx == -1) {
                    hash->clear();
                    delete hash;
                    throw Undeclared(c);
                }

                var_node = hash->__data_at_idx(idx);

                if (var_node->type == "") {;
                    hash->clear();
                    delete hash;
                    throw TypeCannotBeInferred("CALL " + line);
                }

                if (var_node->type == "function") {
                    hash->clear();
                    delete hash;
                    throw TypeMismatch("CALL " + line);
                }
                else
                    res += var_node->data;
            }
        }
    }

    if (numberOfparameter != funcNode->numofParameter) {
        hash->clear();
        delete hash;
        throw TypeMismatch("CALL " + line);
    }

    if (funcNode->data != res) {
        if (funcNode->data == "") {
            hash->__push_data(idx_of_func, res);
        }
        else if (funcNode->data != "") {
            hash->clear();
            delete hash;
            throw TypeMismatch("CALL " + line);
        }
    }

    /////// khong dung kieu tra ve
    if (funcNode->retType != "void" && funcNode->type != "function") {
        hash->clear();
        delete hash;
        throw TypeMismatch("CALL " + line);
    }
    hash->__push_type(idx_funcNode, "function", "void");

    /*string check = parameter;
    while (!check.empty()) {
        string c = HashTable::cutString(check, ",");
        if (!HashTable::const_number(c) && !HashTable::const_string(c)) {
            hash->clear();
            delete hash;
            throw TypeMismatch("CALL " + line);
        }
    }*/
    return _COUNT;
}

void SymbolTable::PRINT(HashTable* hash)
{
    int cap = hash->_capacity();
    bool f = 0;
    for (int i = 0; i < cap; i++)
    {
        Node* out = hash->__data_at_idx(i);
        if (out->var != "") {
            if (!f) {
                f = 1;
                cout << i << " " << out->var << "//" << out->level;
            }
            else {
                cout << ";" << i << " " << out->var << "//" << out->level;
            }
        }
    }
    if (f) cout << endl;
}

int SymbolTable::LOOKUP(const string& line, HashTable* hash, const int& level, method& _method)
{
    int lev = level, idx = 0;
    string var = line;
    int _COUNT = 0,
        count = 0;
    while (lev >= 0) {
        count = 0;
        long long key = HashTable::enCode(var, lev);
        idx = hash->Search(key, _method, count);
        _COUNT += count;
        if (idx != -1) break;
        else
            if (_COUNT > 0)_COUNT--;
        --lev;
    }
    if (idx == -1) {
        hash->clear();
        delete hash;
        throw Undeclared(line);
    }
    return idx;
    return 0;
}

void SymbolTable::run(string filename)
{
    fstream file;
    string line, error;
    HashTable* Hash = new HashTable();
    method _method;

    //int nVariableOfblock = 0;
    int nParameter = 0;
    int level = 0;
    bool flagInsert = 0;

    file.open(filename, ios::in);
    while (!file.eof())
    {
        getline(file, line);
        bool f = SyntaxError(line, nParameter);
        if (!f) {
            Hash->clear();
            delete Hash;
            throw InvalidInstruction(line);
        }

        error = line;
        string type = HashTable::cutString(line, " ");

        if (type == "LINEAR")
        {
            _method.type = 1;
            Hash->tableSize(line, _method);
        }
        else if (type == "QUADRATIC")
        {
            _method.type = 2;
            Hash->tableSize(line, _method);
        }
        else if (type == "DOUBLE")
        {
            _method.type = 3;
            Hash->tableSize(line, _method);
        }
        else if (type == "INSERT")
        {
            cout << INSERT(line, Hash, level, _method, flagInsert) << endl;
        }
        else if (type == "ASSIGN")
        {
            cout << ASSIGN(line, Hash, level, _method, flagInsert) << endl;
        }
        else if (type == "PRINT")
        {
            PRINT(Hash);
        }
        else if (type == "CALL")
        {
            cout << CALL(line, Hash, level, _method) << endl;
        }
        else if (type == "LOOKUP")
        {
            cout << LOOKUP(line, Hash, level, _method) << endl;
        }
        else if (type == "BEGIN")
        {
            ++level;
        }
        else if (type == "END")
        {
            //int l = 0;

            for (int i = 0; i < Hash->_capacity(); i++) {
                Node* f = Hash->__data_at_idx(i);
                if (f && f->level == level) {
                    Hash->__repush(i);
                }
                //Node* fa = Hash->__data_at_idx(i);
            }

            --level;
            if (level < 0)
            {
                Hash->clear();
                delete Hash;
                throw UnknownBlock();
            }
        }
    }
    if (level > 0)
    {
        Hash->clear();
        delete Hash;
        throw UnclosedBlock(level);
    }
    file.close();
    Hash->clear();
    delete Hash;
}

bool SymbolTable::SyntaxError(const string& line, int& nParameter)
{
    bool correct = 0;
    if (!correct)
    {
        regex re("INSERT ([a-z]+\\w*)( (\\d+))*");
        if (regex_match(line, re)) correct = 1;
        else correct = 0;
    }
    if (!correct)
    {
        regex re1("ASSIGN ([a-z]+\\w*) (([a-z]+\\w*)\\(((([a-z]+\\w*)|(\\d*)|('[a-zA-Z0-9_ ]*')))(,((([a-z]+\\w*)|(\\d*)|('[a-zA-Z0-9_ ]*'))))*\\)|\\d+|('[a-zA-Z0-9_ ]*')|([a-z]+\\w*))");
        if (regex_match(line, re1)) correct = 1;
        else correct = 0;
    }
    if (!correct)
    {
        regex re2("BEGIN");
        if (regex_match(line, re2)) correct = 1;
        else correct = 0;
    }
    if (!correct)
    {
        regex re3("END");
        if (regex_match(line, re3)) correct = 1;
        else correct = 0;
    }
    if (!correct)
    {
        regex re4("PRINT");
        if (regex_match(line, re4)) correct = 1;
        else correct = 0;
    }
    if (!correct)
    {
        regex re5("LOOKUP ([a-z]+\\w*)");
        if (regex_match(line, re5)) correct = 1;
        else correct = 0;
    }
    if (!correct)
    {
        regex re7("CALL ([a-z]+\\w*)\\(((([a-z]+\\w*)|(\\d*)|('[a-zA-Z0-9_ ]*')))(,((([a-z]+\\w*)|(\\d*)|('[a-zA-Z0-9_ ]*'))))*\\)");
        if (regex_match(line, re7)) correct = 1;
        else correct = 0;
    }
    if (!correct) {
        regex re6("LINEAR \\d{1,6} \\d{1,6}");
        if (regex_match(line, re6))
        {
            ++nParameter;
            correct = 1;
            //isParameter = 1;
        }
    }
    if (!correct) {
        regex re6("QUADRATIC \\d{1,6} \\d{1,6} \\d{1,6}");
        if (regex_match(line, re6))
        {
            ++nParameter;
            correct = 1;
            //isParameter = 1;
        }
    }
    if (!correct) {
        regex re6("DOUBLE \\d{1,6} \\d{1,6}");
        if (regex_match(line, re6))
        {
            ++nParameter;
            correct = 1;
            //isParameter = 1;
        }
    }
    if (nParameter == 0 && correct)
    {
        return 0;
    }
    else if (nParameter > 1)
    {
        return 0;
    }
    else if (!correct)
    {
        return 0;
    }
    return 1;
}
