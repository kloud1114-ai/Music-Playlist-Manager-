# Music Playlist Manager

A console-based Music Playlist Manager built in C using a **Doubly Linked List**. This project demonstrates fundamental data structures and operations through a simple, menu-driven application.

## Features

- **Add Songs:** Add a song with its title, artist, and duration.
- **Display Playlist:** View all songs in the playlist.
- **Navigate Songs:** Move to the next or previous song.
- **Search Songs:** Search for songs by title.
- **Delete Songs:** Remove songs from the playlist.
- **Reorder Songs:** Move songs to different positions.
- **Save Playlist:** Store playlist data in a text file.
- **Load Playlist:** Load previously saved playlist data.

## Data Structure Used

This project uses a **Doubly Linked List**.

Each song is stored in a node containing:
- Song title
- Artist name
- Duration
- Pointer to the previous song
- Pointer to the next song

This structure allows traversal in both forward and backward directions.

## Technologies Used

- C Programming Language
- Doubly Linked List
- Pointers and Structures
- Dynamic Memory Allocation
- File Handling
- GCC Compiler
- Visual Studio Code

## How to Run

### Prerequisites

Install a C compiler such as GCC and set up your terminal to recognize the `gcc` command.

### Compile

```bash
gcc -std=c11 -Wall -Wextra main.c -o playlist.exe
```

### Run on Windows PowerShell

```powershell
.\playlist.exe
```

### Run on Linux or macOS

```bash
gcc -std=c11 -Wall -Wextra main.c -o playlist
./playlist
```

## Project Structure

```text
Music-Playlist-Manager/
├── main.c
├── README.md
├── .gitignore
└── playlist.txt
```

`playlist.txt` is created or used when saving and loading playlist data; it may not exist until the program saves a playlist.

## Limitations

- The program simulates song navigation; it does not play actual audio.
- Playlist persistence uses a text file.
- Song searches may be case-sensitive.

## Learning Outcomes

- Understanding doubly linked lists.
- Practising pointers and structures in C.
- Implementing insertion, deletion, searching, and traversal.
- Using dynamic memory allocation.
- Performing basic file operations.

## Author

**Saswat, Abhishek, Ishan Sharma, Shrreyah, Parth Rane**

A Data Structures and Algorithms (DSA) mini-project.
