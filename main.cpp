#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>

using namespace std;
uintptr_t GetModuleBaseAddress(DWORD dwProcID, char* szModuleName)
{
    uintptr_t ModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 ModuleEntry32;
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &ModuleEntry32))
        {
            do
            {
                if (strcmp(ModuleEntry32.szModule, szModuleName) == 0)
                {
                    ModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32));
        }
        CloseHandle(hSnapshot);
    }
    return ModuleBaseAddress;
}

HANDLE GetProcessHandle(const char *procName)
{
    HANDLE hProc = NULL;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (!strcmp(pe32.szExeFile, procName)) {
                hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return hProc;
}

void createText(HDC hdc, LPCSTR text, int r, int g, int b)
{
    HFONT hfont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Roboto Th");
    SelectObject(hdc, hfont);
    SetTextColor(hdc, RGB(r, g, b));
//    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, 0, 0, text, strlen(text));
}

int getPIDByName(){
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    int pid;
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, "dota2.exe") == 0)
            {
                pid = entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return pid;
}

void isVisible(){
    HDC hdc = GetDC(0);
    //Get dota pid
    int dwProcId = getPIDByName();
    //Get process handle
    HANDLE pHandle = GetProcessHandle("dota2.exe");
    //Get engine2.dll module addr
    uintptr_t enginedllbaseaddr = GetModuleBaseAddress(dwProcId, "engine2.dll");
    //Read memory values with offsets
    uintptr_t temp;
    int isVisible;
    ReadProcessMemory(pHandle, (void*)(enginedllbaseaddr + 0x5727C0), &temp, sizeof(temp), nullptr); //base + engine2.dll module offset //0x575050 to 0x575090
    ReadProcessMemory(pHandle, (void*)(temp + 0x0), &temp, sizeof(temp), nullptr);
    ReadProcessMemory(pHandle, (void*)(temp + 0x28), &temp, sizeof(temp), nullptr);
    ReadProcessMemory(pHandle, (void*)(temp + 0x38), &temp, sizeof(temp), nullptr);
    ReadProcessMemory(pHandle, (void*)(temp + 0x98), &temp, sizeof(temp), nullptr);
    ReadProcessMemory(pHandle, (void*)(temp + 0x170), &temp, sizeof(temp), nullptr); //1B4 to 1B8
    ReadProcessMemory(pHandle, (void*)(temp + 0x0), &temp, sizeof(temp), nullptr);

    while(1){
        ReadProcessMemory(pHandle, (void*)(temp + 0x1E4), &isVisible, sizeof(isVisible), nullptr);
        if (isVisible == 6 | isVisible == 10){
            createText(hdc, "NotSeen", 0, 255, 0);
        }else if (isVisible == 14){
            createText(hdc, "Seen", 255, 0 ,0);
        }else{
            createText(hdc, "running", 0, 0 ,0);
        }
//        Check if exit
        if(GetKeyState(27) & 0x8000 && (GetKeyState(VK_SHIFT) & 0x8000))
        {
            break;
        }
        Sleep(0.2);
    }
}

int main() {
    cout << "[isVisible hack]" << endl;

    if (getPIDByName() == 0){
        cout << "dota2.exe not found. Please open dota 2 first. Exiting...";
        Sleep(2000);
        return 0;
    };
    cout << "Running..." << endl;
    cout << "Press 'Shift+ESC' to exit" << endl;
    isVisible();
    return 0;
}