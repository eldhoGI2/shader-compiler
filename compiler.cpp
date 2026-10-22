#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

// ==========================================
// 1. TOKEN DEFINITIONS
// ==========================================
enum TokenType {
    // Window & Environment
    TOKEN_WINDOW, TOKEN_TITLE, TOKEN_BG, TOKEN_SHADER,
    
    // Shapes - Rectangles & Squares
    TOKEN_RECTANGLE, TOKEN_RECT_OUTLINE,
    TOKEN_CENTER_RECT, TOKEN_CENTER_RECT_OUTLINE,
    TOKEN_SQUARE, TOKEN_SQUARE_OUTLINE,
    TOKEN_CENTER_SQUARE, TOKEN_CENTER_SQUARE_OUTLINE,
    TOKEN_ROUNDED_RECT, TOKEN_ROUNDED_RECT_OUTLINE,
    
    // Shapes - Circles & Ellipses
    TOKEN_CIRCLE, TOKEN_FILL_CIRCLE,
    TOKEN_CENTER_CIRCLE, TOKEN_FILL_CENTER_CIRCLE,
    TOKEN_ELLIPSE, TOKEN_FILL_ELLIPSE,
    
    // Shapes - Triangles & Polygons
    TOKEN_TRIANGLE, TOKEN_FILL_TRIANGLE,
    TOKEN_QUAD, TOKEN_FILL_QUAD,
    TOKEN_POLYGON, TOKEN_FILL_POLYGON,
    TOKEN_STAR, TOKEN_FILL_STAR,
    
    // Lines & Points
    TOKEN_LINE, TOKEN_POINT,
    TOKEN_LINE_WIDTH, TOKEN_POINT_SIZE,
    
    // Colors & Styling
    TOKEN_COLOR, TOKEN_ALPHA,
    
