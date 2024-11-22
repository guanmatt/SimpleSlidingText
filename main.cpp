#include <ncurses.h>
#include <string.h>
#include <fstream>
#include <algorithm>

#define KEY_CTRL_S 19
#define KEY_ARROW_LEFT 260
#define KEY_ARROW_RIGHT 261

class Buffer
{
public:
    Buffer(size_t initialCapacity = 256) : size(0), capacity(initialCapacity), buf(new char[initialCapacity])
    {
    }

    ~Buffer()
    {
        delete[] buf;
    }

    void append(char c)
    {
        if (size == capacity)
        {
            resize();
        }
        buf[size] = c;
        size++;
    }

    void backspace()
    {
        if (size > 0)
        {
            size--;
        }
    }

    const char *getString(int index = 0)
    {
        // if(index < 0 || index >= size) {
        //     exit(-1);
        // }
        return buf + index;
    }

    const int getSize()
    {
        return size;
    }

private:
    size_t size;
    size_t capacity;
    char *buf;

    void resize()
    {
        capacity *= 2;
        char *new_buf = new char[capacity];

        for (size_t i = 0; i < size; i++)
        {
            new_buf[i] = buf[i];
        }
        delete[] buf;
        buf = new_buf;
    }
};

void saveToFile(char fileName[], const char data[], int n)
{
    std::ofstream ofs(fileName, std::ofstream::trunc);
    ofs.write(data, n);
    ofs.close();
}

int main()
{
    // INIT
    Buffer buf;
    int row, col;
    int ch;
    initscr();
    raw();                // same as cbreak, disables line buffering
    noecho();             // don't echo user input
    keypad(stdscr, TRUE); // allow fn keys
    getmaxyx(stdscr, row, col);

    int valid = true;
    int printIndex = 0;
    int printWindowSize = 10 + 1;
    int deferClear = false;
    while (valid)
    {
        ch = getch();
        switch (ch)
        {
        case 26: // ctrl z
        case 3:  // ctrl c
            valid = false;
            break;
        case KEY_CTRL_S:
        {
            char fileName[] = "output.txt";
            saveToFile(fileName, buf.getString(), buf.getSize());
            break;
        }
        case KEY_BACKSPACE:
            buf.backspace();
            clear();
            break;
        case KEY_ARROW_LEFT:
            printIndex = printIndex == 0 ? 0 : printIndex - 1;
            break;
        case KEY_ARROW_RIGHT:
            printIndex = printIndex == buf.getSize() - 1 ? printIndex : printIndex + 1;
            break;
        case KEY_RESIZE:
            getmaxyx(stdscr, row, col);
            deferClear = true;
            continue;
        default:
            buf.append(ch);
            break;
        }
        if (deferClear) {
            clear();
            deferClear = false;
        }
        mvaddnstr(row - 1, col / 2, buf.getString(printIndex), buf.getSize());//std::min(buf.getSize() - printIndex);
    }
    printf("exiting");
    endwin();
    return 0;
}