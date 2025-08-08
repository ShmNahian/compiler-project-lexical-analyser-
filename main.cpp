#include <bits/stdc++.h>
using namespace std;

// Terminal color codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BLUE    "\033[34m"
#define PURPLE  "\033[35m"
#define RED     "\033[31m"

// ----- Token sets -----
set<string> keywords = {
    "int", "float", "double", "char", "bool", "void", "if", "else", "for", "while", "return", "true", "false", "break", "continue",
    "switch", "case", "default", "struct", "class", "public", "private", "new", "this", "using", "namespace", "const", "auto"
};

const set<string> relOps = { ">", "<", ">=", "<=", "==", "!=" };
set<string> arith_ops = { "+", "-", "*", "/", "%", "++", "--" };
set<string> assign_ops = { "=", "+=", "-=", "*=", "/=", "%=" };
set<string> logic_ops = { "&&", "||", "!" };
set<string> delimiters = { ";", ",", "(", ")", "{", "}", "[", "]" };

// ----- Token count (for summary) -----
map<string, int> tokenCount;

// ----- Banner -----
void showBanner() {
    cout << PURPLE << " Welcome to Lexical Analyzer v1.0\n";
    cout << "---------------------------------------------\n" << RESET;
}

// ----- Utility functions -----
bool isNumber(const string& s) {
    if (s.empty()) return false;
    bool hasDot = false;
    size_t i = (s[0] == '-') ? 1 : 0;
    for (; i < s.size(); ++i) {
        if (s[i] == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!isdigit(s[i])) return false;
    }
    return true;
}

bool isIdentifier(const string& s) {
    if (s.empty() || !(isalpha(s[0]) || s[0] == '_')) return false;
    for (char c : s)
        if (!isalnum(c) && c != '_') return false;
    return true;
}

// ----- Token classification -----
void classifyToken(const string& token, ofstream& out) {
    if (token.empty()) return;
    if (keywords.count(token)) {
        out << "Keyword: " << token << "\n";
        tokenCount["Keyword"]++;
    }
    else if (relOps.count(token)) {
        out << "Relational Operator: " << token << "\n";
        tokenCount["Relational Operator"]++;
    }
    else if (arith_ops.count(token)) {
        out << "Arithmetic Operator: " << token << "\n";
        tokenCount["Arithmetic Operator"]++;
    }
    else if (assign_ops.count(token)) {
        out << "Assignment Operator: " << token << "\n";
        tokenCount["Assignment Operator"]++;
    }
    else if (logic_ops.count(token)) {
        out << "Logical Operator: " << token << "\n";
        tokenCount["Logical Operator"]++;
    }
    else if (delimiters.count(token)) {
        out << "Delimiter: " << token << "\n";
        tokenCount["Delimiter"]++;
    }
    else if (isNumber(token)) {
        out << "Number: " << token << "\n";
        tokenCount["Number"]++;
    }
    else if (isIdentifier(token)) {
        out << "Identifier: " << token << "\n";
        tokenCount["Identifier"]++;
    }
    else {
        out << "Unknown: " << token << "\n";
        tokenCount["Unknown"]++;
    }
}

// ----- Analyzer -----
void analyzeLine(string line, ofstream& out, bool& inMultiLineComment) {
    if (inMultiLineComment) {
        if (line.find("*/") != string::npos) inMultiLineComment = false;
        out << "Multi-line Comment: " << line << "\n";
        tokenCount["Multi-line Comment"]++;
        return;
    }

    if (line.find("/*") != string::npos) {
        inMultiLineComment = true;
        out << "Multi-line Comment: " << line << "\n";
        tokenCount["Multi-line Comment"]++;
        return;
    }

    if (line.find("//") != string::npos) {
        out << "Single-line Comment: " << line << "\n";
        tokenCount["Single-line Comment"]++;
        line = line.substr(0, line.find("//"));
    }

    if (line.find("#include") != string::npos) {
        out << "Library Include: " << line << "\n";
        tokenCount["Library Include"]++;
        return;
    }

    if (line.find("using namespace std") != string::npos) {
        out << "Namespace Declaration: using namespace std\n";
        tokenCount["Namespace Declaration"]++;
        return;
    }

    string token = "";
    bool inString = false;
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '"') {
            if (inString) {
                token += c;
                out << "String: " << token << "\n";
                tokenCount["String"]++;
                token = "";
                inString = false;
            } else {
                inString = true;
                token += c;
            }
            continue;
        }

        if (inString) {
            token += c;
            continue;
        }

        if (i + 1 < line.length()) {
            string twoChar = string() + c + line[i + 1];
            if (relOps.count(twoChar) || arith_ops.count(twoChar) || assign_ops.count(twoChar) || logic_ops.count(twoChar)) {
                classifyToken(token, out);
                token = "";
                classifyToken(twoChar, out);
                i++;
                continue;
            }
        }

        if (isspace(c)) {
            classifyToken(token, out);
            token = "";
        } else if (ispunct(c)) {
            classifyToken(token, out);
            token = "";
            classifyToken(string(1, c), out);
        } else {
            token += c;
        }
    }
    classifyToken(token, out);
}

// ----- Main -----
int main() {
    showBanner();

    ofstream out("output.txt");
    if (!out) {
        cerr << RED << "Error creating output file.\n" << RESET;
        return 1;
    }

    string choice;
    cout << BLUE << "Choose input mode [file/manual]: " << RESET;
    cin >> choice;
    cin.ignore();

    string line;
    bool inMultiLineComment = false;

    if (choice == "file") {
        ifstream in("input.txt");
        if (!in) {
            cerr << RED << "Could not open input.txt\n" << RESET;
            return 1;
        }
        while (getline(in, line)) {
            analyzeLine(line, out, inMultiLineComment);
        }
    } else if (choice == "manual") {
        cout << YELLOW << "Enter code line by line (type END to stop):\n" << RESET;
        while (true) {
            getline(cin, line);
            if (line == "END") break;
            analyzeLine(line, out, inMultiLineComment);
        }
    } else {
        cerr << RED << "Invalid input choice!\n" << RESET;
        return 1;
    }

    //  Terminal summary with color
    cout << "\n" << PURPLE << "Token Statistics Summary:" << RESET << "\n";
     for (const auto& u : tokenCount) {
        cout << CYAN << left << setw(24) << u.first << ": " << YELLOW << u.second << RESET << "\n";
    }

    cout << GREEN << "\nLexical analysis complete. Output written to output.txt\n" << RESET;
    cout << "\a"; // Beep

    return 0;
}
