#ifndef PARSER_H
#define PARSER_H

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <chrono>
#include <cctype>
#include <vector>
#include <initializer_list>
#include "Sequential.h"
#include "../../../Downloads/AVL.h"
//#include "ExtendibleHash.h"


using namespace std;

string KEY = "id";


void savekey(string key, string tabla){
    ofstream archivo("tablekeys.txt");

    if (archivo.is_open()) {
        archivo << "Tabla: " << tabla << endl;
        archivo << "Key: " << key << endl;
        archivo.close();
        cout << "Datos guardados en el archivo." << endl;
    } else {
        cout << "No se pudo abrir el archivo." << endl;
    }
}


string findTkey(const string& tableName) {
    std::ifstream archivo("tablekeys.txt");

    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return ""; // Devuelve una cadena vac�a si no se pudo abrir el archivo.
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        size_t pos = linea.find("Tabla: " + tableName);
        if (pos != std::string::npos) {
            std::string keyLine;
            if (std::getline(archivo, keyLine)) {
                size_t keyPos = keyLine.find("Key: ");
                if (keyPos != std::string::npos) {
                    archivo.close();
                    return keyLine.substr(keyPos + 5);
                }
            }
        }
    }

    archivo.close();
    std::cerr << "No se encontr� la tabla en el archivo." << std::endl;
        return "";
}


// L�gica para leer el archivo CSV y cargar los datos en una estructura
template <class STR>
void readCSVFile(const string& filename, STR& structure) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Leer la primera l�nea (encabezados) y descartarla

    while (getline(file, line)) {
        istringstream ss(line);
        string id_str,id2_str, name_str, type1_str, type2_str, height_str, weight_str, base_exp_str, attack_str, defense_str;

        if (getline(ss, id_str, ',') && getline(ss, name_str, ',') && getline(ss, id2_str, ',') &&
            getline(ss, type1_str, ',') && getline(ss, type2_str, ',') &&
            getline(ss, height_str, ',') && getline(ss, weight_str, ',') &&
            getline(ss, base_exp_str, ',') && getline(ss, attack_str, ',') &&
            getline(ss, defense_str, ',')) {

            Record record;
            record.id = stoi(id_str);
            strncpy(record.name, name_str.c_str(), sizeof(record.name));
            strncpy(record.type1, type1_str.c_str(), sizeof(record.type1));
            strncpy(record.type2, type2_str.c_str(), sizeof(record.type2));
            record.height = stoi(height_str);
            record.weight = stoi(weight_str);
            record.base_exp = stoi(base_exp_str);
            record.attack = stoi(attack_str);
            record.defense = stoi(defense_str);
            structure.add(record);
        } else {
            cerr << "Error al analizar la l�nea: " << line << endl;
        }
    }

    file.close();
}



class Token {
public:
    enum Type { ID, ERR, END, NUM, SELECT, FROM, WHERE, INSERT, DELETE, INTO, SET, VALUES, AND, OR, NOT, GT, LT, GE, LE, EQ, NEQ,
    LPAREN, RPAREN, COMILLAS, USING, INDEX, BETWEEN, CAMPO, CREATE, TABLE, FILE, COMA, BOOL };
    static const char* token_names[34];
    Type type;
    string lexema;
    Token(Type);
    Token(Type, char c);
    Token(Type, const string source);
};

const char* Token::token_names[34] = { "ID", "ERR", "END", "NUM", "SELECT", "FROM", "WHERE", "INSERT", "DELETE", "INTO", "SET", "VALUES", "AND", "OR", "NOT", "GT", "LT", "GE",
"LE", "EQ", "NEQ", "LPAREN", "RPAREN", "COMILLAS", "USING", "INDEX", "BETWEEN", "CAMPO", "CREATE", "TABLE", "FILE", "COMA", "BOOL" };

Token::Token(Type type):type(type){
    lexema = "";
}

Token::Token(Type type, const string source):type(type) {
    lexema = source;
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    if (tok.lexema.empty())
        return outs << Token::token_names[tok.type];
    else
        return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}

