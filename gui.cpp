// gui.cpp — NSW Graph SFML 
// Compile:
//   g++ -std=c++17 gui.cpp vectorizer.cpp nsw_graph.cpp search.cpp \
//       insert.cpp persistence.cpp \
//       -lsfml-graphics -lsfml-window -lsfml-system -o gui

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "nsw_graph.h"
#include "vectorizer.h"
#include "search.h"
#include "insert.hpp"
#include "persistence.h"
#include "config.h"

using namespace std;

// ─── Window ───────────────────────────────────────────────────────────────────
const unsigned WIN_W    = 1100;
const unsigned WIN_H    = 700;
const unsigned TOPBAR_H = 52;
const string   DB_FILE  = "database.txt";

// ─── Palette ──────────────────────────────────────────────────────────────────
const sf::Color C_BG     {13,  13,  24 };
const sf::Color C_TOP    { 8,   8,  17 };
const sf::Color C_CARD   {19,  19,  36 };
const sf::Color C_CARD2  {23,  23,  45 };
const sf::Color C_INPUT  {10,  10,  22 };
const sf::Color C_BORDER {34,  34,  64 };
const sf::Color C_ACCENT {85,  72, 200 };
const sf::Color C_ACC2   {55,  45, 140 };
const sf::Color C_TXT    {210, 210, 235};
const sf::Color C_MUTED  {85,  85, 125 };
const sf::Color C_DIM    {45,  45,  80 };
const sf::Color C_GREEN  {74, 222, 128 };
const sf::Color C_RED    {248,113, 113 };
const sf::Color C_YELLOW {251,191,  36 };
const sf::Color C_ORANGE {251,146,  60 };
const sf::Color C_IBTN   {28, 105,  28 };
const sf::Color C_SBTN   {55,  45, 160 };
const sf::Color C_DELBTN {105,  22,  22 };

// ─── Screens ──────────────────────────────────────────────────────────────────
enum Screen { S_MAIN, S_DELETE, S_GRAPH };

// ─── Global state ─────────────────────────────────────────────────────────────
NSWGraph graph;
Screen   cur = S_MAIN;
sf::Font font;

string               mainInput;
vector<SearchResult> srchResults;
bool                 srchDone   = false;
string               statusMsg;
bool                 statusGood = true;

float delScroll = 0.f;
float gfxScroll = 0.f;

// ─── Low-level helpers ────────────────────────────────────────────────────────
void R(sf::RenderWindow& w, float x, float y, float wd, float ht,
       sf::Color fill, sf::Color out = {0,0,0,0}, float th = 0.f)
{
    sf::RectangleShape s({wd, ht});
    s.setPosition(x, y);
    s.setFillColor(fill);
    if (th > 0.f) { s.setOutlineThickness(th); s.setOutlineColor(out); }
    w.draw(s);
}

void T(sf::RenderWindow& w, const string& s,
       float x, float y, unsigned sz, sf::Color c, bool bold = false)
{
    sf::Text t(s, font, sz);
    t.setPosition(x, y);
    t.setFillColor(c);
    if (bold) t.setStyle(sf::Text::Bold);
    w.draw(t);
}

bool Btn(sf::RenderWindow& w, float x, float y, float wd, float ht,
         const string& label, sf::Color bg, sf::Color tc,
         sf::Vector2i mouse, bool clicked, unsigned sz = 12)
{
    sf::FloatRect r(x, y, wd, ht);
    bool hov = r.contains((float)mouse.x, (float)mouse.y);
    sf::Color f = hov
        ? sf::Color(min(255, bg.r+30), min(255, bg.g+30), min(255, bg.b+30))
        : bg;
    R(w, x, y, wd, ht, f, C_BORDER, 1.f);
    sf::Text t(label, font, sz);
    sf::FloatRect tb = t.getLocalBounds();
    t.setPosition(x + (wd - tb.width) / 2.f - tb.left,
                  y + (ht - tb.height) / 2.f - tb.top);
    t.setFillColor(tc);
    w.draw(t);
    return hov && clicked;
}

sf::Color distCol(double d) {
    if (d < 0.0001) return C_GREEN;
    if (d < 0.25)   return C_GREEN;
    if (d < 0.55)   return C_YELLOW;
    if (d < 0.80)   return C_ORANGE;
    return C_RED;
}
string distLbl(double d) {
    if (d < 0.0001) return "exact match";
    if (d < 0.25)   return "very similar";
    if (d < 0.55)   return "similar";
    if (d < 0.80)   return "loosely related";
    return "unrelated";
}
string trunc(const string& s, size_t n) {
    return s.size() > n ? s.substr(0, n - 2) + ".." : s;
}
void drawCursor(sf::RenderWindow& w, const string& s,
                float ox, float oy, unsigned sz)
{
    sf::Text m(s, font, sz);
    float cx = ox + m.getLocalBounds().left + m.getLocalBounds().width + 2.f;
    R(w, cx, oy, 1.f, (float)sz + 4, C_TXT);
}

