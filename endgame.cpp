#include <bits/stdc++.h>
using namespace std;
#define fi first 
#define se second
struct Piece {
    char type;  // e.g., "K", "G", ...
    int color;    // 0 = black, 1 = red
    int cnt; // for storing global pieces 
    pair<int, int> pos;  // (x, y)
    Piece (char _type, int _color): type(_type), color(_color){}
    Piece (char _type, int _color, int _cnt): type(_type), color(_color), cnt(_cnt){}
    Piece (char _type, int _color, pair<int, int> _pos): type(_type), color(_color), pos(_pos){}
    bool operator < (const Piece &other) const{
        if (color != other.color) return color < other.color;
        if (type != other.type) return type < other.type;
        return pos < other.pos;
    }
    bool operator != (const Piece &other) const{
        return color != other.color or type != other.type or pos != other.pos;
    }
    bool operator == (const Piece &other) const{
        return color == other.color and type == other.type and pos == other.pos;
    }
};
bool eatable(const Piece& A, const Piece& B){
    if (A.color == B.color) return false;
    static map<char, int> partial_order = {
        {'K', 6}, {'G', 5}, {'M', 4}, {'R', 3}, {'N', 2}, {'C', 1}, {'P', 0}
    };
    if (A.type == 'K' and B.type == 'P') return false;
    if (A.type == 'P' and B.type == 'K') return true;
    return partial_order[A.type] >= partial_order[B.type];
}

vector<Piece> global = {Piece('K', 0, 1), Piece('M', 0, 1), Piece('G', 1, 1), Piece('P', 1, 1)};

struct State {
    vector<Piece> pieces;
    int turn;

    vector<Piece> eatens;

    bool operator < (const State &other) const{
        if (turn != other.turn) return turn < other.turn;
        if (pieces.size() != other.pieces.size()) return pieces.size() < other.pieces.size();
        for (int i = 0; i < pieces.size(); i++){
            if (pieces[i] != other.pieces[i]) return pieces[i] < other.pieces[i];
        }
        return false;
    }

    State(const vector<Piece> _pieces, int _turn) : pieces(_pieces), turn(_turn) {
        sort(pieces.begin(), pieces.end());

        for (auto g_piece : global){
            int cnt = 0;
            for (auto piece : pieces){
                if (piece.type == g_piece.type and piece.color == g_piece.color) cnt++;
            }
            if (cnt < g_piece.cnt) eatens.push_back(Piece(g_piece.color, g_piece.type, g_piece.cnt - cnt));
        }
    }

    bool in_bound(int x, int y) const {
        return 0 <= x && x < 4 && 0 <= y && y < 8;
    }

    vector<State> get_rev_neighbors() const{
        vector<State> neighbors;
        for (const auto& piece : pieces) {
            if (piece.color == turn) continue;
            int x = piece.pos.first;
            int y = piece.pos.second;

            vector<pair<int, int>> dirs = {{x, y + 1}, {x, y - 1}, {x + 1, y}, {x - 1, y}};
            for (const auto& [nx, ny] : dirs) {
                if (!in_bound(nx, ny)) continue;
                bool succ = true;
                vector<Piece> new_pieces;
                for (const auto& other : pieces) {
                    if (other.pos == make_pair(nx, ny)) {
                        succ = false;
                        break;
                    }
                    else if (other == piece){
                        new_pieces.push_back(Piece(piece.type, piece.color, make_pair(nx, ny)));
                    }
                    else {
                        new_pieces.push_back(other);
                    }
                }
                if (!succ) continue;
                neighbors.push_back(State(new_pieces, turn ^ 1));
                for (auto eaten : eatens){
                    if (!eatable(piece, eaten)) continue;
                    new_pieces.push_back(Piece(eaten.type, eaten.color, piece.pos));
                    neighbors.push_back(State(new_pieces, turn ^ 1));
                    new_pieces.pop_back();
                }

                
            }
        }
        return neighbors;
    }

    vector<State> get_neighbors() const {
        vector<State> neighbors;
        for (const auto& piece : pieces) {
            if (piece.color != turn) continue;
            int x = piece.pos.first;
            int y = piece.pos.second;

            vector<pair<int, int>> dirs = {{x, y + 1}, {x, y - 1}, {x + 1, y}, {x - 1, y}};
            for (const auto& [nx, ny] : dirs) {
                if (!in_bound(nx, ny)) continue;

                vector<Piece> new_pieces;
                bool succ = true;

                for (const auto& other : pieces) {
                    if (other.pos == piece.pos) {
                        new_pieces.emplace_back(Piece(piece.type, piece.color, {nx, ny}));
                    } else if (other.pos == make_pair(nx, ny)) {
                        if (!eatable(piece, other)) {
                            succ = false;
                            break;
                        }
                        // else: skip adding, it's eaten
                    } else {
                        new_pieces.push_back(other);
                    }
                }

                if (succ) {
                    neighbors.emplace_back(State(new_pieces, turn ^ 1)); 
                }
            }
        }
        return neighbors;
    }

    void Print(){
        for (auto piece : pieces){
            cout << piece.type << ' ' << piece.color << ' ' << piece.pos.fi << ' ' << piece.pos.se << ' ';
        }
        cout << '\n';
    }
};



struct EndgameTable {

