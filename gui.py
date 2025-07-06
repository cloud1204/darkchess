import tkinter as tk
from game import Board  # your logic file

class DarkChessGUI:
    def __init__(self, root, init_board = None):
        self.root = root
        self.root.title("Chinese Dark Chess")
        if not init_board:
            self.board = Board()
            self.board.random_generate()
            self.turn = -1  # 0 or 1
        else:
            self.board = init_board
            self.turn = 0

        self.selected_pos = None
        self.buttons = [[None for _ in range(8)] for _ in range(4)]

        self.status_label = tk.Label(root, text=f"Player {self.turn}'s turn")
        self.status_label.grid(row=0, column=0, columnspan=8)

        # for i in range(4):
        #     for j in range(8):
        #         btn = tk.Button(root, text="?", width=6, height=3,
        #                         command=lambda x=i, y=j: self.on_click(x, y))
        #         btn.grid(row=i + 1, column=j)
        #         self.buttons[i][j] = btn
        self.CELL_SIZE = 70
        self.canvas_grid = [[None for _ in range(8)] for _ in range(4)]

        for i in range(4):
            for j in range(8):
                canvas = tk.Canvas(root, width=self.CELL_SIZE, height=self.CELL_SIZE, bg="lightgray", highlightthickness=1)
                canvas.grid(row=i+1, column=j)
                canvas.bind("<Button-1>", lambda e, x=i, y=j: self.on_click(x, y))
                self.canvas_grid[i][j] = canvas

        self.update_gui()

    def update_gui(self):
        for i in range(4):
            for j in range(8):
                canvas = self.canvas_grid[i][j]
                canvas.delete("all")
                piece = self.board.squares[i][j]

                if piece is None:
                    continue

                # Draw circle if revealed
                if piece.revealed:
                    color = "red" if piece.color == 1 else "black"
                    canvas.create_oval(5, 5, self.CELL_SIZE - 5, self.CELL_SIZE - 5, fill='white')
                    chinese_map = {('K', 0): '將', ('G', 0): '士', ('M', 0): '象', ('R', 0): '車', 
                                   ('N', 0): '馬', ('C', 0): '包', ('P', 0): '卒', ('K', 1): '帥', 
                                   ('G', 1): '仕', ('M', 1): '相', ('R', 1): '俥', ('N', 1): '傌', 
                                   ('C', 1): '炮', ('P', 1): '兵'}

                    canvas.create_text(self.CELL_SIZE // 2, self.CELL_SIZE // 2, text=f"{chinese_map[(piece.type, piece.color)]}", 
                                       fill=color, font=("Helvetica", self.CELL_SIZE // 2, "bold"))
                else:
                    # Draw face-down piece
                    canvas.create_oval(5, 5, self.CELL_SIZE - 5, self.CELL_SIZE - 5, fill='green', outline='black')


        status_color = "red" if self.turn == 1 else "black"
        if self.turn == -1:
            status_color = 'black'
        self.status_label.config(text=f"Player {self.turn}'s turn", fg=status_color)
        # print(self.board.get_unrevealed())

    def on_click(self, x, y):
        piece = self.board.squares[x][y]


        if piece and not piece.revealed:
            piece.revealed = True
            if self.turn == -1: ## first move decides color
                self.turn = piece.color
            self.turn = 1 - self.turn
            self.update_gui()
            return

        if not piece or piece.color != self.turn:
            if self.selected_pos:
                # move or capture
                from_pos = self.selected_pos
                to_pos = [x, y]
                moved = self.try_move(from_pos, to_pos)
                self.selected_pos = None
                if moved:
                    self.turn = 1 - self.turn
                    self.update_gui()
            return

        if piece.color != self.turn:
            self.selected_pos = None
            return

        # Select a piece
        self.selected_pos = [x, y]

    def try_move(self, from_pos, to_pos):
        A = self.board.squares[from_pos[0]][from_pos[1]]
        B = self.board.squares[to_pos[0]][to_pos[1]]
        if A and A.revealed and (not B or (B.revealed and self.board.eatable(from_pos, to_pos))):
            self.board.squares[to_pos[0]][to_pos[1]] = A
            self.board.squares[from_pos[0]][from_pos[1]] = None
            return True
        return False
