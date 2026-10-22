#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;

// 1. LEXER 
enum TokenType { 
    TOKEN_WINDOW, TOKEN_CIRCLE, TOKEN_RECTANGLE, TOKEN_CENTER_RECT, 
    TOKEN_LINE, TOKEN_TRIANGLE, TOKEN_COLOR, TOKEN_NUMBER, TOKEN_UNKNOWN 
};
struct Token { TokenType type; string value; };

vector<Token> lexer(string source) {
    vector<Token> tokens;
    string current_word = "";
    source += " "; 

    for (char c : source) {
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
            if (current_word == "window") tokens.push_back({TOKEN_WINDOW, current_word});
            else if (current_word == "circle") tokens.push_back({TOKEN_CIRCLE, current_word});
            else if (current_word == "rectangle") tokens.push_back({TOKEN_RECTANGLE, current_word});
            else if (current_word == "center_rect") tokens.push_back({TOKEN_CENTER_RECT, current_word});
            else if (current_word == "line") tokens.push_back({TOKEN_LINE, current_word});
            else if (current_word == "triangle") tokens.push_back({TOKEN_TRIANGLE, current_word});
            else if (current_word == "color") tokens.push_back({TOKEN_COLOR, current_word});
            else if (current_word.length() > 0) tokens.push_back({TOKEN_NUMBER, current_word});
            current_word = "";
        } else {
            current_word += c;
        }
    }
    return tokens;
}

// 2. PARSER & CODE GENERATOR
void parse_and_generate(vector<Token> tokens) {
    ofstream out_file("generated_game.c");
    out_file << "// --- AUTOMATICALLY GENERATED C CODE ---" << endl;
    out_file << "#include \"graphics_runtime.h\"" << endl << endl;
    out_file << "int main() {" << endl;

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TOKEN_WINDOW) {
            out_file << "    create_window(" << tokens[i+1].value << ", " << tokens[i+2].value << ");" << endl;
            i += 2; 
        }
        else if (tokens[i].type == TOKEN_CIRCLE) {
            out_file << "    draw_circle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
            i += 3;
        }
        else if (tokens[i].type == TOKEN_RECTANGLE) {
            out_file << "    draw_rectangle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (tokens[i].type == TOKEN_CENTER_RECT) {
            // center_rect only needs width and height! The C code does the math.
            out_file << "    draw_center_rectangle(" << tokens[i+1].value << ", " << tokens[i+2].value << ");" << endl;
            i += 2;
        }
        else if (tokens[i].type == TOKEN_LINE) {
            out_file << "    draw_line(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (tokens[i].type == TOKEN_TRIANGLE) {
            out_file << "    draw_triangle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ", " << tokens[i+6].value << ");" << endl;
            i += 6;
        }
        else if (tokens[i].type == TOKEN_COLOR) {
            out_file << "    set_color(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
            i += 3;
        }
    }
    
    out_file << "    render_and_wait();" << endl;
    out_file << "    return 0;\n}" << endl;
    out_file.close();
}

// 3. MAIN PIPELINE
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: ./mycompiler <filename.my>" << endl;
        return 1;
    }

    ifstream in_file(argv[1]);
    string my_code = "", line;
    while (getline(in_file, line)) my_code += line + "\n";
    
    cout << "[COMPILER] Reading source file: " << argv[1] << "..." << endl;

    vector<Token> tokens = lexer(my_code);
    parse_and_generate(tokens);

    cout << "[COMPILER] Handing off to GCC to build graphics application..." << endl;
    // CRITICAL: We added -lSDL2 here to link the real graphics engine
    int result = system("gcc generated_game.c -o final_game -lSDL2");

    if (result == 0) {
        cout << "[COMPILER] SUCCESS! Run your program with: ./final_game" << endl;
    } else {
        cout << "[COMPILER] ERROR: GCC compilation failed. Ensure SDL2 is installed." << endl;
    }
    return 0;
}