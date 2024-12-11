#include "startwindow.hpp"
#include <iostream>
#include <mutex>
#include <vector>
#include <wchar.h>
// 全局变量
HINSTANCE hInst;

HWND hEdit;
HWND main_win;
HWND new_game;
HWND old_game;
HWND creat_new;
HWND listbox;
HWND load;
int exit_msg;
std::string save_name = "";
std::string explain = "";

    HBITMAP bitmap;
int state = 0; // 0 首页 1 新建游戏 2 旧存档
HWND back;
std::mutex start_window_mutex;
bool CreateFolderIfNotExists(const TCHAR* folderPath)
{
    DWORD dwAttrib = GetFileAttributes(folderPath);

    // 检查文件夹是否存在且是目录
    if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        // 文件夹不存在，创建文件夹
        if (CreateDirectory(folderPath, NULL)) {
            std::wcout << _T("文件夹创建成功: ") << folderPath << std::endl;
            return true; // 成功创建文件夹
        } else {
            MessageBox(NULL, _T("Failed to create folder!"), _T("Error"), MB_OK | MB_ICONERROR);
            return false; // 创建文件夹失败
        }
    }

    std::wcout << _T("文件夹已存在: ") << folderPath << std::endl;
    return true; // 文件夹已存在
}
void ListSubdirectories(const TCHAR* dirPath, std::vector<std::string>& subdirs)
{
    TCHAR searchPath[MAX_PATH];
    WIN32_FIND_DATA findFileData;

    // 构造搜索路径，使用通配符 * 来匹配所有文件和文件夹
    _stprintf(searchPath, _T("%s\\*"), dirPath);

    // 使用 FindFirstFile 来获取第一个文件或文件夹
    HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcout << _T("无法打开目录: ") << dirPath << std::endl;
        return;
    }

    // 遍历目录中的文件和子文件夹
    do {
        // 检查文件是否是文件夹
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 排除 "." 和 ".."
            if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0) {
                // 将子文件夹名存入 vector
                subdirs.push_back(std::string(findFileData.cFileName));
            }
        }

    } while (FindNextFile(hFind, &findFileData) != 0);

    // 关闭搜索句柄
    FindClose(hFind);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE: {
        // 创建一个编辑控件
        CreateFolderIfNotExists(_T(SAVE_PATH));
        bitmap = (HBITMAP)LoadImage(NULL, _T(START_BACKGROUND_IMAGE), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (bitmap == NULL) {
            MessageBox(hWnd, _T("Failed to load bitmap!"), _T("Error"), MB_OK | MB_ICONERROR);
        }
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            _T("EDIT"), // 控件类名
            _T(""), // 初始文字
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            620 - 150, 550, 300, 30, // 控件位置和大小
            hWnd, // 父窗口句柄
            (HMENU)1, // 控件ID
            hInst, // 应用实例句柄
            NULL // 附加参数
        );
        ShowWindow(hEdit, SW_HIDE); // 显示编辑控件
        new_game = CreateWindow(_T("BUTTON"), _T("新游戏"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 500, 300, 30, hWnd, (HMENU)2, hInst, NULL);
        old_game = CreateWindow(_T("BUTTON"), _T("存档"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 550, 300, 30, hWnd, (HMENU)3, hInst, NULL);
        back = CreateWindow(_T("BUTTON"), _T("返回"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1000, 600, 100, 30, hWnd, (HMENU)4, hInst, NULL);
        creat_new = CreateWindow(_T("BUTTON"), _T("创建"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 600, 300, 30, hWnd, (HMENU)5, hInst, NULL);
        load = CreateWindow(_T("BUTTON"), _T("加载"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 600, 300, 30, hWnd, (HMENU)7, hInst, NULL);
        listbox = CreateWindow(_T("LISTBOX"), _T("存档"), WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 620 - 150, 200, 300, 30, hWnd, (HMENU)6, hInst, NULL);
        CreateWindow(_T("BUTTON"), _T("玩法说明"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1100, 10, 100, 30, hWnd, (HMENU)8, hInst, NULL);
        ShowWindow(listbox, SW_HIDE);
        ShowWindow(creat_new, SW_HIDE);
        ShowWindow(back, SW_HIDE);
        ShowWindow(load, SW_HIDE);
        
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) {

            if (HIWORD(wParam) == EN_CHANGE) { // 监控文本变化事件
                TCHAR buffer[256];
                GetWindowText(hEdit, buffer, 256); // 获取编辑控件内容
                save_name = buffer;
                OutputDebugString(buffer); // 输出到调试窗口
                InvalidateRect(hWnd, NULL, TRUE);
                // UpdateWindow(hWnd);
            }
        }
        if (LOWORD(wParam) == 2) {
            if (state == 0) {
                ShowWindow(hEdit, SW_SHOW);
                ShowWindow(back, SW_SHOW);
                ShowWindow(new_game, SW_HIDE);
                ShowWindow(old_game, SW_HIDE);
                ShowWindow(creat_new, SW_SHOW);
                ShowWindow(listbox, SW_HIDE);
                ShowWindow(load, SW_HIDE);
                state = 1;
            }
        }
        if (LOWORD(wParam) == 3) {
            if (state == 0) {
                SendMessage(listbox, LB_RESETCONTENT, 0, 0);
                ShowWindow(hEdit, SW_HIDE);
                ShowWindow(back, SW_SHOW);
                ShowWindow(new_game, SW_HIDE);
                ShowWindow(old_game, SW_HIDE);
                ShowWindow(creat_new, SW_HIDE);
                ShowWindow(listbox, SW_SHOW);
                ShowWindow(load, SW_SHOW);
                std::vector<std::string> subdirs;
                CreateFolderIfNotExists(_T(SAVE_PATH));
                ListSubdirectories(_T(SAVE_PATH), subdirs);
                for (int i = 0; i < subdirs.size(); i++) {
                    SendMessage(listbox, LB_ADDSTRING, 0, (LPARAM)subdirs[i].c_str());
                }
                SetWindowPos(listbox, 0, 620 - 150, 200, 300, 20 * subdirs.size(), SWP_NOZORDER);
                state = 2;
            }
        }
        if (LOWORD(wParam) == 4) {

            ShowWindow(hEdit, SW_HIDE);
            ShowWindow(back, SW_HIDE);
            ShowWindow(new_game, SW_SHOW);
            ShowWindow(old_game, SW_SHOW);
            ShowWindow(creat_new, SW_HIDE);
            ShowWindow(listbox, SW_HIDE);
            ShowWindow(load, SW_HIDE);
            state = 0;
        }
        if (LOWORD(wParam) == 5) {
            if (HIWORD(wParam) == BN_CLICKED) {
                if (state == 1) {
                    start_window_mutex.lock();
                    save_name = std::string(SAVE_PATH) + save_name + "\\\0";
                    CreateFolderIfNotExists(_T(save_name.c_str()));
                    start_window_mutex.unlock();
                    start_window_mutex.lock();
                    exit_msg = 1;
                    start_window_mutex.unlock();
                }
            }

            break;
        }
        if (LOWORD(wParam) == 7) {
            int index = SendMessage(listbox, LB_GETCURSEL, 0, 0);
            if (index == -1) {
                MessageBox(hWnd, _T("请选择一个存档"), _T("错误"), MB_OK | MB_ICONERROR);
            } else {
                TCHAR buffer[256] = { 0 };
                SendMessage(listbox, LB_GETTEXT, index, (LPARAM)buffer);
                save_name = std::string(SAVE_PATH) + buffer + "\\";
                OutputDebugString(save_name.c_str());
                start_window_mutex.lock();
                TextOut(GetDC(hWnd), 0, 0, ("正在加载" + save_name).c_str(), 8 + save_name.length());
                Sleep(2000);
                exit_msg = 2;
                start_window_mutex.unlock();
            }
        }
        if (LOWORD(wParam) == 8) {
            
            MessageBox(hWnd, _T(explain.c_str()), _T("玩法说明"), MB_OK | MB_ICONINFORMATION);
        }
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, bitmap);
        BitBlt(hdc, 0, 0, 1240, 720, hdcMem, 0, 0, SRCCOPY);
        DeleteDC(hdcMem);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam; // 获取编辑框的设备上下文
        SetBkMode(hdcEdit, TRANSPARENT); // 设置透明模式
        SetTextColor(hdcEdit, RGB(255, 255, 255)); // 设置文字颜色为黑色
        return (LRESULT)GetStockObject(HOLLOW_BRUSH); // 返回空画刷，使背景透明
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 主函数
int start_screen(HINSTANCE hInstance)
{
    hInst = hInstance;
    explain = "荒野探索是一款开放世界无限地图探险游戏，玩家可以在游戏中探索荒野，收集资源，建造设施。\n";
    explain += "游戏操作如下：\n";
    explain += "WASD：移动\n";
    explain += "鼠标左键：采集资源 或攻击生物\n";
    explain += "鼠标右键：放置物品 或食用食物\n";
    explain += "E：打开背包\n";
    explain += "S: 拆分物品\n";
    explain += "Q: 丢弃物品\n";
    explain += "shift+Q:丢弃一组物品\n";
    explain += "1-9: 切换物品\n";
    explain += "ESC：退出游戏\n";
    explain += "F1：打开控制台\n";
    explain += "F2：打开玩法说明\n";
    explain += "F3：打开调试信息\n";
    // 注册窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("TextInputApp");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    // 创建主窗口
    HWND hWnd = CreateWindow(
        _T("TextInputApp"), // 窗口类名
        _T("荒野探索"), // 窗口标题
        WS_OVERLAPPEDWINDOW, // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT, 1240, 720,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return FALSE;
    main_win = hWnd;
    ShowWindow(hWnd, 1);
    UpdateWindow(hWnd);
    // 消息循环
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (exit_msg != 0) {
            break;
        }
    }
    DestroyWindow(hWnd);
    return exit_msg;
}
