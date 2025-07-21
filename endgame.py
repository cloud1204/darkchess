class State:
    def __init__(self, pieces, turn):
        self.pieces = pieces
        self.turn = turn # 0 for black, 1 for red
    def in_bound(self, x, y):
        return 0 <= x < 4 and 0 <= y < 8
    def eatable(self, A, B):
        if B[1] == A[1]:
            return False
        partial_order = {'K': 6, 'G': 5, 'M': 4, 'R': 3, 'N': 2, 'C': 1, 'P': 0}
        return partial_order[A[0]] >= partial_order[B[0]]
    def get_neighbors(self):
        neighbors = []
        for piece in self.pieces:
            (tp, color) = piece[0]
            if color != self.turn:
                continue
            (x, y) = piece[1]
            for (_x, _y) in [(x, y + 1), (x, y - 1), (x + 1, y), (x - 1, y)]:
                if not self.in_bound(_x, _y):
                    continue
                new_pieces = []
                succ = True
                for (_piece, pos) in self.pieces:
                    if pos == piece[1]:
                        new_pieces.append((_piece, (_x, _y)))
                    elif pos == (_x, _y): ## old piece
                        if not self.eatable((tp, color), _piece):
                            succ = False
                            break
                        pass # skip append, equal as remove
                    else:
                        new_pieces.append((_piece, pos))
            if succ:
                neighbors.append(State(new_pieces, self.turn ^ 1))
        return neighbors
                            

class EndgameTable:
    def __init__(self, pieces=[]) -> None:

        self.table = {}
        import itertools

        blocks = [(i, j) for i in range(8) for j in range(4)]  # 32 blocks

        # Choose 4 unique blocks, then permute pieces over them
        assignments = []
        for positions in itertools.combinations(blocks, 4):
            for perm in itertools.permutations(pieces):
                assignments.append(list(zip(perm, positions)))
        print(len(assignments))
        for state in assignments:
            self.build(State(state, 0))
            self.build(State(state, 1))

    def build(self, state: State):
        if state in self.table:
            return self.table[tuple(state)]
        self.table[state] = -2
        if len([c for ((tp, c), _) in state.pieces if c == 0]) == 0:
            self.table[state] = 1 if state.turn == 1 else -1
        elif len([c for ((tp, c), _) in state.pieces if c == 1]) == 0:
            self.table[state] = 1 if state.turn == 0 else -1
        else:
            neighbors = state.get_neighbors()

            result = -1
            for _state in neighbors:
                tmp = self.build(_state)
                if tmp == -1:
                    result = 1
                    break
                elif tmp == -2 or tmp == 0:
                    result = 0
            self.table[state] = result
        return self.table[state]

try:
    tmp = EndgameTable(pieces=[('K', 0), ('G', 1), ('M', 0), ('R', 1)])
    print("success")
except Exception as e:
    print(e)