// https://ide.usaco.guide/OMwdBnLGcGkjXAqrL9u

#include <bits/stdc++.h>
using namespace std;

// struct for tokens in the tokenizer
struct Token {
    string type; // the type of the token
    string tok; // the actual token
};

// struct for the recursive rules, basically allowing for two typed list
// plan is that if token is null, you can look at the other two, otherwise you just look at the token
// you use type and val for the big things, and token for individual tokens
struct Node {
    string type;
    vector<Node> vals;
    Token token;

    Node(Token newToken){
        type = "token";
        token = newToken;
    }

    Node(string newType){
        type = newType;
    }
};

// set input and output file name
void setIO(string s) {
	freopen((s + ".in").c_str(), "r", stdin);
	freopen((s + ".out").c_str(), "w", stdout);
}

// general helper functions
bool isAlphabetic(char c);
bool isNumeric(char c);
bool isAlphanumeric(char c);
bool isWhite(char c);
bool strEq(string a, string b);
bool strIn(string str, vector<string> lis);

// outputs the end of the next token given a position and a fsm function
int eot(string str, int i, int (*fsm)(int, char));

// the fsm functions used to find the end of tokens:
int fsm_ki(int state, char c); // used for keywords and identifiers
int fsm_str(int state, char c); // used for string constants
int fsm_int(int state, char c); // used for integer constants
int fsm_com(int state, char c); // used for block comments
int fsm_bs(int state, char c); // used for string constants, has the feature of backslash working

/*
parser functions:
plan to have them return the new index of the token list
they take in the pointer to the parse vector so they can update it within the function
*/

// Program Structure:
int parseClass(vector<Node>* parse, vector<Token> tokens, int idx);
int parseClassVarDec(vector<Node>* parse, vector<Token> tokens, int idx);
int parseSubroutineDec(vector<Node>* parse, vector<Token> tokens, int idx);
int parseParameterList(vector<Node>* parse, vector<Token> tokens, int idx);
int parseSubroutineBody(vector<Node>* parse, vector<Token> tokens, int idx);
int parseVarDec(vector<Node>* parse, vector<Token> tokens, int idx);

// Statements:



void tests();

// using finite state machine to find tokens
// state = -1 means exclude current character
// state = -2 means include current character

int main() {
    // setting input and output files up
    setIO("parser");

    // sucks the whole input into the string variable: file
    string file;
	stringstream buffer;
    buffer << cin.rdbuf();
    file = buffer.str();

    // the set of all valid symbols
    set<char> symbols = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+',
                         '-', '*', '/', '&', '|', '<', '>', '=', '~'};
    // the set of all valid keywords
    set<string> keywords = {"class", "constructor", "function", "method", "field", "static",
                            "var", "int", "char", "boolean", "void", "true", "false", "null",
                            "this", "let", "do", "if", "else", "while", "return"};

    // the list of all tokens
    vector<Token> tokens;

    int i = 0;
    while (i < file.size()){
        // skipping whitespace
        while (isWhite(file[i])){
            i += 1;
        }

        // line and block comments
        if (file[i] == '/' && (file[i+1] == '/' || file[i+1] == '*')){
            int end = eot(file, i, fsm_com);
            int len = end - i;
            string newToken = file.substr(i, len);
            tokens.push_back({"comment", newToken});
            i = end;
        }
        // checking for symbols
        else if (symbols.count(file[i])){
            string newToken = file.substr(i, 1);
            tokens.push_back({"symbol", newToken});
            i = i + 1;
        }
        // identifiers or keywords
        else if (isAlphabetic(file[i])){
            int end = eot(file, i, fsm_ki);
            int len = end - i;
            string newToken = file.substr(i, len);

            // check if keyword or identifier
            string type;
            if (keywords.count(newToken)){
                type = "keyword";
            }
            else {
                type = "identifier";
            }

            tokens.push_back({type, newToken});
            i = end;
        }
        // string constants
        else if (file[i] == '"'){
            int end = eot(file, i, fsm_str);
            int len = end - i;
            string newToken = file.substr(i, len);
            tokens.push_back({"StringConstant", newToken});
            i = end;
        }
        // integer constants
        else if (isNumeric(file[i])){
            int end = eot(file, i, fsm_int);
            int len = end - i;
            string newToken = file.substr(i, len );
            tokens.push_back({"integerConstant", newToken});
            i = end;
        }
    }

    for (Token t : tokens){
        string type = t.type;
        string tok = t.tok;
        cout << '<' << type << "> " << tok << '\n';
    }


    
    // start of parser
    vector<Node> parse;

    
    tests();

    return 0;
}

