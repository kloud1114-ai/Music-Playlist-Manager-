#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#define MAX_TEXT 100
#define FILE_NAME "playlist.txt"

typedef struct Song {
    char title[MAX_TEXT];
    char artist[MAX_TEXT];
    int duration;
    struct Song *prev;
    struct Song *next;
} Song;

static Song *head = NULL;
static Song *tail = NULL;
static Song *current = NULL;
static int songCount = 0;



static void trimNewline(char *text) {
    text[strcspn(text, "\r\n")] = '\0';
}

static int readLine(const char *prompt, char *buffer, size_t size) {
    int ch;

    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    if (strchr(buffer, '\n') == NULL && strchr(buffer, '\r') == NULL) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Discard the remainder of an overlong input line. */
        }
        printf("Input too long. Please try again.\n");
        return -1;
    }

    trimNewline(buffer);
    return 1;
}

static int readInt(const char *prompt, int min, int max) {
    char buffer[100];
    char *end;
    long value;
    int result;

    while (1) {
        result = readLine(prompt, buffer, sizeof(buffer));
        if (result == 0) {
            return -1;
        }
        if (result < 0) {
            continue;
        }

        errno = 0;
        value = strtol(buffer, &end, 10);
        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (buffer[0] != '\0' && end != buffer && *end == '\0' &&
            errno != ERANGE && value >= min && value <= max) {
            return (int)value;
        }

        printf("Enter a valid number between %d and %d.\n", min, max);
    }
}

static int readText(const char *prompt, char *buffer, size_t size) {
    int result;

    while (1) {
        result = readLine(prompt, buffer, size);
        if (result <= 0) {
            return result;
        }

        if (buffer[0] == '\0') {
            printf("This field cannot be empty.\n");
            continue;
        }

        if (strchr(buffer, '|') != NULL) {
            printf("The character '|' is reserved and cannot be used.\n");
            continue;
        }

        return 1;
    }
}

/* ---------- DOUBLY LINKED LIST ---------- */

