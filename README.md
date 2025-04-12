# Terminal Tetris (C + ncurses)

This is a simple Tetris-inspired terminal game written in C using the `ncurses` library. Blocks fall, the game speeds up, and your score increases as you clear rows. It uses a grid-based system, basic struct logic, and handles real-time input.

---

## 🎮 How It Works

- 7 classic Tetris block types (L, T, Z, square, line, etc.)
- Uses `ncurses` for real-time rendering in the terminal
- Blocks drop automatically over time
- Player can control blocks with:
  - `a` to move left
  - `d` to move right
  - `s` to drop down
  - `w` to rotate
- Rows are cleared when full, and the game gets faster
- Score increases for every row cleared
- Game ends when a new block can’t be placed

---

## 🧠 Features

- Custom `Block` struct with rotation logic
- Collision and boundary checking
- Dynamic block generation using `malloc`
- Frame-timed automatic falling using `gettimeofday()`
- Classic terminal output using `printw` and `ncurses` screen refresh
- Progressive difficulty with faster drop intervals

---

## 🛠 How to Build and Run

### 🔧 Dependencies

You’ll need:

- A C compiler (`gcc`)
- `ncurses` library

### 🧪 Compile

```bash
gcc -o tetris tetris.c -lncurses
```

### ▶️ Run

```bash
./tetris
```

Use your keyboard to play!

---

## 📦 File Structure

- `tetris.c`: Full source code for the game
- `README.md`: You're reading it!

---

## 🚧 Known Improvements

- No high score tracking yet
- Rotation can clip against the wall
- Game over screen could be cleaner
- Doesn’t support hard drop or hold blocks

---

## 🪙 Score Logic

- +100 points per row cleared
- Game speeds up slightly after each clear

---

## 🧹 Cleanup

All dynamically allocated memory is freed before exit. `endwin()` ensures the terminal resets properly after the game.

---

## 📄 License

MIT — use it, modify it, enjoy it.

---

This was a fun weekend project to get more practice with pointers, structs, dynamic memory, and real-time terminal apps. Let me know if you want a version with sound or leaderboard support!
