// gui.cpp — NSW Graph SFML Frontend (with animated graph visualiser)
// Compile: g++ -std=c++17 gui.cpp vectorizer.cpp nsw_graph.cpp search.cpp insert.cpp persistence.cpp -lsfml-graphics -lsfml-window -lsfml-system -o gui
// ./gui.exe
// Put any .ttf font inside an  assets/  folder next to gui.cpp.
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <set>
#include "nsw_graph.h"
#include "vectorizer.h"
#include "search.h"
#include "insert.hpp"
#include "persistence.h"
#include "config.h"
using namespace std;
const unsigned WIN_W = 1100;
const unsigned WIN_H = 700;
const unsigned TOPBAR_H = 52;
const string DB_FILE = "database.txt";
const sf::Color C_BG{13, 13, 24};
const sf::Color C_TOP{8, 8, 17};
const sf::Color C_CARD{19, 19, 36};
const sf::Color C_CARD2{23, 23, 45};
const sf::Color C_INPUT{10, 10, 22};
const sf::Color C_BORDER{34, 34, 64};
const sf::Color C_ACCENT{85, 72, 200};
const sf::Color C_ACC2{55, 45, 140};
const sf::Color C_TXT{210, 210, 235};
const sf::Color C_MUTED{85, 85, 125};
const sf::Color C_DIM{45, 45, 80};
const sf::Color C_GREEN{74, 222, 128};
const sf::Color C_RED{248, 113, 113};
const sf::Color C_YELLOW{251, 191, 36};
const sf::Color C_ORANGE{251, 146, 60};
const sf::Color C_IBTN{28, 105, 28};
const sf::Color C_SBTN{55, 45, 160};
const sf::Color C_DELBTN{105, 22, 22};
enum Screen { S_MAIN, S_DELETE, S_GRAPH };
const float VIS_PX = 255.f;
const float NODE_R = 26.f;
const float MY_PI = 3.14159265f;
NSWGraph graph;
Screen cur = S_MAIN;
sf::Font font;
string mainInput;
vector<SearchResult> srchResults;
bool srchDone = false;
string statusMsg;
bool statusGood = true;
float delScroll = 0.f;
map<string, sf::Vector2f> npos;
bool needRepos = true;
enum AnimSt { AS_IDLE, AS_INSERT, AS_SEARCH, AS_DELETE };
AnimSt animSt = AS_IDLE;
sf::Clock animCk;
string hlWalk;
set<string> hlPath;
set<string> hlResult;
string hlNew;
string hlDel;
Node *pendingDel = nullptr;
vector<string> walkSeq;
int walkIdx = 0;
const float STEP_T = 0.45f;
const float INS_T = 0.80f;
const float DEL_T = 0.75f;

