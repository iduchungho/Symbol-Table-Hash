#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"
struct Node
{
    string var, data, type, retType;
    long long key = -1;
    int numofParameter = -1, level = 0;
    Node(long long k = -1, int n = -1)
        :key(k), numofParameter(n) {};
};
struct method
{
    int type, _const1, _const2, m;
};
class HashTable
{
private:
    int capacity;
    Node* DATA;
    Node* LinearSearch(method& _method, const int& count, int& idx, const long long& key);
    Node* QuadraticSearch(method& _method, const int& count, int& idx, const long long& key);
    Node* DoubleMethod(method& _method, const int& count, int& idx, const long long& key);
public:
    HashTable();

    int _capacity();
    void __repush(const int& idx);

    int __hashFunction(const long long& key, method& _method, int& count);

    Node* __data_at_idx(const int& idx);
    int Search(const long long& key, method& _method, int& count);
    int tableSize(const string& line, method& _method);
    void clear();
    void __push_type(const int& idx, const string& type, const string& retType);
    void __push_data(const int& idx, const string& data);
    void __push_parameter(const int& idx, const int& num);
    void __push_key(const int& idx, const long long& key);
    void __push_block(string var, string data, string tpye, string retType, int idx);
    void __push_level(int idx, int level);
    static long long enCode(const string& line, const int& level);
    static int __sto_number(const string& number);
    static bool const_number(const string& number);
    static bool const_string(const string& string);
    static bool cVariable(const string& var);
    static string cutString(string& line, const string& char_pos);
};

class SymbolTable
{
private:
    int INSERT(const string& line, HashTable* hash, const int& level, method& _method, bool& f);
    int ASSIGN(const string& line, HashTable* hash, const int& level, method& _method, bool& f);
    int function(HashTable* hash, const string& line, const string& error, const int& level, method& _method);
    int CALL(const string& line, HashTable* hash, const int& level, method& _method);
    int LOOKUP(const string& line, HashTable* hash, const int& level, method& _method);
    void PRINT(HashTable* hash);
public:
    SymbolTable() {}
    void run(string filename);
    static bool SyntaxError(const string& line, int& parameter);
};
#endif