static Song *createSong(const char *title, const char *artist, int duration) {
    Song *newSong = (Song *)malloc(sizeof(Song));

    if (newSong == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    snprintf(newSong->title, sizeof(newSong->title), "%s", title);
    snprintf(newSong->artist, sizeof(newSong->artist), "%s", artist);
    newSong->duration = duration;
    newSong->prev = NULL;
    newSong->next = NULL;

    return newSong;
}

static void appendNode(Song *newSong) {
    if (newSong == NULL) {
        return;
    }

    if (head == NULL) {
        head = tail = newSong;
    } else {
        newSong->prev = tail;
        tail->next = newSong;
        tail = newSong;
    }

    songCount++;
    if (current == NULL) {
        current = newSong;
    }
}

static Song *getSongAt(int position) {
    Song *temp;
    int i;

    if (position < 1 || position > songCount) {
        return NULL;
    }

    if (position <= songCount / 2 + songCount % 2) {
        temp = head;
        for (i = 1; i < position; i++) {
            temp = temp->next;
        }
    } else {
        temp = tail;
        for (i = songCount; i > position; i--) {
            temp = temp->prev;
        }
    }

    return temp;
}

static void displayPlaylist(void) {
    Song *temp;
    int position = 1;

    if (head == NULL) {
        printf("\nYour playlist is empty.\n");
        return;
    }

    printf("\n================ YOUR PLAYLIST ================\n");
    temp = head;

    while (temp != NULL) {
        printf("%c %2d. %-24s | %-18s | %02d:%02d\n",
               temp == current ? '>' : ' ',
               position,
               temp->title,
               temp->artist,
               temp->duration / 60,
               temp->duration % 60);
        temp = temp->next;
        position++;
    }

    printf("------------------------------------------------\n");
    printf("Total songs: %d\n", songCount);
    printf("> = Currently selected song\n");
}

static void addSong(void) {
    char title[MAX_TEXT];
    char artist[MAX_TEXT];
    int minutes;
    int seconds;
    Song *newSong;

    if (readText("Song title: ", title, sizeof(title)) <= 0) {
        return;
    }
    if (readText("Artist name: ", artist, sizeof(artist)) <= 0) {
        return;
    }

    minutes = readInt("Duration - minutes: ", 0, 9999);
    if (minutes < 0) {
        return;
    }

    seconds = readInt("Duration - seconds (0-59): ", 0, 59);
    if (seconds < 0) {
        return;
    }

    if (minutes > (INT_MAX - seconds) / 60) {
        printf("Duration is too large.\n");
        return;
    }

    newSong = createSong(title, artist, minutes * 60 + seconds);
    if (newSong == NULL) {
        return;
    }

    appendNode(newSong);
    printf("Song added successfully.\n");
}

static void playSong(void) {
    int position;

    if (head == NULL) {
        printf("Add songs before selecting a song.\n");
        return;
    }

    displayPlaylist();
    position = readInt("Select song number: ", 1, songCount);
    if (position < 0) {
        return;
    }

    current = getSongAt(position);
    printf("\nNow playing: %s - %s [%02d:%02d]\n",
           current->title, current->artist,
           current->duration / 60, current->duration % 60);
}

static void nextSong(void) {
    if (current == NULL) {
        printf("Your playlist is empty.\n");
    } else if (current->next == NULL) {
        printf("You are at the last song.\n");
    } else {
        current = current->next;
        printf("Now playing: %s - %s\n", current->title, current->artist);
    }
}

static void previousSong(void) {
    if (current == NULL) {
        printf("Your playlist is empty.\n");
    } else if (current->prev == NULL) {
        printf("You are at the first song.\n");
    } else {
        current = current->prev;
        printf("Now playing: %s - %s\n", current->title, current->artist);
    }
}

static void searchSong(void) {
    char query[MAX_TEXT];
    Song *temp;
    int position = 1;
    int found = 0;

    if (head == NULL) {
        printf("Your playlist is empty.\n");
        return;
    }
    if (readText("Enter song title to search: ", query, sizeof(query)) <= 0) {
        return;
    }

    printf("\nSearch results:\n");
    temp = head;

    while (temp != NULL) {
        if (strstr(temp->title, query) != NULL) {
            printf("%d. %s - %s [%02d:%02d]\n",
                   position, temp->title, temp->artist,
                   temp->duration / 60, temp->duration % 60);
            found = 1;
        }
        temp = temp->next;
        position++;
    }

    if (!found) {
        printf("No matching song found.\n");
    }
}

static void detachNode(Song *song) {
    if (song->prev != NULL) {
        song->prev->next = song->next;
    } else {
        head = song->next;
    }

    if (song->next != NULL) {
        song->next->prev = song->prev;
    } else {
        tail = song->prev;
    }

    song->prev = NULL;
    song->next = NULL;
}

static void insertAtPosition(Song *song, int position) {
    Song *before;
    Song *previous;

    if (position == 1) {
        song->prev = NULL;
        song->next = head;

        if (head != NULL) {
            head->prev = song;
        } else {
            tail = song;
        }
        head = song;
    } else if (position == songCount + 1) {
        song->next = NULL;
        song->prev = tail;

        if (tail != NULL) {
            tail->next = song;
        } else {
            head = song;
        }
        tail = song;
    } else {
        before = getSongAt(position);
        previous = before->prev;

        song->prev = previous;
        song->next = before;
        previous->next = song;
        before->prev = song;
    }

    songCount++;
}

static void deleteSong(void) {
    int position;
    Song *target;
    Song *replacement;

    if (head == NULL) {
        printf("Your playlist is empty.\n");
        return;
    }

    displayPlaylist();
    position = readInt("Song number to delete: ", 1, songCount);
    if (position < 0) {
        return;
    }

    target = getSongAt(position);
    replacement = target->next != NULL ? target->next : target->prev;

    if (target == current) {
        current = replacement;
    }

    detachNode(target);
    songCount--;
    free(target);

    if (songCount == 0) {
        head = tail = current = NULL;
    }

    printf("Song deleted successfully.\n");
}

static void moveSong(void) {
    int from;
    int to;
    Song *target;
    Song *savedCurrent;

    if (songCount < 2) {
        printf("At least two songs are required to reorder.\n");
        return;
    }

    displayPlaylist();
    from = readInt("Move song number: ", 1, songCount);
    if (from < 0) {
        return;
    }
    to = readInt("New position: ", 1, songCount);
    if (to < 0) {
        return;
    }

    if (from == to) {
        printf("The song is already at that position.\n");
        return;
    }

    target = getSongAt(from);
    savedCurrent = current;

    detachNode(target);
    songCount--;
    insertAtPosition(target, to);
    current = savedCurrent;

    printf("Song moved successfully.\n");
}

/* ---------- FILE HANDLING ---------- */

static int savePlaylist(void) {
    FILE *file;
    Song *temp;

    file = fopen(FILE_NAME, "w");
    if (file == NULL) {
        perror("Could not save playlist");
        return 0;
    }

    temp = head;
    while (temp != NULL) {
        if (fprintf(file, "%s|%s|%d\n",
                    temp->title, temp->artist, temp->duration) < 0) {
            printf("Error writing playlist file.\n");
            fclose(file);
            return 0;
        }
        temp = temp->next;
    }

    if (fclose(file) != 0) {
        printf("Error closing playlist file.\n");
        return 0;
    }

    printf("Playlist saved to %s.\n", FILE_NAME);
    return 1;
}

static void clearPlaylist(void) {
    Song *temp = head;

    while (temp != NULL) {
        Song *next = temp->next;
        free(temp);
        temp = next;
    }

    head = tail = current = NULL;
    songCount = 0;
}

static void loadPlaylist(void) {
    FILE *file;
    char line[2 * MAX_TEXT + 40];
    int loaded = 0;

    file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("No saved playlist found. Add some songs first.\n");
        return;
    }

    clearPlaylist();

    while (fgets(line, sizeof(line), file) != NULL) {
        char *first;
        char *second;
        char *end;
        long duration;
        Song *newSong;

        trimNewline(line);

        first = strchr(line, '|');
        if (first == NULL) {
            continue;
        }
        *first = '\0';

        second = strchr(first + 1, '|');
        if (second == NULL) {
            continue;
        }
        *second = '\0';

        if (line[0] == '\0' || first[1] == '\0') {
            continue;
        }

        errno = 0;
        duration = strtol(second + 1, &end, 10);
        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (errno == ERANGE || end == second + 1 || *end != '\0' ||
            duration < 0 || duration > INT_MAX) {
            continue;
        }

        newSong = createSong(line, first + 1, (int)duration);
        if (newSong == NULL) {
            printf("Stopped loading because memory allocation failed.\n");
            break;
        }

        appendNode(newSong);
        loaded++;
    }

    if (ferror(file)) {
        printf("Warning: an error occurred while reading the playlist.\n");
    }

    fclose(file);
    printf("Loaded %d song(s) from %s.\n", loaded, FILE_NAME);
}

