
#include <iostream>
#include<iomanip>
#include<windows.h>
#include<conio.h>
#include <mmsystem.h>
#include<winuser.h>

using namespace std;
void hideConsoleCursor()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
void getRowColbyLeftClick(int& rpos, int& cpos)
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD Events;
    INPUT_RECORD InputRecord;
    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT |
        ENABLE_EXTENDED_FLAGS);
    do
    {
        ReadConsoleInput(hInput, &InputRecord, 1, &Events);
        if (InputRecord.Event.MouseEvent.dwButtonState ==

            FROM_LEFT_1ST_BUTTON_PRESSED)

        {
            cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
            rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
            break;
        }
    } while (true);
}
void gotoRowCol(int rpos, int cpos)
{
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = cpos;
    scrn.Y = rpos;
    SetConsoleCursorPosition(hOuput, scrn);
}
void Color(int k)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, k);
}


const int WIDTH = 100;
const int HEIGHT = 150;
const int JUMP_HEIGHT = 5;
const int MAX_BULLETS = 100;

struct Entity {
    int x, y;
    bool jumping = false;
    int jumpCounter = 0;
    void (*update)(Entity&);
};

struct Bullet {
    int x, y;
    bool active = false;
};

Bullet bullets[MAX_BULLETS];



void playerUpdate(Entity& player) 
{
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
        case 'a': 
            if (player.x > 0) 
                player.x--;
            break;
        case 'd':
            if (player.x < WIDTH - 1) 
                player.x++;
            break;
        case 'w': 
            if(!player.jumping)
            { player.jumping = true; player.jumpCounter = JUMP_HEIGHT; }
            break;
        case ' ': 
            // Fire bullet
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullets[i].active) {
                    bullets[i].x = player.x + 2;
                    bullets[i].y = player.y;
                    bullets[i].active = true;
                    break;
                }
            }
            break;
        }
    }

   
    if (player.jumping) {
        if (player.jumpCounter > 0) {
            player.y--;
            player.jumpCounter--;
        }
        else if (player.y < HEIGHT - 1) {
            player.y++;
        }
        else {
            player.jumping = false;
        }
    }
}

void updateBullets() 
{
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            gotoRowCol(bullets[i].y, bullets[i].x);
            cout << ' ';
            bullets[i].x++;
            if (bullets[i].x >= WIDTH) bullets[i].active = false;
        }
    }
}

void drawBullets() {
    for (int i = 1; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            gotoRowCol(bullets[i].y, bullets[i].x);
            cout << '-';
        }
    }
}

void drawEntity(const Entity& entity) {
    gotoRowCol(entity.y, entity.x);
    cout << char(178) << char(178); // Graphical representation of player
    gotoRowCol(entity.y + 1, entity.x);
    cout << char(178) << char(178);
}

void clearEntity(const Entity& entity) {
    gotoRowCol(entity.y, entity.x);
    cout << "  ";
    gotoRowCol(entity.y + 1, entity.x);
    cout << "  ";
}

int main()
{
    Entity player = { 10, HEIGHT - 2, false, 0, playerUpdate };
    bool running = true;

    while (running) {

        hideConsoleCursor();
        clearEntity(player);
        updateBullets();
        player.update(player);
        drawEntity(player);
        drawBullets();
        Sleep(100);
    }
    return 0;
}