// ─── Top navigation bar ───────────────────────────────────────────────────────
void drawTopbar(sf::RenderWindow& w, sf::Vector2i mouse, bool clicked)
{
    R(w, 0, 0, WIN_W, TOPBAR_H, C_TOP);
    R(w, 0, TOPBAR_H - 1, WIN_W, 1, C_BORDER);

    R(w, 14, 10, 36, 32, C_ACCENT);
    T(w, "NSW", 18, 15, 10, {255,255,255}, true);
    T(w, "NSW Graph — Private Local Text Search", 58, 13, 13, C_TXT, true);
    T(w, "Trigram Hashing  |  Cosine Similarity  |  Greedy NSW Search",
       58, 31, 9, C_DIM);

    T(w, to_string(graph.getNodeCount()) + " nodes", WIN_W - 340, 19, 10, C_DIM);

    if (Btn(w, WIN_W - 295, 10, 95, 32, "DELETE NODES",
            cur == S_DELETE ? C_ACC2 : C_DELBTN,
            {255,255,255}, mouse, clicked, 10))
    {
        cur = S_DELETE;
        delScroll = 0.f;
        statusMsg.clear();
    }

    if (Btn(w, WIN_W - 192, 10, 90, 32, "SHOW GRAPH",
            cur == S_GRAPH ? C_ACC2 : sf::Color{35,35,70},
            {255,255,255}, mouse, clicked, 10))
    {
        cur = S_GRAPH;
        gfxScroll = 0.f;
    }

    if (Btn(w, WIN_W - 94, 10, 82, 32,
            cur == S_MAIN ? "HOME" : "<- BACK",
            cur == S_MAIN ? sf::Color{28,28,55} : C_ACC2,
            {255,255,255}, mouse, clicked, 10))
    {
        cur = S_MAIN;
        statusMsg.clear();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN SCREEN
// One input box + INSERT button + SEARCH button on a single row.
// Results expand below.
//
// INSERT — Areeba Izhar (ai09625)  — insert.hpp / insert.cpp
//           also uses nsw_graph.cpp (createNode, addEdge) and node.h
// SEARCH — Noor E Hira  (nh10085) — search.h  / search.cpp
// save   — Shaheer Qureshi (sq09647) — persistence.h / persistence.cpp
// ─────────────────────────────────────────────────────────────────────────────
void drawMain(sf::RenderWindow& w, sf::Vector2i mouse, bool clicked)
{
    const float mx  = 110.f;
    const float top = TOPBAR_H + 40.f;

    // ── Instruction text ─────────────────────────────────────────────────────
    T(w, "Enter a word or phrase", mx, top, 15, C_TXT, true);
    T(w, "Click INSERT to add it to the graph.   "
         "Click SEARCH to find the 3 closest matches.",
       mx, top + 22, 10, C_MUTED);

    // ── Input + button row ───────────────────────────────────────────────────
    const float iy  = top + 52.f;
    const float iw  = 638.f;
    const float ibw = 100.f;
    const float ibh = 42.f;
    const float gap =  8.f;

    R(w, mx, iy, iw, ibh, C_INPUT, C_BORDER, 1.f);
    if (mainInput.empty())
        T(w, "e.g.  machine learning fundamentals",
           mx + 12, iy + 12, 12, C_DIM);
    else {
        T(w, mainInput, mx + 12, iy + 12, 12, C_TXT);
        drawCursor(w, mainInput, mx + 12, iy + 12, 12);
    }

    // INSERT — Areeba Izhar (ai09625)
    bool doInsert = Btn(w, mx + iw + gap, iy, ibw, ibh,
                        "INSERT", C_IBTN, {255,255,255}, mouse, clicked);

    // // SEARCH — Noor E Hira (nh10085)
    // bool doSearch = Btn(w, mx + iw + gap + ibw + gap, iy, ibw, ibh,
    //                     "SEARCH", C_SBTN, {255,255,255}, mouse, clicked);

    // ── Handle INSERT ─────────────────────────────────────────────────────────
    if (doInsert && !mainInput.empty()) {
        bool dup = false;
        for (Node* n : graph.getAllNodes())
            if (n->text == mainInput) { dup = true; break; }

        if (dup) {
            statusMsg  = "\"" + mainInput + "\" already exists in the graph.";
            statusGood = false;
        } else {
            insert(&graph, mainInput);          // Areeba — insert.cpp
            saveToFile(&graph, DB_FILE);        // Shaheer — persistence.cpp
            statusMsg  = "Inserted  \"" + mainInput
                       + "\"  —  saved to " + DB_FILE
                       + "  (" + to_string(graph.getNodeCount()) + " nodes total)";
            statusGood = true;
            srchDone   = false;
            srchResults.clear();
            mainInput.clear();
        }
    }

    // // ── Handle SEARCH ─────────────────────────────────────────────────────────
    // if (doSearch && !mainInput.empty()) {
    //     if (graph.getNodeCount() == 0) {
    //         statusMsg  = "Graph is empty — insert some phrases first.";
    //         statusGood = false;
    //     } else {
    //         // Noor E Hira — search.cpp
    //         srchResults = search(&graph, vectorize(mainInput), 3, EF_SEARCH);
    //         srchDone    = true;
    //         statusMsg   = to_string(srchResults.size())
    //                     + " result(s) for  \"" + mainInput + "\"";
    //         statusGood  = true;
    //     }
    // }

    // ── Status line ───────────────────────────────────────────────────────────
    const float sy = iy + ibh + 22.f;
    if (!statusMsg.empty())
        T(w, statusMsg, mx, sy, 11, statusGood ? C_GREEN : C_RED);

    // ── Search results ────────────────────────────────────────────────────────
    if (!srchDone || srchResults.empty()) return;

    const float ry0 = sy + 26.f;
    T(w, "TOP 3 CLOSEST MATCHES", mx, ry0, 9, C_MUTED);

    const sf::Color rankBar[3] = { C_ACCENT, {60,50,150}, {35,30,90} };
    float ry = ry0 + 16.f;

    for (int i = 0; i < (int)srchResults.size(); i++) {
        const float rh = 80.f;
        R(w, mx, ry, 860.f, rh, C_CARD, C_BORDER, 1.f);
        R(w, mx, ry, 3.f, rh, rankBar[i]);         // left accent bar

        // rank
        T(w, "[" + to_string(i + 1) + "]", mx + 10, ry + 10, 11, C_MUTED);

        // similarity badge (top-right corner of card)
        string     lbl = distLbl(srchResults[i].distance);
        sf::Color  lc  = distCol(srchResults[i].distance);
        R(w, mx + 746, ry + 10, 106, 18, C_CARD2, C_BORDER, 1.f);
        T(w, lbl, mx + 752, ry + 12, 9, lc);

        // phrase
        T(w, "\"" + trunc(srchResults[i].text, 68) + "\"",
           mx + 32, ry + 10, 13, C_TXT, i == 0);

        // distance value
        ostringstream oss;
        oss << fixed << setprecision(6) << srchResults[i].distance;
        T(w, "dist = " + oss.str(), mx + 32, ry + 34, 11, C_MUTED);

        // distance bar
        float barW = min((float)(srchResults[i].distance * 380.f), 380.f);
        R(w, mx + 32, ry + 58, 380.f, 7.f, {18,18,38}, C_BORDER, 1.f);
        if (barW > 0.f)
            R(w, mx + 32, ry + 58, barW, 7.f, distCol(srchResults[i].distance));

        ry += rh + 6.f;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// DELETE SCREEN
// Lists every node. Click the DELETE button next to any row to remove it.
//
// Owner: Areeba Izhar (ai09625)
// File:  nsw_graph.cpp — deleteNode() removes the node and relinks neighbours
// Save:  Shaheer Qureshi (sq09647) — persistence.cpp
// ─────────────────────────────────────────────────────────────────────────────
void drawDelete(sf::RenderWindow& w, sf::Vector2i mouse, bool clicked)
{
    const float mx      = 110.f;
    const float top     = TOPBAR_H + 18.f;
    const float listTop = top + 40.f;
    const float listH   = WIN_H - listTop - 8.f;
    const float itemH   = 48.f;

    T(w, "DELETE A NODE", mx, top, 15, C_TXT, true);
    T(w, "Areeba Izhar  |  ai09625  |  nsw_graph.cpp  deleteNode() with neighbour relinking",
       mx, top + 20, 9, C_MUTED);

    if (!statusMsg.empty())
        T(w, statusMsg, mx + 500, top + 8, 10,
          statusGood ? C_GREEN : C_RED);

    vector<Node*> snap(graph.getAllNodes().begin(), graph.getAllNodes().end());

    if (snap.empty()) {
        T(w, "Graph is empty. Go back and insert some phrases.",
           mx, listTop + 30, 12, C_MUTED);
        return;
    }

    T(w, to_string(snap.size()) + " node(s) in graph — scroll with mouse wheel",
       mx, listTop - 14, 9, C_DIM);

    float maxScroll = max(0.f, (float)snap.size() * itemH - listH);
    delScroll = max(0.f, min(delScroll, maxScroll));

    Node* toDelete = nullptr;
    float iy = listTop - delScroll;

    for (Node* n : snap) {
        if (iy + itemH > listTop && iy < listTop + listH) {
            bool isEP = (n == graph.getEntryPoint());
            R(w, mx, iy, 860.f, itemH - 2.f,
              isEP ? sf::Color{12,40,12} : C_CARD, C_BORDER, 1.f);
            if (isEP) R(w, mx, iy, 3.f, itemH - 2.f, C_GREEN);

            // phrase text
            T(w, "\"" + trunc(n->text, 74) + "\"",
               mx + 10, iy + 7, 12, C_TXT);

            // neighbour count + entry label
            string sub = to_string(n->neighbors.size()) + " neighbour(s)";
            if (isEP) sub += "   |   ENTRY POINT";
            T(w, sub, mx + 10, iy + 26, 9, isEP ? C_GREEN : C_MUTED);

            // DELETE button
            sf::FloatRect br(mx + 768, iy + 10, 84, 28);
            bool hov = br.contains((float)mouse.x, (float)mouse.y);
            R(w, br.left, br.top, br.width, br.height,
              hov ? sf::Color{160,40,40} : C_DELBTN, C_BORDER, 1.f);
            T(w, "DELETE", br.left + 14, br.top + 7, 10, {255,255,255}, true);

            if (hov && clicked && iy >= listTop)
                toDelete = n;
        }
        iy += itemH;
    }

    // Process deletion outside the loop (pointer safety)
    if (toDelete) {
        string saved = toDelete->text;
        graph.deleteNode(toDelete);             // Areeba — nsw_graph.cpp
        saveToFile(&graph, DB_FILE);            // Shaheer — persistence.cpp
        delScroll  = max(0.f, delScroll - itemH);
        statusMsg  = "Deleted \"" + saved
                   + "\"  —  saved to " + DB_FILE;
        statusGood = true;
        srchDone   = false;
        srchResults.clear();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// GRAPH VIEW SCREEN
// Renders every node as a card showing its text and neighbour list.
//
// Owner: Areeba Izhar (ai09625)
// Files: nsw_graph.h / nsw_graph.cpp — adjacency list structure
//        node.h — Node struct (text, numericalVector, neighbors)
// ─────────────────────────────────────────────────────────────────────────────
/
// ─── Main ────────────────────────────────────────────────────────────────────
int main()
{
    // Font fallback chain
    if (!font.loadFromFile("assets/font.ttf"))
    if (!font.loadFromFile("font.ttf"))
    if (!font.loadFromFile("assets/DejaVuSansMono.ttf"))
        font.loadFromFile(
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");

    // Restore saved graph — Shaheer Qureshi (persistence.cpp)
    loadFromFile(&graph, DB_FILE);

    sf::RenderWindow window(
        sf::VideoMode(WIN_W, WIN_H),
        "NSW Graph — Private Local Text Search",
        sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Vector2i mouse   = sf::Mouse::getPosition(window);
        bool         clicked = false;
        float        scroll  = 0.f;

        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();

            if (ev.type == sf::Event::MouseButtonPressed &&
                ev.mouseButton.button == sf::Mouse::Left)
                clicked = true;

            if (ev.type == sf::Event::MouseWheelScrolled)
                scroll = ev.mouseWheelScroll.delta;

            // Keyboard text — only on main screen
            if (cur == S_MAIN && ev.type == sf::Event::TextEntered) {
                uint32_t ch = ev.text.unicode;
                if (ch == 8) {                        // backspace
                    if (!mainInput.empty()) mainInput.pop_back();
                } else if (ch >= 32 && ch < 127) {   // printable ASCII
                    mainInput += static_cast<char>(ch);
                }
            }

            // Escape returns to main from any screen
            if (ev.type == sf::Event::KeyPressed &&
                ev.key.code == sf::Keyboard::Escape)
            {
                cur = S_MAIN;
                statusMsg.clear();
            }
        }

        // Route scroll to the right screen
        if (cur == S_DELETE) delScroll -= scroll * 42.f;
        if (cur == S_GRAPH)  gfxScroll -= scroll * 42.f;

        // ── Draw ──────────────────────────────────────────────────────────────
        window.clear(C_BG);
        drawTopbar(window, mouse, clicked);

        switch (cur) {
            case S_MAIN:   drawMain  (window, mouse, clicked); break;
            case S_DELETE: drawDelete(window, mouse, clicked); break;
            case S_GRAPH:  drawGraph (window, mouse, clicked); break;
        }

        window.display();
    }
    return 0;
}