    int cnt = 0;
    map<State, int> table, degree;
    map<State, bool> visited, in_terminals;
    queue<State> q;
    vector<State> terminals;
    EndgameTable(){}
    void init_degree(State state){
        auto it = degree.find(state);
        if (visited[state] or it != degree.end())
            return;
        degree[state] = state.get_neighbors().size();
    }
    vector<pair<int, int>> generate_all_pos(){
        vector<pair<int, int>> pos;
        for (int i = 0; i < 4; i++) for (int j = 0; j < 8; j++) pos.push_back({i, j});
        return pos;
    }
    void generate_terminals(vector<Piece> pieces, int cur){
        map<pair<int, int>, bool> used;
        int n = pieces.size();
        if (cur >= n) {
            for (int msk = 1; msk < (1 << n); msk++){
                vector<Piece> tmp;
                for (int i = 0; i < n; i++){
                    if (msk >> i & 1) tmp.push_back(pieces[i]);
                }
                State terminal = State(tmp, pieces.back().color ^ 1);
                if (!in_terminals[terminal]){
                    terminals.push_back(terminal);
                    in_terminals[terminal] = true;
                }
            }
            return;
        }
        for (int i = 0; i < cur; i++){
            used[pieces[i].pos] = 1;
        }
        for (auto pos : generate_all_pos()){
            if (used[pos]) continue;
            pieces[cur].pos = pos;
            generate_terminals(pieces, cur + 1);
        }
    }
    EndgameTable(const vector<Piece>& pieces) {
        // vector<pair<int, int>> blocks;
        // for (int i = 0; i < 8; ++i)
        //     for (int j = 0; j < 4; ++j)
        //         blocks.emplace_back(j, i); // (x, y)

        // int total = 0;

        // // All combinations of 4 positions
        // vector<vector<pair<int, int>>> position_combos;
        // vector<pair<int, int>> pos_combo;
        // function<void(int)> choose_positions = [&](int start) {
        //     if (pos_combo.size() == 4) {
        //         position_combos.push_back(pos_combo);
        //         return;
        //     }
        //     for (int i = start; i < blocks.size(); ++i) {
        //         pos_combo.push_back(blocks[i]);
        //         choose_positions(i + 1);
        //         pos_combo.pop_back();
        //     }
        // };
        // choose_positions(0);

        // for (const auto& positions : position_combos) {
        //     vector<int> perm_idx = {0, 1, 2, 3};
        //     do {
        //         vector<Piece> assigned;
        //         for (int i = 0; i < 4; ++i) {
        //             Piece p = pieces[perm_idx[i]];
        //             p.pos = positions[i];
        //             assigned.push_back(p);
        //         }
        //         build(State(assigned, 0));
        //         build(State(assigned, 1));
        //         total += 2;
        //     } while (next_permutation(perm_idx.begin(), perm_idx.end()));
        // }
        // cout << "Total built: " << total << endl;
        vector<Piece> group[2];
        for (auto piece : pieces){
            group[piece.color].push_back(piece);
        }
        generate_terminals(group[0], 0);
        generate_terminals(group[1], 0);
        cout << "total terminals:" << terminals.size() << '\n';
        for (auto state : terminals){
            visited[state] = 1;
            table[state] = query(state);
            q.push(state);
        }
        bfs();
    }
    void bfs(){
        while (!q.empty()){
            cnt++;
            auto state = q.front();
            int result = table[state];
            q.pop();
            // cout << "cur node:\n";
            // state.Print();
            for (auto nei : state.get_rev_neighbors()){
                //nei.Print();
                init_degree(nei);
                if (result == -1){
                    table[nei] = 1;
                    q.push(nei);
                } 
                else if (result == 1){
                    degree[nei]--;
                    if (!degree[nei]){
                        table[nei] = table[nei] == 1 ? 1 : -1;
                        q.push(nei);
                    }
                }
            }
        }
    }

    int build(const State& state) {
        auto it = table.find(state);
        if (it != table.end())
            return it->second;
        //cnt++;

        //assert(cnt <= 2000000);

        table[state] = -2;  // mark as visiting

        int count0 = 0, count1 = 0;
        for (const auto& piece : state.pieces) {
            if (piece.color == 0) count0++;
            if (piece.color == 1) count1++;
        }

        if (count0 == 0)
            return table[state] = (state.turn == 1 ? 1 : -1);
        else if (count1 == 0)
            return table[state] = (state.turn == 0 ? 1 : -1);

        auto neighbors = state.get_neighbors();
        int result = -1;

        for (const auto& neighbor : neighbors) {
            int tmp = build(neighbor);
            if (tmp == -1) {
                result = 1;
                break;
            } else if (tmp == -2 || tmp == 0) {
                result = 0;
            }
        }

        return table[state] = result;
    }
    int query(State state){
        return build(state);
    }
};
int main(){
    // cout << eatable(Piece('P', 1), Piece('R', 0)) << '\n';
    // return 0;
    EndgameTable table({Piece('K', 0), Piece('G', 1), Piece('M', 0), Piece('P', 1)});
    cout << "Finish building table, total state visited: " << table.cnt << '\n';
    return 0;
    cout << "Enter Query:\n";

    string query_s;
    int turn = 0;
    while (cin >> query_s >> turn){
        if (query_s.size() % 4){
            cout << "Invalid Format. Try Again.\n";
            continue;
        }
        vector<Piece> pieces;
        int pc_cnt = query_s.size() / 4;
        for (int i = 0; i < pc_cnt; i++){
            char tp = query_s[i * 4];
            int color = query_s[i * 4 + 1] - '0';
            int x = query_s[i * 4 + 2] - '0';
            int y = query_s[i * 4 + 3] - '0';
            pieces.push_back(Piece(tp, color, {x, y}));
        }
        auto v = State(pieces, turn).get_neighbors();
        cout << "neighbors:\n";
        for (auto nei : v){
            for (auto pc : nei.pieces){
                cout << pc.type << pc.color << pc.pos.first << pc.pos.second;
            }
            cout << " " << nei.turn << " nei_result:" << table.query(nei) << '\n';
        }
        cout << "result: " << table.query(State(pieces, turn)) << '\n';
    }
}
