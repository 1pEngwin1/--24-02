#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <cctype>
#include <iterator>
#include <locale.h>

using namespace std;

enum TokKind { TK_IDENT, TK_INT_LIT, TK_FLOAT_LIT, TK_OP, TK_PUNC, TK_STR, TK_CHAR, TK_END };
struct Tok { TokKind kind; string text; int pos; };

string src;
size_t iidx = 0;
size_t nsrc = 0; 

bool starts_with(const string& s) {
    return src.compare(iidx, s.size(), s) == 0;
}
bool is_ident_start(char c) { return static_cast<bool>(isalpha(static_cast<unsigned char>(c))) || c == '_'; }
bool is_ident(char c) { return static_cast<bool>(isalnum(static_cast<unsigned char>(c))) || c == '_'; }

Tok nextTok() {
    while (iidx < nsrc && isspace(static_cast<unsigned char>(src[iidx]))) ++iidx;
    if (iidx >= nsrc) return { TK_END,"", (int)iidx };
    int pos = (int)iidx;
    char c = src[iidx];

    if (starts_with("//")) {
        iidx += 2;
        while (iidx < nsrc && src[iidx] != '\n') ++iidx;
        return nextTok();
    }
    if (starts_with("/*")) {
        iidx += 2;
        while (iidx + 1 < nsrc && !(src[iidx] == '*' && src[iidx + 1] == '/')) ++iidx;
        if (iidx + 1 < nsrc) iidx += 2;
        return nextTok();
    }

    if (c == '"') {
        string t; t.push_back(c); ++iidx;
        while (iidx < nsrc) {
            char q = src[iidx++];
            t.push_back(q);
            if (q == '\\' && iidx < nsrc) { t.push_back(src[iidx++]); continue; }
            if (q == '"') break;
        }
        return { TK_STR, t, pos };
    }

    if (c == '\'') {
        string t; t.push_back(c); ++iidx;
        while (iidx < nsrc) {
            char q = src[iidx++];
            t.push_back(q);
            if (q == '\\' && iidx < nsrc) { t.push_back(src[iidx++]); continue; }
            if (q == '\'') break;
        }
        return { TK_CHAR, t, pos };
    }

    if (is_ident_start(c)) {
        string t;
        while (iidx < nsrc && is_ident(src[iidx])) t.push_back(src[iidx++]);
        return { TK_IDENT, t, pos };
    }

    if (isdigit(static_cast<unsigned char>(c)) || (c == '.' && iidx + 1 < nsrc && isdigit(static_cast<unsigned char>(src[iidx + 1])))) {
        string t;
        bool has_dot = false, has_exp = false;
        if (c == '.') { has_dot = true; t.push_back(src[iidx++]); }
        while (iidx < nsrc && isdigit(static_cast<unsigned char>(src[iidx]))) t.push_back(src[iidx++]);
        if (iidx < nsrc && src[iidx] == '.') { has_dot = true; t.push_back(src[iidx++]); while (iidx < nsrc && isdigit(static_cast<unsigned char>(src[iidx]))) t.push_back(src[iidx++]); }
        if (iidx < nsrc && (src[iidx] == 'e' || src[iidx] == 'E')) {
            has_exp = true; t.push_back(src[iidx++]);
            if (iidx < nsrc && (src[iidx] == '+' || src[iidx] == '-')) t.push_back(src[iidx++]);
            while (iidx < nsrc && isdigit(static_cast<unsigned char>(src[iidx]))) t.push_back(src[iidx++]);
        }

        while (iidx < nsrc && isalpha(static_cast<unsigned char>(src[iidx]))) t.push_back(src[iidx++]);
        return { (has_dot || has_exp) ? TK_FLOAT_LIT : TK_INT_LIT, t, pos };
    }

    vector<string> multi = { "<<=", ">>=", "==", "!=", "<=", ">=", "&&", "||", "++", "--", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "->", "::" };
    for (size_t k = 0; k < multi.size(); ++k) {
        const string& m = multi[k];
        if (starts_with(m)) {
            iidx += m.size(); return { TK_OP, m, pos };
        }
    }

    iidx++;
    string t(1, c);
    string ops = "+-*/%&|^~<>!=;:,(){}[].#";
    if (ops.find(c) != string::npos) return { TK_OP, t, pos };
    return { TK_PUNC, t, pos };
}