class Scanner {
public:
    Scanner(const char* in_s);
    Token* nextToken();
    ~Scanner();
private:
    string input;
    int first, current;
    int state;
    char nextChar();
    void rollBack();
    void startLexema();
    void incrStartLexema();
    string getLexema();
    map<string, Token::Type> keywords;
};

Scanner::Scanner(const char* s):input(s),first(0),current(0) {
    keywords["SELECT"] = Token::SELECT;
    keywords["FROM"] = Token::FROM;
    keywords["WHERE"] = Token::WHERE;
    keywords["INSERT"] = Token::INSERT;
    keywords["DELETE"] = Token::DELETE;
    keywords["INTO"] = Token::INTO;
    keywords["SET"] = Token::SET;
    keywords["VALUES"] = Token::VALUES;
    keywords["AND"] = Token::AND;
    keywords["OR"] = Token::OR;
    keywords["NOT"] = Token::NOT;
    keywords["USING"] = Token::USING;
    keywords["INDEX"] = Token::INDEX;
    keywords["BETWEEN"] = Token::BETWEEN;
    keywords["CAMPO"] = Token::CAMPO;
    keywords["CREATE"] = Token::CREATE;
    keywords["TABLE"] = Token::TABLE;
    keywords["FILE"] = Token::FILE;
}

char Scanner::nextChar() {
    int c = input[current];
    if (c != '\0')
        current++;
    return c;
}

void Scanner::rollBack() { // retract
    if (input[current] != '\0')
        current--;
}

void Scanner::startLexema() {
    first = current-1;
    return;
}

void Scanner::incrStartLexema() {
    first++;
}

Scanner::~Scanner() { }

string Scanner::getLexema() {
    string res = input.substr(first,current-first);
    size_t n = res.size();

    if (res.front() == '"' and res.back() == '"') {
        res = res.substr(1, res.length() - 2);
    }

    if (res[n-1] == ')'){
        res = res.substr(0, n-1);
        current--;
    }

    if (res[n-1] == '\n'){
        res = res.substr(0, n-1);
        current--;
    }

    return res;
}

Token* Scanner::nextToken() {
    Token* token;
    char c;
    c = nextChar();

    while (c == ' ' || c == '\t' || c == '\n')  c = nextChar();

    if(c == '#'){
        while (c != '\n')   c = nextChar();
        c = nextChar();
    }

    startLexema();

    if(c == '*')
        return new Token(Token::CAMPO, getLexema());
    if(c == ',')
        return new Token(Token::COMA);
    if(c == '"'){
        c = nextChar();
        while (c != '"') c = nextChar();
        return new Token(Token::ID, getLexema());
    }
    if(c == '(') return new Token(Token::LPAREN);
    if(c == ')') return new Token(Token::RPAREN);
    if(c == '<'){
        c = nextChar();
        if(c == '=') return new Token(Token::LE);
        return new Token(Token::LT);
    }
    if(c == '>'){
        c = nextChar();
        if(c == '=') return new Token(Token::GE);
        return new Token(Token::GT);
    }
    if(c == '='){
        c = nextChar();
        if(c == '>') return new Token(Token::GE);
        if(c == '<') return new Token(Token::LE);
        return new Token(Token::EQ);
    }
    if (isalpha(c)) {
        c = nextChar();
        while (isdigit(c) || isalpha(c)) c = nextChar();
        rollBack();
        if(getLexema() == "true" or getLexema() == "false") return new Token(Token::BOOL, getLexema());
        else if(keywords[getLexema()] == Token::ID) return new Token(Token::CAMPO, getLexema());
        else return new Token(keywords[getLexema()]);

    } else if (isdigit(c)) {
        c = nextChar();
        while (isdigit(c)) c = nextChar();
        rollBack();

        return new Token(Token::NUM, getLexema());

    } else if (c ==  '\0') {
        return new Token(Token::END);

    }else {
        return new Token(Token::ERR, getLexema());
    }
}


