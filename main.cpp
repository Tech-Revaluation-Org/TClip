#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

vector<string> clipboardHistory;

// Function to add text to clipboard history
void AddToClipboardHistory(const string& text) {
    clipboardHistory.push_back(text);
}

// Function to delete the clipboard history
void DeleteClipboardHistory() {
    clipboardHistory.clear();
    cout << "Clipboard history deleted." << endl;
}

// Function to simulate typing out text
void SimulateTyping(const string& text, int typingSpeedMs) {
    for (char c : text) {
        cout << c;
        // Adjust typing speed by changing sleep duration
        this_thread::sleep_for(chrono::milliseconds(typingSpeedMs)); // Typing speed from config
    }
    cout << endl;
}

// Function to paste items from clipboard history one by one
void PasteFromHistory(int typingSpeedMs) {
    static size_t currentIndex = 0;

    if (currentIndex < clipboardHistory.size()) {
        // Paste the current item
        SimulateTyping(clipboardHistory[currentIndex], typingSpeedMs); // Adjust typing speed as needed
        currentIndex++;
    } else {
        cout << "No more items in clipboard history." << endl;
    }
}

// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* pkbhs = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            if ((pkbhs->vkCode == 'V') && GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU)) {
                // Ctrl + Alt + V pressed, simulate typing
                if (!clipboardHistory.empty()) {
                    // Read typing speed from config.json
                    ifstream configFile("config.json");
                    if (!configFile.is_open()) {
                        cerr << "Failed to open config.json" << endl;
                        return 1;
                    }

                    json config;
                    configFile >> config;
                    configFile.close();

                    int typingSpeedMs = config["typing_speed_ms"];
                    
                    // Simulate typing out the last clipboard item
                    SimulateTyping(clipboardHistory.back(), typingSpeedMs); // Adjust typing speed as needed
                }
                return 1; // Block default behavior
            }
            else if ((pkbhs->vkCode == 'L') && GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU)) {
                // Ctrl + Alt + L pressed, paste items from history
                // Read typing speed from config.json
                ifstream configFile("config.json");
                if (!configFile.is_open()) {
                    cerr << "Failed to open config.json" << endl;
                    return 1;
                }

                json config;
                configFile >> config;
                configFile.close();

                int typingSpeedMs = config["typing_speed_ms"];

                PasteFromHistory(typingSpeedMs);
                return 1; // Block default behavior
            }
            else if ((pkbhs->vkCode == 'D') && GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_MENU)) {
                // Ctrl + Alt + D pressed, delete clipboard history
                DeleteClipboardHistory();
                return 1; // Block default behavior
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    // Install keyboard hook
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    
    if (hHook == NULL) {
        cout << "Failed to install keyboard hook" << endl;
        return 1;
    }

    cout << "Clipboard Manager" << endl;

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Unhook keyboard hook
    UnhookWindowsHookEx(hHook);

    return 0;
}