    // Primitives & General Literals
    TOKEN_NUMBER, TOKEN_STRING, TOKEN_WORD, TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

// ==========================================
// Color Table & Parsing Helpers
// ==========================================
struct ColorRGB {
    int r, g, b, a;
};

static const map<string, ColorRGB> COLOR_TABLE = {
    {"red",         {255, 0, 0, 255}},
    {"green",       {0, 255, 0, 255}},
    {"blue",        {0, 0, 255, 255}},
    {"white",       {255, 255, 255, 255}},
    {"black",       {0, 0, 0, 255}},
    {"yellow",      {255, 255, 0, 255}},
    {"cyan",        {0, 255, 255, 255}},
    {"magenta",     {255, 0, 255, 255}},
    {"orange",      {255, 165, 0, 255}},
    {"purple",      {128, 0, 128, 255}},
    {"pink",        {255, 192, 203, 255}},
    {"gray",        {128, 128, 128, 255}},
    {"grey",        {128, 128, 128, 255}},
    {"darkgray",    {64, 64, 64, 255}},
    {"darkgrey",    {64, 64, 64, 255}},
    {"lightgray",   {200, 200, 200, 255}},
    {"lightgrey",   {200, 200, 200, 255}},
    {"gold",        {255, 215, 0, 255}},
    {"silver",      {192, 192, 192, 255}},
    {"brown",       {139, 69, 19, 255}},
    {"lime",        {50, 205, 50, 255}},
    {"teal",        {0, 128, 128, 255}},
    {"navy",        {0, 0, 128, 255}},
    {"violet",      {238, 130, 238, 255}},
    {"indigo",      {75, 0, 130, 255}},
    {"coral",       {255, 127, 80, 255}},
    {"crimson",     {220, 20, 60, 255}},
    {"skyblue",     {135, 206, 235, 255}},
    {"sky_blue",    {135, 206, 235, 255}},
    {"turquoise",   {64, 224, 208, 255}},
    {"maroon",      {128, 0, 0, 255}},
    {"olive",       {128, 128, 0, 255}},
    {"mint",        {152, 255, 152, 255}},
    {"peach",       {255, 218, 185, 255}},
    {"lavender",    {230, 230, 250, 255}},
    {"amber",       {255, 191, 0, 255}},
    {"rose",        {255, 0, 127, 255}}
};

static string to_lower_str(string s) {
    for (char &c : s) c = tolower(c);
    return s;
}

static bool is_number_str(const string& s) {
    if (s.empty()) return false;
    size_t start = 0;
    if (s[0] == '-' && s.length() > 1) start = 1;
    for (size_t i = start; i < s.length(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

static bool is_hex_color_literal(const string& s, size_t pos) {
    if (s[pos] != '#') return false;
    size_t j = pos + 1;
    while (j < s.length() && isxdigit(s[j])) {
        j++;
    }
    size_t hex_len = j - (pos + 1);
    if (hex_len == 3 || hex_len == 4 || hex_len == 6 || hex_len == 8) {
        if (j >= s.length() || s[j] == ' ' || s[j] == '\t' || s[j] == '\r' || s[j] == '\n') {
            return true;
        }
    }
    return false;
}

static bool parse_hex_color(string s, ColorRGB &c) {
    if (s.rfind("#", 0) == 0) s = s.substr(1);
    else if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) s = s.substr(2);
    else return false;

    try {
        if (s.length() == 3) {
            c.r = stoi(string(2, s[0]), nullptr, 16);
            c.g = stoi(string(2, s[1]), nullptr, 16);
            c.b = stoi(string(2, s[2]), nullptr, 16);
            c.a = 255;
            return true;
        } else if (s.length() == 6) {
            c.r = stoi(s.substr(0, 2), nullptr, 16);
            c.g = stoi(s.substr(2, 2), nullptr, 16);
            c.b = stoi(s.substr(4, 2), nullptr, 16);
            c.a = 255;
            return true;
        } else if (s.length() == 8) {
            c.r = stoi(s.substr(0, 2), nullptr, 16);
            c.g = stoi(s.substr(2, 2), nullptr, 16);
            c.b = stoi(s.substr(4, 2), nullptr, 16);
            c.a = stoi(s.substr(6, 2), nullptr, 16);
            return true;
        }
    } catch (...) {}
    return false;
}

// ==========================================
// 2. LEXER
// ==========================================
vector<Token> lexer(const string& source) {
    vector<Token> tokens;
    size_t i = 0;
    size_t n = source.length();

    while (i < n) {
        char c = source[i];

        // 1. Skip whitespace
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            i++;
            continue;
        }

        // 2. Comments:
        // "//" is always a comment.
        // "#" is a comment UNLESS it is a hex color literal (#ff0000)
        if ((c == '/' && i + 1 < n && source[i + 1] == '/') ||
            (c == '#' && !is_hex_color_literal(source, i))) {
            while (i < n && source[i] != '\n') {
                i++;
            }
            continue;
        }

        // 3. String literals ("title in quotes")
        if (c == '"') {
            i++;
            string str_val = "";
            while (i < n && source[i] != '"') {
                str_val += source[i];
                i++;
            }
            if (i < n && source[i] == '"') i++;
            tokens.push_back({TOKEN_STRING, str_val});
            continue;
        }

        // 4. Words, Hex Literals, & Numbers
        string word = "";
        while (i < n && source[i] != ' ' && source[i] != '\t' && source[i] != '\r' && source[i] != '\n') {
            // Stop if inline comment starts
            if (source[i] == '/' && i + 1 < n && source[i + 1] == '/') break;
            if (source[i] == '#' && !is_hex_color_literal(source, i)) break;

            word += source[i];
            i++;
        }

        if (word.empty()) continue;

        string low = to_lower_str(word);

        // Keywords matching
        if (low == "window") tokens.push_back({TOKEN_WINDOW, word});
        else if (low == "title") tokens.push_back({TOKEN_TITLE, word});
        else if (low == "bg" || low == "background") tokens.push_back({TOKEN_BG, word});
        else if (low == "shader") tokens.push_back({TOKEN_SHADER, word});

        // Rectangles & Squares
        else if (low == "rect" || low == "rectangle") tokens.push_back({TOKEN_RECTANGLE, word});
        else if (low == "rect_outline" || low == "stroke_rect") tokens.push_back({TOKEN_RECT_OUTLINE, word});
        else if (low == "center_rect") tokens.push_back({TOKEN_CENTER_RECT, word});
        else if (low == "center_rect_outline") tokens.push_back({TOKEN_CENTER_RECT_OUTLINE, word});
        else if (low == "square") tokens.push_back({TOKEN_SQUARE, word});
        else if (low == "square_outline") tokens.push_back({TOKEN_SQUARE_OUTLINE, word});
        else if (low == "center_square") tokens.push_back({TOKEN_CENTER_SQUARE, word});
        else if (low == "center_square_outline") tokens.push_back({TOKEN_CENTER_SQUARE_OUTLINE, word});
        else if (low == "rounded_rect" || low == "round_rect" || low == "fill_rounded_rect") tokens.push_back({TOKEN_ROUNDED_RECT, word});
        else if (low == "rounded_rect_outline" || low == "stroke_rounded_rect") tokens.push_back({TOKEN_ROUNDED_RECT_OUTLINE, word});

        // Circles & Ellipses
        else if (low == "circle") tokens.push_back({TOKEN_CIRCLE, word});
        else if (low == "fill_circle" || low == "circle_filled") tokens.push_back({TOKEN_FILL_CIRCLE, word});
        else if (low == "center_circle") tokens.push_back({TOKEN_CENTER_CIRCLE, word});
        else if (low == "fill_center_circle") tokens.push_back({TOKEN_FILL_CENTER_CIRCLE, word});
        else if (low == "ellipse") tokens.push_back({TOKEN_ELLIPSE, word});
        else if (low == "fill_ellipse") tokens.push_back({TOKEN_FILL_ELLIPSE, word});

        // Triangles & Polygons
        else if (low == "triangle") tokens.push_back({TOKEN_TRIANGLE, word});
        else if (low == "fill_triangle") tokens.push_back({TOKEN_FILL_TRIANGLE, word});
        else if (low == "quad") tokens.push_back({TOKEN_QUAD, word});
        else if (low == "fill_quad") tokens.push_back({TOKEN_FILL_QUAD, word});
        else if (low == "polygon") tokens.push_back({TOKEN_POLYGON, word});
        else if (low == "fill_polygon") tokens.push_back({TOKEN_FILL_POLYGON, word});
        else if (low == "star") tokens.push_back({TOKEN_STAR, word});
        else if (low == "fill_star") tokens.push_back({TOKEN_FILL_STAR, word});

        // Lines & Points
        else if (low == "line") tokens.push_back({TOKEN_LINE, word});
        else if (low == "point" || low == "dot") tokens.push_back({TOKEN_POINT, word});
        else if (low == "line_width" || low == "stroke_width") tokens.push_back({TOKEN_LINE_WIDTH, word});
        else if (low == "point_size") tokens.push_back({TOKEN_POINT_SIZE, word});

        // Colors & Alpha
        else if (low == "color" || low == "colour") tokens.push_back({TOKEN_COLOR, word});
        else if (low == "alpha") tokens.push_back({TOKEN_ALPHA, word});

        // Literals: Number or Word/Color/Hex
        else if (is_number_str(word)) tokens.push_back({TOKEN_NUMBER, word});
        else tokens.push_back({TOKEN_WORD, word});
    }

    return tokens;
}

// ==========================================
// 3. PARSER & CODE GENERATOR (OpenGL 2 Backend)
// ==========================================
void parse_and_generate(const vector<Token>& tokens) {
    ofstream out_file("generated_game.c");
    out_file << "// --- AUTOMATICALLY GENERATED OPENGL 2 GRAPHICS CODE ---" << endl;
    out_file << "#include \"graphics_runtime.h\"" << endl << endl;
    out_file << "int main() {" << endl;

    string window_title = "Shader Program Compiler (OpenGL 2)";
    bool window_created = false;

    // Scan ahead to find if a custom window title is defined
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TOKEN_TITLE && i + 1 < tokens.size()) {
            window_title = tokens[i + 1].value;
            break;
        }
    }