void parseInsertStatement();
void parseDeleteStatement();
///-------------------------PARSER----------------------------
class Parser {
    Scanner scanner;
    Token* currentToken;
    Token* previousToken;

public:
    Parser(const string& input) : scanner(input.c_str()), currentToken(nullptr), previousToken(nullptr) {}

    void parse(vector<Record>& records) {
        currentToken = scanner.nextToken();
        if (currentToken->type == Token::SELECT) {
            records = parseSelectStatement();
            return;
        }
        else if (currentToken->type == Token::CREATE) {
            parseCreateStatement();
            return;
        }
        else if (currentToken->type == Token::INSERT) {
            parseInsertStatement();
            return;
        }
        else if (currentToken->type == Token::DELETE) {
            parseDeleteStatement();
            return;
        }
    }

    ~Parser(){
        if (previousToken) {
            delete previousToken;
            previousToken = nullptr;
        }
        if (currentToken) {
            delete currentToken;
            currentToken = nullptr;
        }
    }

private:
    void advance() {
        previousToken = currentToken;
        currentToken = scanner.nextToken();
    }

    void expect(Token::Type expectedType) {
        if (currentToken && currentToken->type == expectedType) {
            advance();
        } else {
            cout << "Error: Unexpected token" << endl;
            exit(1);
        }
    }

    void expect(initializer_list<Token::Type> expectedTypes) {
        if (currentToken) {
            for (Token::Type type : expectedTypes) {
                if (currentToken->type == type) {
                    advance();
                    return;
                }
            }
        }

        cout << "Error: Unexpected token" << endl;
        exit(1);
    }

