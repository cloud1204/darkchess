import tkinter as tk
from gui import DarkChessGUI
from game import Board, Piece

if __name__ == "__main__":
    root = tk.Tk()
    board = Board()
    board.squares[1][0] = Piece('R', 1, revealed=True)
    board.squares[1][2] = Piece('K', 0, revealed=True)
    board.squares[2][4] = Piece('G', 1, revealed=True)
    board.squares[1][7] = Piece('M', 0, revealed=True)
    app = DarkChessGUI(root, init_board=board)
    root.mainloop()