    for (size_t i = 0; i < tokens.size(); i++) {
        TokenType t = tokens[i].type;

        if (t == TOKEN_TITLE) {
            i += 1;
        }
        else if (t == TOKEN_WINDOW) {
            out_file << "    create_window_with_title(" << tokens[i+1].value << ", " << tokens[i+2].value 
                     << ", \"" << window_title << "\");" << endl;
            window_created = true;
            i += 2;
        }
        else if (t == TOKEN_BG) {
            string val = to_lower_str(tokens[i+1].value);
            ColorRGB c;
            if (COLOR_TABLE.count(val)) {
                c = COLOR_TABLE.at(val);
                out_file << "    set_background(" << c.r << ", " << c.g << ", " << c.b << ");" << endl;
                i += 1;
            } else if (parse_hex_color(tokens[i+1].value, c)) {
                out_file << "    set_background(" << c.r << ", " << c.g << ", " << c.b << ");" << endl;
                i += 1;
            } else {
                out_file << "    set_background(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
                i += 3;
            }
        }
        else if (t == TOKEN_SHADER) {
            string shader_name = to_lower_str(tokens[i+1].value);
            out_file << "    set_shader(\"" << shader_name << "\");" << endl;
            i += 1;
        }
        else if (t == TOKEN_COLOR) {
            string val = to_lower_str(tokens[i+1].value);
            ColorRGB c;
            if (COLOR_TABLE.count(val)) {
                c = COLOR_TABLE.at(val);
                out_file << "    set_color_rgba(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ");" << endl;
                i += 1;
            } else if (parse_hex_color(tokens[i+1].value, c)) {
                out_file << "    set_color_rgba(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ");" << endl;
                i += 1;
            } else {
                if (i + 4 < tokens.size() && is_number_str(tokens[i+4].value) && 
                    (i + 5 >= tokens.size() || tokens[i+5].type != TOKEN_NUMBER)) {
                    out_file << "    set_color_rgba(" << tokens[i+1].value << ", " << tokens[i+2].value 
                             << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
                    i += 4;
                } else {
                    out_file << "    set_color(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
                    i += 3;
                }
            }
        }
        else if (t == TOKEN_ALPHA) {
            out_file << "    set_alpha(" << tokens[i+1].value << ");" << endl;
            i += 1;
        }
        else if (t == TOKEN_LINE_WIDTH) {
            out_file << "    set_line_width(" << tokens[i+1].value << ");" << endl;
            i += 1;
        }
        else if (t == TOKEN_POINT_SIZE) {
            out_file << "    set_point_size(" << tokens[i+1].value << ");" << endl;
            i += 1;
        }

        // Rectangles & Squares
        else if (t == TOKEN_RECTANGLE) {
            out_file << "    draw_rectangle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (t == TOKEN_RECT_OUTLINE) {
            out_file << "    draw_rect_outline(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (t == TOKEN_CENTER_RECT) {
            out_file << "    draw_center_rectangle(" << tokens[i+1].value << ", " << tokens[i+2].value << ");" << endl;
            i += 2;
        }
        else if (t == TOKEN_CENTER_RECT_OUTLINE) {
            out_file << "    draw_center_rect_outline(" << tokens[i+1].value << ", " << tokens[i+2].value << ");" << endl;
            i += 2;
        }
        else if (t == TOKEN_SQUARE) {
            out_file << "    draw_square(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
            i += 3;
        }
        else if (t == TOKEN_SQUARE_OUTLINE) {
            out_file << "    draw_square_outline(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
            i += 3;
        }
        else if (t == TOKEN_CENTER_SQUARE) {
            out_file << "    draw_center_square(" << tokens[i+1].value << ");" << endl;
            i += 1;
        }
        else if (t == TOKEN_CENTER_SQUARE_OUTLINE) {
            out_file << "    draw_center_square_outline(" << tokens[i+1].value << ");" << endl;
            i += 1;
        }
        else if (t == TOKEN_ROUNDED_RECT) {
            out_file << "    draw_fill_rounded_rect(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ");" << endl;
            i += 5;
        }
        else if (t == TOKEN_ROUNDED_RECT_OUTLINE) {
            out_file << "    draw_rounded_rect(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ");" << endl;
            i += 5;
        }

        // Circles & Ellipses
        else if (t == TOKEN_CIRCLE) {
            out_file << "    draw_circle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
            i += 3;
        }
        else if (t == TOKEN_FILL_CIRCLE) {
            out_file << "    draw_fill_circle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ");" << endl;
            i += 3;
        }
        else if (t == TOKEN_CENTER_CIRCLE) {
            out_file << "    draw_center_circle(" << tokens[i+1].value << ");" << endl;
            i += 1;
        }
        else if (t == TOKEN_FILL_CENTER_CIRCLE) {
            out_file << "    draw_fill_center_circle(" << tokens[i+1].value << ");" << endl;
            i += 1;
        }
        else if (t == TOKEN_ELLIPSE) {
            out_file << "    draw_ellipse(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (t == TOKEN_FILL_ELLIPSE) {
            out_file << "    draw_fill_ellipse(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }

        // Triangles & Polygons
        else if (t == TOKEN_TRIANGLE) {
            out_file << "    draw_triangle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ", " << tokens[i+6].value << ");" << endl;
            i += 6;
        }
        else if (t == TOKEN_FILL_TRIANGLE) {
            out_file << "    draw_fill_triangle(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ", " << tokens[i+6].value << ");" << endl;
            i += 6;
        }
        else if (t == TOKEN_QUAD) {
            out_file << "    draw_quad(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ", " << tokens[i+6].value << ", " << tokens[i+7].value << ", " << tokens[i+8].value << ");" << endl;
            i += 8;
        }
        else if (t == TOKEN_FILL_QUAD) {
            out_file << "    draw_fill_quad(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ", " << tokens[i+6].value << ", " << tokens[i+7].value << ", " << tokens[i+8].value << ");" << endl;
            i += 8;
        }
        else if (t == TOKEN_POLYGON) {
            out_file << "    draw_polygon(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (t == TOKEN_FILL_POLYGON) {
            out_file << "    draw_fill_polygon(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (t == TOKEN_STAR) {
            out_file << "    draw_star(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ");" << endl;
            i += 5;
        }
        else if (t == TOKEN_FILL_STAR) {
            out_file << "    draw_fill_star(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ", " << tokens[i+5].value << ");" << endl;
            i += 5;
        }

        // Lines & Points
        else if (t == TOKEN_LINE) {
            out_file << "    draw_line(" << tokens[i+1].value << ", " << tokens[i+2].value << ", " << tokens[i+3].value << ", " << tokens[i+4].value << ");" << endl;
            i += 4;
        }
        else if (t == TOKEN_POINT) {
            out_file << "    draw_point(" << tokens[i+1].value << ", " << tokens[i+2].value << ");" << endl;
            i += 2;
        }
    }

    if (!window_created) {
        out_file << "    create_window_with_title(800, 600, \"" << window_title << "\");" << endl;
    }

    out_file << "    render_and_wait();" << endl;
    out_file << "    return 0;\n}" << endl;
    out_file.close();
}