    vector<Record> parseSelectStatement(){
        vector<Record> found;

        vector<string> campos;
        advance();
        expect(Token::CAMPO);
        campos.push_back(previousToken->lexema);
        string tabla;

        while(currentToken->type == Token::COMA) {
            advance();  // Avanzar sobre la coma
            expect(Token::CAMPO);
            campos.push_back(previousToken->lexema);
        }

        expect(Token::FROM);
        expect(Token::CAMPO);

        tabla = previousToken->lexema;
        KEY = findTkey(tabla);

        if(currentToken->type == Token::USING){
            advance();
            expect(Token::CAMPO);
            if(currentToken->type == Token::END){
                if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                    Sequential f(tabla);
                    found = f.inorder();
                }else if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                    AVLFile f(tabla);
                    found = f.inorder();
                }//else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
                  //  DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
                  //  found = f.inorder();
                //}
            }
            return found;
        }

        // Consultas condicionadas
        if(currentToken->type == Token::WHERE)
            advance();

        string campo;

        expect(Token::CAMPO);
        campo = previousToken->lexema;

        expect({Token::GT, Token::GE, Token::LT, Token::LE, Token::EQ, Token::BETWEEN});
        if(previousToken->type == Token::EQ){
            string value;
            expect({Token::NUM, Token::ID, Token::BOOL});
            value = previousToken->lexema;
            expect(Token::USING);
            expect(Token::CAMPO);
            if(currentToken->type == Token::END){
                if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                    Sequential f(tabla);
                    found = f.search(stoi(value));
                }else if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                    AVLFile f(tabla);
                    found = f.search(stoi(value));
                }//else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
                  //  DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
                  // found.push_back(f.search(stoi(value)));
                //}
            }
            return found;
        }else if(previousToken->type == Token::BETWEEN){
            string begin, end;
            expect(Token::LPAREN);
            expect({Token::NUM, Token::ID});
            begin = previousToken->lexema;
            expect(Token::COMA);
            expect({Token::NUM, Token::ID});
            end = previousToken->lexema;
            expect(Token::RPAREN);
            expect(Token::USING);
            expect(Token::CAMPO);
            if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                AVLFile f(tabla);
                found = f.rangeSearch(stoi(begin), stoi(end));
            }else if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                Sequential f(tabla);
                found = f.rangeSearch(stoi(begin), stoi(end));
            }

            return found;
        }

    };

    void parseCreateStatement(){
        string tabla, filename, index, key;
        advance();
        expect(Token::TABLE);
        expect(Token::CAMPO);
        tabla = previousToken->lexema;
        expect(Token::FROM);
        expect(Token::FILE);
        expect(Token::ID);
        filename = previousToken->lexema;
        expect(Token::USING);
        expect(Token::INDEX);
        expect(Token::CAMPO);
        index = previousToken->lexema;
        expect(Token::LPAREN);
        expect(Token::ID);
        key = previousToken->lexema;
        KEY = key;
        expect(Token::RPAREN);
        if(currentToken->type == Token::END){
            if(index == "AVL"){
                savekey(key, tabla);
                AVLFile indice(tabla);
                readCSVFile<AVLFile>(filename, indice);
                cout << "Tabla creada con nombre " << tabla << endl;
            }else if(index == "SEQUENTIAL"){
                savekey(key, tabla);
                Sequential indice(tabla);
                readCSVFile<Sequential>(filename, indice);
                cout << "Tabla creada con nombre " << tabla << endl;
            }//else if(index== "HASH" or index == "hash"){
                //savekey(key, tabla);
               // DynamicHash indice(tabla + ".bin", tabla + "index.bin", D, FB);
              //  readCSVFile<DynamicHash>(filename, indice);
             //   cout << "Tabla creada con nombre " << tabla << endl;
            //}
            else{
                cout << "Indice no existente";
            }
        }
    };

    void parseInsertStatement(){
        Record record;
        string tabla;

        advance();
        expect(Token::INTO);
        expect(Token::CAMPO);
        tabla = previousToken->lexema;
        expect(Token::VALUES);
        expect(Token::LPAREN);
        expect(Token::NUM);
        record.id = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::ID);
        strncpy(record.name, previousToken->lexema.c_str(), sizeof(record.name));
        expect(Token::COMA);
        expect(Token::NUM);
        record.height = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::NUM);
        record.weight = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::NUM);
        record.base_exp = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::ID);
        strncpy(record.type1, previousToken->lexema.c_str(), sizeof(record.type1));
        expect(Token::COMA);
        expect(Token::ID);
        strncpy(record.type2, previousToken->lexema.c_str(), sizeof(record.type2));
        expect(Token::COMA);
        expect(Token::NUM);
        record.attack = stoi(previousToken->lexema);
        expect(Token::COMA);
        expect(Token::NUM);
        record.defense = stoi(previousToken->lexema);
        expect(Token::RPAREN);
        expect(Token::USING);
        expect(Token::CAMPO);
        KEY = findTkey(tabla);
        if(currentToken->type == Token::END){
            if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                AVLFile f(tabla);
                f.add(record);
            }else if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                Sequential f(tabla);
                f.add(record);
            }//else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
              //  DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
              //  f.add(record);
            //}
        }
    };

    void parseDeleteStatement(){
        string campo, tabla;
        advance();
        expect(Token::FROM);
        expect(Token::CAMPO);
        tabla = previousToken->lexema;
        KEY = findTkey(tabla);
        expect(Token::WHERE);

        expect(Token::CAMPO);
        campo = previousToken->lexema;

        expect({Token::GT, Token::GE, Token::LT, Token::LE, Token::EQ, Token::BETWEEN});
        if(previousToken->type == Token::EQ){
            string value;
            expect({Token::NUM, Token::ID, Token::BOOL});
            value = previousToken->lexema;
            expect(Token::USING);
            expect(Token::CAMPO);
            if(currentToken->type == Token::END){
                if(previousToken->lexema == "SEQUENTIAL" or previousToken->lexema == "sequential"){
                    Sequential f(tabla);
                    f.remove(stoi(value));
                }else if(previousToken->lexema == "AVL" or previousToken->lexema == "avl"){
                    AVLFile f(tabla);
                    f.remove(stoi(value));
                }//else if(previousToken->lexema == "HASH" or previousToken->lexema == "hash"){
                  //  DynamicHash f(tabla + ".bin", tabla + "index.bin", D, FB);
                 //   f.remove(stoi(value));
                //}
            }
        }
    };

};

#endif // PARSER_H