void R(sf::RenderWindow &w, float x, float y, float wd, float ht,
       sf::Color fill, sf::Color out = {0, 0, 0, 0}, float th = 0.f)
{
    sf::RectangleShape s({wd, ht});
    s.setPosition(x, y);
    s.setFillColor(fill);
    if (th > 0.f) { s.setOutlineThickness(th); s.setOutlineColor(out); }
    w.draw(s);
}
void T(sf::RenderWindow &w, const string &s,
       float x, float y, unsigned sz, sf::Color c, bool bold = false)
{
    sf::Text t(s, font, sz);
    t.setPosition(x, y);
    t.setFillColor(c);
    if (bold) t.setStyle(sf::Text::Bold);
    w.draw(t);
}
bool Btn(sf::RenderWindow &w, float x, float y, float wd, float ht,
         const string &label, sf::Color bg, sf::Color tc,
         sf::Vector2i mouse, bool clicked, unsigned sz = 12)
{
    sf::FloatRect r(x, y, wd, ht);
    bool hov = r.contains((float)mouse.x, (float)mouse.y);
    sf::Color f = hov
        ? sf::Color(min(255, bg.r + 28), min(255, bg.g + 28), min(255, bg.b + 28))
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
sf::Color distCol(double d)
{
    if (d < 0.0001) return C_GREEN;
    if (d < 0.25)   return C_GREEN;
    if (d < 0.55)   return C_YELLOW;
    if (d < 0.80)   return C_ORANGE;
    return C_RED;
}
string distLbl(double d)
{
    if (d < 0.0001) return "exact match";
    if (d < 0.25)   return "very similar";
    if (d < 0.55)   return "similar";
    if (d < 0.80)   return "loosely related";
    return "unrelated";
}
string shrink(const string &s, size_t n)
{
    return s.size() > n ? s.substr(0, n - 2) + ".." : s;
}
void drawCursor(sf::RenderWindow &w, const string &s,
                float ox, float oy, unsigned sz)
{
    sf::Text m(s, font, sz);
    float cx = ox + m.getLocalBounds().left + m.getLocalBounds().width + 2.f;
    R(w, cx, oy, 1.f, (float)sz + 4, C_TXT);
}

void reposNodes()
{
    const auto &nodes = graph.getAllNodes();
    int n = (int)nodes.size();
    set<string> live;
    for (Node *nd : nodes) live.insert(nd->text);
    for (auto it = npos.begin(); it != npos.end();)
        it = live.count(it->first) ? ++it : npos.erase(it);
    if (n == 0) { needRepos = false; return; }
    float cx = VIS_PX + (WIN_W - VIS_PX) / 2.f;
    float cy = TOPBAR_H + (WIN_H - TOPBAR_H) / 2.f;
    float r;
    if (n == 1)       r = 0.f;
    else if (n <= 4)  r = 140.f;
    else if (n <= 8)  r = 200.f;
    else if (n <= 14) r = 245.f;
    else              r = 265.f;
    for (int i = 0; i < n; i++)
    {
        float angle = 2.f * MY_PI * i / n - MY_PI / 2.f;
        npos[nodes[i]->text] = {cx + r * cosf(angle), cy + r * sinf(angle)};
    }
    needRepos = false;
}

vector<string> simWalk(const vector<double> &qv)
{
    vector<string> path;
    Node *ep = graph.getEntryPoint();
    if (!ep) return path;
    Node *cur = ep;
    set<Node *> seen;
    while (cur)
    {
        path.push_back(cur->text);
        seen.insert(cur);
        double bestD = cosineDistance(qv, cur->numericalVector);
        Node *best = nullptr;
        for (Node *nb : cur->neighbors)
        {
            if (seen.count(nb)) continue;
            double d = cosineDistance(qv, nb->numericalVector);
            if (d < bestD) { bestD = d; best = nb; }
        }
        cur = best;
    }
    return path;
}

void drawEdge(sf::RenderWindow &w, sf::Vector2f a, sf::Vector2f b, sf::Color c)
{
    sf::Vertex line[2] = {sf::Vertex(a, c), sf::Vertex(b, c)};
    w.draw(line, 2, sf::Lines);
}

void drawNodeCircle(sf::RenderWindow &w, sf::Vector2f pos, const string &label,
                    sf::Color fill, sf::Color outline, float ow = 2.f)
{
    sf::CircleShape circle(NODE_R);
    circle.setOrigin(NODE_R, NODE_R);
    circle.setPosition(pos);
    circle.setFillColor(fill);
    circle.setOutlineColor(outline);
    circle.setOutlineThickness(ow);
    w.draw(circle);
    string lbl = shrink(label, 12);
    sf::Text t(lbl, font, 9);
    sf::FloatRect tb = t.getLocalBounds();
    t.setPosition(pos.x - tb.width / 2.f - tb.left, pos.y + NODE_R + 3.f);
    t.setFillColor(C_TXT);
    w.draw(t);
}

void drawTopbar(sf::RenderWindow &w, sf::Vector2i mouse, bool clicked)
{
    R(w, 0, 0, WIN_W, TOPBAR_H, C_TOP);
    R(w, 0, TOPBAR_H - 1, WIN_W, 1, C_BORDER);
    R(w, 14, 10, 36, 32, C_ACCENT);
    T(w, "NSW", 18, 15, 10, {255, 255, 255}, true);
    T(w, "NSW Graph — Private Local Text Search", 58, 13, 13, C_TXT, true);
    T(w, "Trigram Hashing  |  Cosine Similarity  |  Greedy NSW Search", 58, 31, 9, C_DIM);
    T(w, to_string(graph.getNodeCount()) + " nodes", WIN_W - 340, 19, 10, C_DIM);
    if (Btn(w, WIN_W - 295, 10, 95, 32, "DELETE NODES",
            cur == S_DELETE ? C_ACC2 : C_DELBTN, {255, 255, 255}, mouse, clicked, 10))
    { cur = S_DELETE; delScroll = 0.f; statusMsg.clear(); }
    if (Btn(w, WIN_W - 192, 10, 90, 32, "SHOW GRAPH",
            cur == S_GRAPH ? C_ACC2 : sf::Color{35, 35, 70}, {255, 255, 255}, mouse, clicked, 10))
    { cur = S_GRAPH; needRepos = true; }
    if (Btn(w, WIN_W - 94, 10, 82, 32,
            cur == S_MAIN ? "HOME" : "<- BACK",
            cur == S_MAIN ? sf::Color{28, 28, 55} : C_ACC2, {255, 255, 255}, mouse, clicked, 10))
    { cur = S_MAIN; statusMsg.clear(); }
}

// MAIN SCREEN
// INSERT — Areeba Izhar (ai09625) — insert.hpp / insert.cpp
// SEARCH — Noor E Hira  (nh10085) — search.h  / search.cpp
// save   — Shaheer Qureshi (sq09647) — persistence.h / persistence.cpp
void drawMain(sf::RenderWindow &w, sf::Vector2i mouse, bool clicked)
{
    const float mx = 110.f;
    const float top = TOPBAR_H + 40.f;
    T(w, "Enter a word or phrase", mx, top, 15, C_TXT, true);
    T(w, "Click INSERT to add it to the graph.   "
         "Click SEARCH to find the 3 closest matches.",
      mx, top + 22, 10, C_MUTED);
    const float iy = top + 52.f;
    const float iw = 638.f;
    const float ibw = 100.f;
    const float ibh = 42.f;
    const float gap = 8.f;
    R(w, mx, iy, iw, ibh, C_INPUT, C_BORDER, 1.f);
    if (mainInput.empty())
        T(w, "e.g.  machine learning fundamentals", mx + 12, iy + 12, 12, C_DIM);
    else
    {
        T(w, mainInput, mx + 12, iy + 12, 12, C_TXT);
        drawCursor(w, mainInput, mx + 12, iy + 12, 12);
    }
    bool doInsert = Btn(w, mx + iw + gap, iy, ibw, ibh, "INSERT", C_IBTN, {255, 255, 255}, mouse, clicked);
    bool doSearch = Btn(w, mx + iw + gap + ibw + gap, iy, ibw, ibh, "SEARCH", C_SBTN, {255, 255, 255}, mouse, clicked);

    // ── Handle INSERT ─────────────────────────────────────────────────────────
    if (doInsert && !mainInput.empty())
    {
        bool dup = false;
        for (Node *n : graph.getAllNodes())
            if (n->text == mainInput) { dup = true; break; }
        if (dup)
        {
            statusMsg = "\"" + mainInput + "\" already exists in the graph.";
            statusGood = false;
        }
        else
        {
            insert(&graph, mainInput);
            saveToFile(&graph, DB_FILE);
            statusMsg = "Inserted  \"" + mainInput + "\"  —  saved to " + DB_FILE
                      + "  (" + to_string(graph.getNodeCount()) + " nodes total)";
            statusGood = true;
            srchDone = false;
            srchResults.clear();
            needRepos = true;
            mainInput.clear();
        }
    }
    if (doSearch && !mainInput.empty())
    {
        if (graph.getNodeCount() == 0)
        {
            statusMsg = "Graph is empty — insert some phrases first.";
            statusGood = false;
        }
        else
        {
            srchResults = search(&graph, vectorize(mainInput), 3, EF_SEARCH);
            srchDone = true;
            statusMsg = to_string(srchResults.size()) + " result(s) for  \"" + mainInput + "\"";
            statusGood = true;
        }
    }
    const float sy = iy + ibh + 22.f;
    if (!statusMsg.empty())
        T(w, statusMsg, mx, sy, 11, statusGood ? C_GREEN : C_RED);
    if (!srchDone || srchResults.empty()) return;
    const float ry0 = sy + 26.f;
    T(w, "TOP 3 CLOSEST MATCHES", mx, ry0, 9, C_MUTED);
    const sf::Color rankBar[3] = {C_ACCENT, {60, 50, 150}, {35, 30, 90}};
    float ry = ry0 + 16.f;
    for (int i = 0; i < (int)srchResults.size(); i++)
    {
        const float rh = 80.f;
        R(w, mx, ry, 860.f, rh, C_CARD, C_BORDER, 1.f);
        R(w, mx, ry, 3.f, rh, rankBar[i]);
        T(w, "[" + to_string(i + 1) + "]", mx + 10, ry + 10, 11, C_MUTED);
        R(w, mx + 746, ry + 10, 106, 18, C_CARD2, C_BORDER, 1.f);
        T(w, distLbl(srchResults[i].distance), mx + 752, ry + 12, 9, distCol(srchResults[i].distance));
        T(w, "\"" + shrink(srchResults[i].text, 68) + "\"", mx + 32, ry + 10, 13, C_TXT, i == 0);
        ostringstream oss;
        oss << fixed << setprecision(6) << srchResults[i].distance;
        T(w, "dist = " + oss.str(), mx + 32, ry + 34, 11, C_MUTED);
        float barW = min((float)(srchResults[i].distance * 380.f), 380.f);
        R(w, mx + 32, ry + 58, 380.f, 7.f, {18, 18, 38}, C_BORDER, 1.f);
        if (barW > 0.f) R(w, mx + 32, ry + 58, barW, 7.f, distCol(srchResults[i].distance));
        ry += rh + 6.f;
    }
}

// DELETE SCREEN
// Owner: Areeba Izhar (ai09625) — nsw_graph.cpp deleteNode()
// Save:  Shaheer Qureshi (sq09647) — persistence.cpp
void drawDelete(sf::RenderWindow &w, sf::Vector2i mouse, bool clicked)
{
    const float mx = 110.f;
    const float top = TOPBAR_H + 18.f;
    const float listTop = top + 40.f;
    const float listH = WIN_H - listTop - 8.f;
    const float itemH = 48.f;
    T(w, "DELETE A NODE", mx, top, 15, C_TXT, true);
    T(w, "Select a phrase from the list and click DELETE to remove it from the graph.",
      mx, top + 20, 9, C_MUTED);
    if (!statusMsg.empty())
        T(w, statusMsg, mx + 500, top + 8, 10, statusGood ? C_GREEN : C_RED);
    vector<Node *> snap(graph.getAllNodes().begin(), graph.getAllNodes().end());
    if (snap.empty())
    {
        T(w, "Graph is empty. Go back and insert some phrases.", mx, listTop + 30, 12, C_MUTED);
        return;
    }
    T(w, to_string(snap.size()) + " node(s) in graph — scroll with mouse wheel",
      mx, listTop - 14, 9, C_DIM);
    float maxScroll = max(0.f, (float)snap.size() * itemH - listH);
    delScroll = max(0.f, min(delScroll, maxScroll));
    Node *toDelete = nullptr;
    float iy = listTop - delScroll;
    for (Node *n : snap)
    {
        if (iy + itemH > listTop && iy < listTop + listH)
        {
            bool isEP = (n == graph.getEntryPoint());
            R(w, mx, iy, 860.f, itemH - 2.f,
              isEP ? sf::Color{12, 40, 12} : C_CARD, C_BORDER, 1.f);
            if (isEP) R(w, mx, iy, 3.f, itemH - 2.f, C_GREEN);
            T(w, "\"" + shrink(n->text, 74) + "\"", mx + 10, iy + 7, 12, C_TXT);
            string sub = to_string(n->neighbors.size()) + " neighbour(s)";
            if (isEP) sub += "   |   ENTRY POINT";
            T(w, sub, mx + 10, iy + 26, 9, isEP ? C_GREEN : C_MUTED);
            sf::FloatRect br(mx + 768, iy + 10, 84, 28);
            bool hov = br.contains((float)mouse.x, (float)mouse.y);
            R(w, br.left, br.top, br.width, br.height,
              hov ? sf::Color{160, 40, 40} : C_DELBTN, C_BORDER, 1.f);
            T(w, "DELETE", br.left + 14, br.top + 7, 10, {255, 255, 255}, true);
            if (hov && clicked && iy >= listTop) toDelete = n;
        }
        iy += itemH;
    }
    if (toDelete)
    {
        string saved = toDelete->text;
        graph.deleteNode(toDelete);
        saveToFile(&graph, DB_FILE);
        needRepos = true;
        delScroll = max(0.f, delScroll - itemH);
        statusMsg = "Deleted \"" + saved + "\"  —  saved to " + DB_FILE;
        statusGood = true;
        srchDone = false;
        srchResults.clear();
    }
}

// GRAPH VISUALISER
// INSERT animation — Areeba Izhar (ai09625)  — insert.hpp / insert.cpp
// SEARCH animation — Noor E Hira  (nh10085) — search.h  / search.cpp
// DELETE animation — Areeba Izhar (ai09625)  — nsw_graph.cpp deleteNode()
// Auto-save        — Shaheer Qureshi (sq09647) — persistence.cpp
void drawGraph(sf::RenderWindow &w, sf::Vector2i mouse, bool clicked)
{
    R(w, 0, TOPBAR_H, VIS_PX, WIN_H - TOPBAR_H, {7, 7, 16});
    R(w, VIS_PX - 1, TOPBAR_H, 1, WIN_H - TOPBAR_H, C_BORDER);
    float py = TOPBAR_H + 13.f;
    T(w, "GRAPH VISUALISER", 10, py, 12, C_TXT, true);
    py += 24.f;

    // Input box
    R(w, 10, py, 232, 34, C_INPUT, C_BORDER, 1.f);
    string dispIn = shrink(mainInput, 28);
    if (mainInput.empty())
        T(w, "enter phrase...", 14, py + 9, 10, C_DIM);
    else
    {
        T(w, dispIn, 14, py + 9, 10, C_TXT);
        if (mainInput.size() <= 28) drawCursor(w, dispIn, 14, py + 9, 10);
    }
    py += 40.f;

    // Action buttons — CHANGE 4: removed member names from button labels
    bool doIns  = Btn(w, 10, py, 232, 30, "INSERT",  C_IBTN,   {255, 255, 255}, mouse, clicked, 10);
    py += 36.f;
    bool doSrch = Btn(w, 10, py, 232, 30, "SEARCH",  C_SBTN,   {255, 255, 255}, mouse, clicked, 10);
    py += 36.f;
    bool doDel  = Btn(w, 10, py, 232, 30, "DELETE",  C_DELBTN, {255, 255, 255}, mouse, clicked, 10);
    py += 42.f;

    R(w, 10, py, 232, 1, C_BORDER);
    py += 9.f;

    if (!statusMsg.empty())
    {
        T(w, shrink(statusMsg, 30), 10, py, 10, statusGood ? C_GREEN : C_RED);
        py += 14.f;
        if (statusMsg.size() > 30)
            T(w, shrink(statusMsg.substr(30), 30), 10, py, 10, statusGood ? C_GREEN : C_RED);
        py += 16.f;
    }

    if (animSt == AS_SEARCH && !walkSeq.empty())
    {
        T(w, "GREEDY WALK:", 10, py, 9, C_MUTED);
        py += 13.f;
        int show = min(walkIdx + 1, (int)walkSeq.size());
        for (int i = 0; i < show && py < WIN_H - 130; i++)
        {
            bool isCur = (i == walkIdx);
            T(w, (isCur ? "> " : "  ") + shrink(walkSeq[i], 22),
              10, py, 9, isCur ? C_YELLOW : C_DIM);
            py += 12.f;
        }
    }

    float ly = WIN_H - 110.f;
    T(w, "LEGEND:", 10, ly, 9, C_MUTED);
    ly += 13.f;
    struct LegItem { sf::Color c; const char *lbl; };
    const LegItem leg[] = {
        {C_ACCENT,       "normal node"   },
        {{0, 160, 160},  "entry point"   },
        {{0, 200, 220},  "just inserted" },
        {C_YELLOW,       "walk: current" },
        {{80, 60, 200},  "walk: visited" },
        {C_GREEN,        "search result" },
        {C_RED,          "being deleted" },
    };
    for (auto &l : leg)
    {
        sf::CircleShape dot(4.f);
        dot.setFillColor(l.c);
        dot.setPosition(10.f, ly + 1.f);
        w.draw(dot);
        T(w, l.lbl, 22, ly, 9, C_DIM);
        ly += 13.f;
    }

    // INSERT
    if (doIns && !mainInput.empty() && animSt == AS_IDLE)
    {
        bool dup = false;
        for (Node *n : graph.getAllNodes())
            if (n->text == mainInput) { dup = true; break; }
        if (dup)
        {
            statusMsg = "Already exists: \"" + shrink(mainInput, 18) + "\"";
            statusGood = false;
        }
        else
        {
            insert(&graph, mainInput);
            saveToFile(&graph, DB_FILE);
            needRepos = true;
            hlNew = mainInput;
            animSt = AS_INSERT;
            animCk.restart();
            hlResult.clear(); hlPath.clear(); hlWalk.clear();
            statusMsg = "Inserted: \"" + shrink(mainInput, 16) + "\"";
            statusGood = true;
            mainInput.clear();
        }
    }

    // SEARCH
    if (doSrch && !mainInput.empty() && animSt == AS_IDLE)
    {
        if (graph.getNodeCount() == 0)
        {
            statusMsg = "Graph empty — insert first.";
            statusGood = false;
        }
        else
        {
            auto qv = vectorize(mainInput);
            srchResults = search(&graph, qv, 3, EF_SEARCH);
            srchDone = true;
            walkSeq = simWalk(qv);
            walkIdx = 0;
            hlWalk = walkSeq.empty() ? "" : walkSeq[0];
            hlPath.clear(); hlResult.clear();
            animSt = AS_SEARCH;
            animCk.restart();
            statusMsg = "Searching: \"" + shrink(mainInput, 14) + "\"";
            statusGood = true;
        }
    }

    // DELETE
    if (doDel && !mainInput.empty() && animSt == AS_IDLE)
    {
        Node *target = nullptr;
        for (Node *n : graph.getAllNodes())
            if (n->text == mainInput) { target = n; break; }
        if (!target)
        {
            statusMsg = "Not found: \"" + shrink(mainInput, 16) + "\"";
            statusGood = false;
        }
        else
        {
            pendingDel = target;
            hlDel = mainInput;
            animSt = AS_DELETE;
            animCk.restart();
            hlResult.clear(); hlPath.clear(); hlWalk.clear();
            statusMsg = "Deleting: \"" + shrink(mainInput, 14) + "\"";
            statusGood = true;
            mainInput.clear();
        }
    }

    // Animation tick
    float el = animCk.getElapsedTime().asSeconds();
    if (animSt == AS_SEARCH && el > STEP_T)
    {
        animCk.restart();
        if (walkIdx + 1 < (int)walkSeq.size())
        {
            hlPath.insert(walkSeq[walkIdx]);
            walkIdx++;
            hlWalk = walkSeq[walkIdx];
        }
        else
        {
            hlWalk = ""; hlPath.clear();
            for (const auto &r : srchResults) hlResult.insert(r.text);
            animSt = AS_IDLE;
            statusMsg = to_string(srchResults.size()) + " result(s) highlighted in green";
            statusGood = true;
        }
    }
    if (animSt == AS_INSERT && el > INS_T) { hlNew = ""; animSt = AS_IDLE; }
    if (animSt == AS_DELETE && el > DEL_T)
    {
        if (pendingDel)
        {
            graph.deleteNode(pendingDel);
            saveToFile(&graph, DB_FILE);
            pendingDel = nullptr;
            needRepos = true;
            statusMsg = "Deleted. Graph saved.";
            statusGood = true;
        }
        hlDel = ""; animSt = AS_IDLE;
    }

    // Canvas
    if (needRepos) reposNodes();
    const auto &nodes = graph.getAllNodes();
    Node *ep = graph.getEntryPoint();
    if (nodes.empty())
    {
        T(w, "Graph is empty.", VIS_PX + 30, TOPBAR_H + 30, 13, C_MUTED);
        T(w, "Type a phrase and click INSERT.", VIS_PX + 30, TOPBAR_H + 52, 11, C_DIM);
        return;
    }

    // Draw edges
    for (Node *n : nodes)
    {
        auto it = npos.find(n->text);
        if (it == npos.end()) continue;
        for (Node *nb : n->neighbors)
        {
            if (n >= nb) continue;
            auto jt = npos.find(nb->text);
            if (jt == npos.end()) continue;
            bool walkEdge = false;
            for (int i = 0; i + 1 <= walkIdx && i + 1 < (int)walkSeq.size(); i++)
            {
                if ((walkSeq[i] == n->text && walkSeq[i + 1] == nb->text) ||
                    (walkSeq[i] == nb->text && walkSeq[i + 1] == n->text))
                { walkEdge = true; break; }
            }
            sf::Color ec = walkEdge ? sf::Color{140, 120, 255, 230} : sf::Color{40, 40, 80, 180};
            drawEdge(w, it->second, jt->second, ec);
        }
    }

    // Draw nodes
    for (Node *n : nodes)
    {
        auto it = npos.find(n->text);
        if (it == npos.end()) continue;
        bool isEP  = (n == ep);
        bool isDel = (!hlDel.empty()  && n->text == hlDel);
        bool isNew = (!hlNew.empty()  && n->text == hlNew);
        bool isRes = hlResult.count(n->text);
        bool isWlk = (!hlWalk.empty() && n->text == hlWalk);
        bool isPth = hlPath.count(n->text);
        sf::Color fill, outline;
        float ow = 2.f;
        if      (isDel)  { fill = {100,15,15};  outline = C_RED;           ow = 3.f; }
        else if (isRes)  { fill = {15,70,35};   outline = C_GREEN;         ow = 3.f; }
        else if (isWlk)  { fill = {70,60,5};    outline = C_YELLOW;        ow = 3.f; }
        else if (isPth)  { fill = {35,28,90};   outline = {90,75,200};     ow = 2.f; }
        else if (isNew)  { fill = {8,55,75};    outline = {0,200,220};     ow = 3.f; }
        else if (isEP)   { fill = {15,50,25};   outline = {0,160,160};     ow = 2.f; }
        else             { fill = C_CARD;        outline = C_BORDER; }
        drawNodeCircle(w, it->second, n->text, fill, outline, ow);
    }
}

int main()
{
    if (!font.loadFromFile("assets/font.ttf"))
        if (!font.loadFromFile("font.ttf"))
            if (!font.loadFromFile("assets/DejaVuSansMono.ttf"))
                font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf");

    loadFromFile(&graph, DB_FILE);
    needRepos = true;

    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
        "NSW Graph — Private Local Text Search",
        sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        bool clicked = false;
        float scroll = 0.f;
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::MouseButtonPressed &&
                ev.mouseButton.button == sf::Mouse::Left) clicked = true;
            if (ev.type == sf::Event::MouseWheelScrolled)
                scroll = ev.mouseWheelScroll.delta;
            if ((cur == S_MAIN || cur == S_GRAPH) &&
                ev.type == sf::Event::TextEntered)
            {
                uint32_t ch = ev.text.unicode;
                if (ch == 8) { if (!mainInput.empty()) mainInput.pop_back(); }
                else if (ch >= 32 && ch < 127) mainInput += static_cast<char>(ch);
            }
            if (ev.type == sf::Event::KeyPressed)
            {
                if (ev.key.code == sf::Keyboard::Escape)
                { cur = S_MAIN; statusMsg.clear(); }
                if (ev.key.code == sf::Keyboard::Enter && cur == S_MAIN
                    && !mainInput.empty() && graph.getNodeCount() > 0)
                {
                    srchResults = search(&graph, vectorize(mainInput), 3, EF_SEARCH);
                    srchDone = true;
                    statusMsg = to_string(srchResults.size()) + " result(s) for  \"" + mainInput + "\"";
                    statusGood = true;
                }
            }
        }
        if (cur == S_DELETE) delScroll -= scroll * 42.f;
        window.clear(C_BG);
        drawTopbar(window, mouse, clicked);
        switch (cur)
        {
            case S_MAIN:   drawMain  (window, mouse, clicked); break;
            case S_DELETE: drawDelete(window, mouse, clicked); break;
            case S_GRAPH:  drawGraph (window, mouse, clicked); break;
        }
        window.display();
    }
    return 0;
}