// ==========================================
// 4. MAIN PIPELINE
// ==========================================
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "==========================================================" << endl;
        cout << "   Shader Program Compiler (OpenGL 2 + Custom DSL)        " << endl;
        cout << "==========================================================" << endl;
        cout << "Usage: ./mycompiler <filename.my>" << endl;
        return 1;
    }

    ifstream in_file(argv[1]);
    if (!in_file.is_open()) {
        cerr << "[COMPILER ERROR] Could not open source file: " << argv[1] << endl;
        return 1;
    }

    string my_code = "", line;
    while (getline(in_file, line)) my_code += line + "\n";
    in_file.close();

    cout << "[COMPILER] Reading source file: " << argv[1] << "..." << endl;

    vector<Token> tokens = lexer(my_code);
    parse_and_generate(tokens);

    cout << "[COMPILER] Generated OpenGL 2 C code: generated_game.c" << endl;
    cout << "[COMPILER] Handing off to GCC to build native OpenGL 2 graphics executable..." << endl;

    int result = system("gcc generated_game.c -o final_game -lSDL2 -lGL -lm");

    if (result == 0) {
        cout << "[COMPILER] SUCCESS! Run your program with: ./final_game" << endl;
    } else {
        cout << "[COMPILER] ERROR: GCC compilation failed. Ensure SDL2 and OpenGL libraries are installed." << endl;
    }

    return result;
}