bool isAlphabetic(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool isNumeric(char c){
    return ('0' <= c && c <= '9');
}

bool isAlphanumeric(char c){
    return isAlphabetic(c) || isNumeric(c) || c == '_';
}

bool isWhite(char c){
    return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

bool strEq(string a, string b){
    return a.compare(b) == 0;
}

bool strIn(string str, vector<string> lis){
    for (string s : lis){
        if (strEq(str, s)){
            return true;
        }
    }
    return false;
}

// tokenizer functions:

int eot(string str, int i, int (*fsm)(int, char)){
    int state = 0;
    while (state >= 0 && i < str.size()){
        state = fsm(state, str[i]);
        i += 1;
    }

    // exclude last character
    if (state == -1){
        return i-1;
    }
    // include last character
    else if (state == -2){
        return i;
    }
    // if you run into the end of the string or if something went wrong
    return i;
}

int fsm_ki(int state, char c){
    if (!isAlphanumeric(c)){
        return -1;
    }
    return state;
}

int fsm_str(int state, char c){
    // starts at state = 0
    if (state == 0){
        return 1;
    }
    else if (state == 1 && c == '"'){
        return -2;
    }
    return state;
}

int fsm_int(int state, char c){
    if (!isNumeric(c)){
        return -1;
    }
    return 0;
}

int fsm_com(int state, char c){
    if (state == 0 && c == '/'){
        return 1;
    }
    // block comments
    else if (state == 1 && c == '*'){
        return 2;
    }
    else if (state == 2 && c == '*'){
        return 3;
    }
    else if (state == 3 && c == '/'){
        return -2;
    }

    // one line comments
    else if (state == 1 && c == '/'){
        return 4;
    }
    else if (state == 4 && (c == '\n' || c == '\r')){
        return -1;
    }

    return state;
}

int fsm_bs(int state, char c){
    if (state == 0 && c == '"'){
        return 1;
    }
    else if (state == 1 && c == '\\'){
        return 2;
    }
    else if (state == 1 && c == '"'){
        return -2;
    }
    return 1;
}

// parser fucntions:
int parseClass(vector<Node>* parse, vector<Token> tokens, int idx){
    Node newParse = Node("class");
    // class className '{'
    for (int i = 0; i < 3; i += 1){
        newParse.vals.push_back(Node(tokens[idx]));
        idx += 1;
    }

    while (strIn(tokens[idx].tok, {"static", "field"})){
        idx = parseClassVarDec(&newParse.vals, tokens, idx);
    }

    while (strIn(tokens[idx].tok, {"constructor", "function", "method"})){
        idx = parseSubroutineDec(&newParse.vals, tokens, idx);
    }

    // ending '}'
    newParse.vals.push_back(tokens[idx]);
    idx += 1;

    (*parse).push_back(newParse);
    
    return idx;
}

int parseClassVarDec(vector<Node>* parse, vector<Token> tokens, int idx){
    Node newParse = Node("classVarDec");

    // static|field type varName
    for (int i = 0; i < 3; i += 1){
        newParse.vals.push_back(Node(tokens[idx]));
        idx += 1;
    }

    // if there is more than one variable being declared: ',' varName
    while (!strEq(tokens[idx].tok, ";")){
        newParse.vals.push_back(Node(tokens[idx]));
        idx += 1;
        newParse.vals.push_back(Node(tokens[idx]));
        idx += 1;
    }

    // the ending semicolon ';'
    newParse.vals.push_back(Node(tokens[idx]));
    idx += 1;

    (*parse).push_back(newParse);

    return idx;
}

int parseSubroutineDec(vector<Node>* parse, vector<Token> tokens, int idx){
    Node newParse = Node("subroutineDec");

    // (constructor|function|method) (void|type) subroutineName '('
    for (int i = 0; i < 4; i += 1){
        newParse.vals.push_back(Node(tokens[idx]));
        idx += 1;
    }

    // parameterList
    idx = parseParameterList(&newParse.vals, tokens, idx);
    
    // ')' at the end of parameterList
    newParse.vals.push_back(Node(tokens[idx]));
    idx += 1;

    // subroutineBody
    idx = parseSubroutineBody(&newParse.vals, tokens, idx);

    (*parse).push_back(newParse);

    return idx;
}


int parseParameterList(vector<Node>* parse, vector<Token> tokens, int idx){
    Node newParse = Node("parameterList");
    
    if (strEq(tokens[idx].tok, ")")){
        (*parse).push_back(newParse);
        return idx;
    }

    // type varName
    for (int i = 0; i < 2; i += 1){
        newParse.vals.push_back(Node(tokens[idx]));
        idx += 1;
    }

    // if there are more parameters in the parameterList:
    while (!strEq(tokens[idx].tok, ")")){
        // ',' type varName
        for (int i = 0; i < 3; i += 1){
            newParse.vals.push_back(Node(tokens[idx]));
            idx += 1;
        }
    }

    (*parse).push_back(newParse);

    return idx;
}

int parseSubroutineBody(vector<Node>* parse, vector<Token> tokens, int idx){
    Node newParse = Node("subroutineBody");

    // the opening '{'
    newParse.vals.push_back(Node(tokens[idx]));
    idx += 1;

    // variable declarations:
    while (strEq(tokens[idx].tok, "var")){
        idx = parseVarDec(&newParse.vals, tokens, idx);
    }

    // parsing all the statements
    idx = parseStatements(&newParse.vals, tokens, idx);

    // the closing '}'
}

int parseVarDec(vector<Node>* parse, vector<Token> tokens, int idx){
    Node newParse = Node("varDec");

    // 'var' type varName
    for (int i = 0; i < 3; i += 1){
        newParse.vals.push_back(Node(tokens[idx]));
        idx += 1;
    }

    // if there is more than one variable declared:
    while (strEq()

    // the ending ';'
    newParse.vals.push_back(Node(tokens[idx]));
    idx += 1;
}

void tests(){
    bool cases[] = {
        eot("rasdfh/asdf toh_5ta n", 0, fsm_ki) == 6,
        eot("rasdfh/asdf toh_5ta n", 8, fsm_ki) == 11,
        eot("rasdfh/asdf toh_5ta n", 12, fsm_ki) == 19,
        eot("string str = \"blah blah\";", 13, fsm_str) == 24,
        eot("1234bsa", 0, fsm_int) == 4,
        eot("/*\nthe comment\nasd\n*/ more stuff", 0, fsm_com) == 21,
        eot("// jivgarfag\n asvba", 0, fsm_com) == 12,
        eot("\"badfp\\\"morestuff \" dafdas", 0, fsm_bs) == 19,
    };

    for (int i = 0; i < sizeof(cases); i += 1){
        if (!cases[i]){
            cout << "Failed on case #" + to_string(i+1) << '\n';
        }
    }
}