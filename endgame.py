class State:
    def __init__(self, pieces, turn):
        self.pieces = pieces
        self.turn = turn # 0 for black, 1 for red
    def in_bound(x, y):
        return 0 <= x < 4 and 0 <= y < 8
    def get_neighbors(self):
        import copy
        neighbors = []
        for piece in self.pieces:
            [tp, color] = piece[0]
            if color != self.turn:
                continue
            [x, y] = piece[1]
            for (_x, _y) in [(x, y + 1), (x, y - 1), (x + 1, y), (x - 1, y)]:
                if not self.in_bound(_x, _y):
                    continue
                occ = [(_piece, pos) for (_piece, pos) in self.pieces if pos == [_x, _y]]
                if occ == []:
                    new_pieces = []
                    for (_piece, pos) in self.pieces:
                        if pos == piece[1]:
                            new_pieces.append((_piece, (_x, _y)))
                        else:
                            pass






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
        print(assignments[:10])
    def build(self, state):
        if len([c for ((tp, c), _) in state.pieces if c == 0]) == 0:
            self.table[state] = (state.turn == 1)
            return self.table[state]
        if len([c for ((tp, c), _) in state.pieces if c == 1]) == 0:
            self.table[state] = (state.turn == 0)
            return self.table[state]
        
        

tmp = EndgameTable(pieces=[('K', 0), ('G', 1), ('M', 0), ('R', 1)])