/* ---------- MAIN MENU ---------- */

int main(void) {
    int choice;

    printf("========================================\n");
    printf("        MUSIC PLAYLIST MANAGER\n");
    printf("        Doubly Linked List in C\n");
    printf("========================================\n");

    loadPlaylist();

    while (1) {
        printf("\n--------------- MENU ----------------\n");
        printf("1.  Add song\n");
        printf("2.  Display playlist\n");
        printf("3.  Play selected song\n");
        printf("4.  Next song\n");
        printf("5.  Previous song\n");
        printf("6.  Search song\n");
        printf("7.  Delete song\n");
        printf("8.  Move song\n");
        printf("9.  Save playlist\n");
        printf("10. Reload saved playlist\n");
        printf("0.  Save and exit\n");
        printf("-------------------------------------\n");

        choice = readInt("Enter your choice: ", 0, 10);

        if (choice < 0) {
            printf("\nInput ended. Exiting safely.\n");
            clearPlaylist();
            return 0;
        }

        switch (choice) {
            case 1:
                addSong();
                break;
            case 2:
                displayPlaylist();
                break;
            case 3:
                playSong();
                break;
            case 4:
                nextSong();
                break;
            case 5:
                previousSong();
                break;
            case 6:
                searchSong();
                break;
            case 7:
                deleteSong();
                break;
            case 8:
                moveSong();
                break;
            case 9:
                savePlaylist();
                break;
            case 10:
                loadPlaylist();
                break;
            case 0:
                savePlaylist();
                clearPlaylist();
                printf("Playlist manager closed. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