vector<Tok> tokenize(const string& s) {
    src = s; iidx = 0; nsrc = src.size();
    vector<Tok> out;
    for (;;) {
        Tok t = nextTok();
        if (t.kind == TK_END) break;
        out.push_back(t);
    }
    return out;
}

bool is_int_type_keyword(const string& s) {
    static const set<string> ints = { "int","char","signed","unsigned","bool","uint32_t","uint16_t","int32_t","int16_t","size_t","ptrdiff_t","intptr_t","uintptr_t" };
    return ints.find(s) != ints.end();
}
bool is_float_type_keyword(const string& s) {
    static const set<string> fs = { "float" };
    return fs.find(s) != fs.end();
}

int main() {
    setlocale(LC_ALL, "");
    ios::sync_with_stdio(false);
    cin.tie(NULL);


    string input;
    {
        istreambuf_iterator<char> it(cin), end;
        string tmp(it, end);
        input.swap(tmp);
    }

    vector<Tok> toks = tokenize(input);

    unordered_map<string, string> idtype;

    for (size_t i = 0; i < toks.size(); ++i) {
        if (toks[i].kind == TK_IDENT) {
            if (is_int_type_keyword(toks[i].text) || is_float_type_keyword(toks[i].text)) {
                string typ = is_int_type_keyword(toks[i].text) ? "int" : "float";
                size_t j = i + 1;

                while (j < toks.size()) {
                    if (toks[j].kind == TK_IDENT) {
                        idtype[toks[j].text] = typ;

                    }
                    else if (toks[j].kind == TK_OP && toks[j].text == ",") {

                    }
                    else if (toks[j].kind == TK_OP && toks[j].text == "*") {

                    }
                    else break;
                    ++j;
                }
            }
        }
    }

    bool any_error = false;
    for (size_t i = 0; i < toks.size(); ++i) {
        if (toks[i].kind == TK_OP && toks[i].text == "%") {
           
            int li = (int)i - 1;
            while (li >= 0 && (toks[li].kind == TK_PUNC || (toks[li].kind == TK_OP && (toks[li].text == "," || toks[li].text == ";")))) --li;
            int ri = (int)i + 1;
            while (ri < (int)toks.size() && (toks[ri].kind == TK_PUNC || (toks[ri].kind == TK_OP && (toks[ri].text == "," || toks[ri].text == ";")))) ++ri;
            string ltyp = "unknown", rtyp = "unknown";
            if (li >= 0) {
                if (toks[li].kind == TK_INT_LIT) ltyp = "int";
                else if (toks[li].kind == TK_FLOAT_LIT) ltyp = "float";
                else if (toks[li].kind == TK_IDENT) {
                    auto it = idtype.find(toks[li].text);
                    if (it != idtype.end()) ltyp = it->second;
                }
            }
            if (ri < (int)toks.size()) {
                if (toks[ri].kind == TK_INT_LIT) rtyp = "int";
                else if (toks[ri].kind == TK_FLOAT_LIT) rtyp = "float";
                else if (toks[ri].kind == TK_IDENT) {
                    auto it = idtype.find(toks[ri].text);
                    if (it != idtype.end()) rtyp = it->second;
                }
            }
            bool left_ok = (ltyp == "int");
            bool right_ok = (rtyp == "int");
            if (!left_ok || !right_ok) {
                any_error = true;
                int pos = toks[i].pos;
                int a = pos - 20; if (a < 0) a = 0;
                int b = pos + 20; if (b > (int)nsrc) b = (int)nsrc;
                cout << "Операция с % недействительна, так как использован float" 
                    << ": слева ='" << ltyp << "', справа ='" << rtyp << "'. Случай: \"" << input.substr(a, b - a) << "\"";
            }
        }
    }

    if (!any_error) {
        cout << "Отсутствуют операции % с операндами, которые не являются целыми.\n";
    }

    return 0;
}