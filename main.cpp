#include <ncurses.h>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <string.h>

#define KEY_CTRL_S 19
#define KEY_ARROW_LEFT 260
#define KEY_ARROW_RIGHT 261
#define INITIAL_CAPACITY 8 // 256

// left points to start of gap
// right points to start of content on right of gap
class Buffer
{
public:
    Buffer(size_t initialCapacity = INITIAL_CAPACITY) : left(0), right(initialCapacity), capacity(initialCapacity)
    {
        buf = new char[initialCapacity];
        memset(buf, '-', initialCapacity);
    }

    ~Buffer()
    {
        delete[] buf;
    }

    void debug(int x, int y)
    {
        std::string s = "left:" + std::to_string(left) + ", right: " + std::to_string(right) + ", capacity: " + std::to_string(capacity);
        mvaddnstr(x, y, s.c_str(), s.length());
    }

    void insert(char c)
    {
        if (left == right)
        {
            resize();
        }
        buf[left] = c;
        left++;
    }

    void erase(bool backwards = true)
    {
        if (backwards && left > 0) {
            left--;
        } else if (!backwards && right < capacity) {
            right++;
        }
    }

    bool moveLeft()
    {
        if (left > 0)
        {
            left--;
            right--;
            buf[right] = buf[left];
            return true;
        }
        return false;
    }

    bool moveRight()
    {
        if (right < capacity)
        {
            buf[left] = buf[right];
            left++;
            right++;
            return true;
        }
        return false;
    }

    std::string getRawStr()
    {
        std::string str(buf, capacity);
        return str;
    }

    std::string getStr()
    {
        std::string str;
        if (left > 0)
        {
            str.append(buf, left);
        }
        if (capacity - right > 0)
        {
            str.append(buf + right, capacity - right);
        }
        return str;
    }

    int getCursorPosition() {
        return left;
    }

private:
    int left;
    int right;
    size_t capacity;
    char *buf;

    void resize()
    {
        size_t new_capacity = 2 * capacity;
        char *new_buf = new char[new_capacity];
        memset(new_buf, '-', new_capacity);
        for (int i = 0; i < left; i++)
        {
            new_buf[i] = buf[i];
        }

        for (int i = right; i < capacity; i++)
        {
            new_buf[i + capacity] = buf[i];
        }

        delete[] buf;
        buf = new_buf;
        right = right + capacity;
        capacity = new_capacity;
    }
};

void saveToFile(char fileName[], const char data[], int n)
{
    std::ofstream ofs(fileName, std::ofstream::trunc);
    ofs.write(data, n);
    ofs.close();
}

void print(Buffer* buf, int row) {
    buf->debug(row - 3, 0);
    std::string rawStr = buf->getRawStr();
    mvaddnstr(row - 2, 0, rawStr.c_str(), rawStr.size());

    std::string str = buf->getStr();
    mvaddnstr(row - 1, 0, str.c_str(), str.size());
    move(row - 1, buf->getCursorPosition());
}

int main()
{
    // INIT
    Buffer buf;
    int row, col;
    int y, x;
    int ch;
    initscr();
    raw();                // same as cbreak, disables line buffering
    noecho();             // don't echo user input
    keypad(stdscr, TRUE); // allow fn keys
    getmaxyx(stdscr, row, col);
    // attron(A_STANDOUT);
    curs_set(2); // hide cursor

    int valid = true;
    int deferClear = false;

    print(&buf, row);
    while (valid)
    {
        ch = getch();
        switch (ch)
        {
        case KEY_UP:
        case KEY_DOWN:
            // fall through, ignore for now
            break;
        case 26: // ctrl z
        case 3:  // ctrl c
            valid = false;
            break;
        case KEY_CTRL_S:
        {
            char fileName[] = "output.txt";

            std::string str = buf.getStr();
            saveToFile(fileName, str.c_str(), str.size() - 1);
            break;
        }
        case KEY_BACKSPACE:
            buf.erase();
            break;
        case KEY_DC:
            buf.erase(false);
            break;
            // todo delete forward
        case KEY_ARROW_LEFT:
            if (buf.moveLeft())
            {
                getyx(stdscr, y, x); /* get the current curser position */
                x--;
            }
            break;
        case KEY_ARROW_RIGHT:
            if (buf.moveRight())
            {
                getyx(stdscr, y, x); /* get the current curser position */
                x++;
            }
            break;
        case KEY_RESIZE:
            getmaxyx(stdscr, row, col);
            deferClear = true;
            continue;
        default:
            buf.insert(ch);
            break;
        }
        if (deferClear)
        {
            clear();
            deferClear = false;
        }
        erase();
        print(&buf, row);
    }
    printf("exiting");
    endwin();
    return 0;
}