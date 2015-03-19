#define WINVER 0x0500
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
    for(item = (array) + count; keep; keep = !keep)


static char* windowName = "World of Warcraft";


void sendInput(char char_to_send)
{
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = char_to_send;
    ip.ki.wScan = 0;
    ip.ki.dwFlags = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Send to specified window
    SendInput(1, &ip, sizeof(INPUT));
    // Release key
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

unsigned int
randr(unsigned int min, unsigned int max)
{
    double scaled = (double)rand()/RAND_MAX;
    return (max - min + 1)*scaled + min;
}


void handlePVP()
{
    fprintf(stderr, "Doing magic..\n");
    // F10   = 0x79
    // F11   = 0x7A
    // F12   = 0x7B
    // SPACE = 0x20
    // http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
    sendInput(0x7A);
    sendInput(0x7B);

    // Randomize and tap honor button twice
    int choice = randr(1, 3);
    if (choice == 1)
    {
        sendInput(0x79);
        sleep(1);
        sendInput(0x79);
    }
    // Random jump
    choice = randr(1, 7);
    if (choice == 1)
    {
        sendInput(0x20);
    }
}


int handleWindows(char* windowName)
{
    // Get first window on desktop
    HWND firstwindow = FindWindowEx(NULL, NULL, NULL, NULL);
    HWND window = firstwindow;
    TCHAR windowtext[MAX_PATH];

    // Get console title for safety
    TCHAR consoletitle[MAX_PATH];
    GetConsoleTitle(consoletitle, MAX_PATH);

    int calculateWow = 0;
    while(1)
    {
        // Check window title for match
        GetWindowText(window, windowtext, MAX_PATH);
        if (strstr(windowtext, windowName) != NULL &&
            strcmp(windowtext, consoletitle) != 0)
        {
            int wowpid = GetWindowThreadProcessId(window, NULL);
            fprintf(stderr, "WoW PID: %d. Is this good? Y/N\n", wowpid);
            int good;
            scanf("%s", &good);
            if (toupper(good) == 'Y')
            {
                fprintf(stderr, "Selected PID %d.\n", wowpid);
                break;
            }
            calculateWow++;
        }

        // Get next window
        window = FindWindowEx(NULL, window, NULL, NULL);
        if (window == NULL || window == firstwindow)
        {
            fprintf(stderr, "No more WoW windows.\n");
            exit(0);
        }
    }

    SetForegroundWindow(window);
    return calculateWow;
}


void refocusWindow(int instanceNumber)
{
    HWND firstwindow = FindWindowEx(NULL, NULL, NULL, NULL);
    HWND window = firstwindow;
    TCHAR windowtext[MAX_PATH];

    TCHAR consoletitle[MAX_PATH];
    GetConsoleTitle(consoletitle, MAX_PATH);

    int windowcalc = 0;
    while(1)
    {
        GetWindowText(window, windowtext, MAX_PATH);
        if (strstr(windowtext, windowName) != NULL &&
            strcmp(windowtext, consoletitle) != 0)
        {
            if (windowcalc == instanceNumber)
            {
                break;
            }
            windowcalc++;
        }
        window = FindWindowEx(NULL, window, NULL, NULL);
        if (window == NULL || window == firstwindow)
        {
            fprintf(stderr, "No more WoW windows.\n");
            exit(0);
        }
    }
    SetForegroundWindow(window);
}


volatile sig_atomic_t stop;
void inthand(int signum)
{
    stop = 1;
}


int main(int argc, char **argv)
{
    signal(SIGINT, inthand);
    
    int wowWindow = handleWindows(windowName);
    while(!stop)
    {
        int amount = randr(3, 7);
        sleep(amount);
        refocusWindow(wowWindow);
        handlePVP();
    }
    printf("Exiting safely\n");
    return 0;
}

