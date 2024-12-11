#include "startwindow.hpp"
#include <iostream>
#include <mutex>
#include <vector>
#include <wchar.h>
// ȫ�ֱ���
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
int state = 0; // 0 ��ҳ 1 �½���Ϸ 2 �ɴ浵
HWND back;
std::mutex start_window_mutex;
bool CreateFolderIfNotExists(const TCHAR* folderPath)
{
    DWORD dwAttrib = GetFileAttributes(folderPath);

    // ����ļ����Ƿ��������Ŀ¼
    if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        // �ļ��в����ڣ������ļ���
        if (CreateDirectory(folderPath, NULL)) {
            std::wcout << _T("�ļ��д����ɹ�: ") << folderPath << std::endl;
            return true; // �ɹ������ļ���
        } else {
            MessageBox(NULL, _T("Failed to create folder!"), _T("Error"), MB_OK | MB_ICONERROR);
            return false; // �����ļ���ʧ��
        }
    }

    std::wcout << _T("�ļ����Ѵ���: ") << folderPath << std::endl;
    return true; // �ļ����Ѵ���
}
void ListSubdirectories(const TCHAR* dirPath, std::vector<std::string>& subdirs)
{
    TCHAR searchPath[MAX_PATH];
    WIN32_FIND_DATA findFileData;

    // ��������·����ʹ��ͨ��� * ��ƥ�������ļ����ļ���
    _stprintf(searchPath, _T("%s\\*"), dirPath);

    // ʹ�� FindFirstFile ����ȡ��һ���ļ����ļ���
    HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        std::wcout << _T("�޷���Ŀ¼: ") << dirPath << std::endl;
        return;
    }

    // ����Ŀ¼�е��ļ������ļ���
    do {
        // ����ļ��Ƿ����ļ���
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // �ų� "." �� ".."
            if (_tcscmp(findFileData.cFileName, _T(".")) != 0 && _tcscmp(findFileData.cFileName, _T("..")) != 0) {
                // �����ļ��������� vector
                subdirs.push_back(std::string(findFileData.cFileName));
            }
        }

    } while (FindNextFile(hFind, &findFileData) != 0);

    // �ر��������
    FindClose(hFind);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE: {
        // ����һ���༭�ؼ�
        CreateFolderIfNotExists(_T(SAVE_PATH));
        bitmap = (HBITMAP)LoadImage(NULL, _T(START_BACKGROUND_IMAGE), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (bitmap == NULL) {
            MessageBox(hWnd, _T("Failed to load bitmap!"), _T("Error"), MB_OK | MB_ICONERROR);
        }
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            _T("EDIT"), // �ؼ�����
            _T(""), // ��ʼ����
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            620 - 150, 550, 300, 30, // �ؼ�λ�úʹ�С
            hWnd, // �����ھ��
            (HMENU)1, // �ؼ�ID
            hInst, // Ӧ��ʵ�����
            NULL // ���Ӳ���
        );
        ShowWindow(hEdit, SW_HIDE); // ��ʾ�༭�ؼ�
        new_game = CreateWindow(_T("BUTTON"), _T("����Ϸ"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 500, 300, 30, hWnd, (HMENU)2, hInst, NULL);
        old_game = CreateWindow(_T("BUTTON"), _T("�浵"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 550, 300, 30, hWnd, (HMENU)3, hInst, NULL);
        back = CreateWindow(_T("BUTTON"), _T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1000, 600, 100, 30, hWnd, (HMENU)4, hInst, NULL);
        creat_new = CreateWindow(_T("BUTTON"), _T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 600, 300, 30, hWnd, (HMENU)5, hInst, NULL);
        load = CreateWindow(_T("BUTTON"), _T("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620 - 150, 600, 300, 30, hWnd, (HMENU)7, hInst, NULL);
        listbox = CreateWindow(_T("LISTBOX"), _T("�浵"), WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 620 - 150, 200, 300, 30, hWnd, (HMENU)6, hInst, NULL);
        CreateWindow(_T("BUTTON"), _T("�淨˵��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1100, 10, 100, 30, hWnd, (HMENU)8, hInst, NULL);
        ShowWindow(listbox, SW_HIDE);
        ShowWindow(creat_new, SW_HIDE);
        ShowWindow(back, SW_HIDE);
        ShowWindow(load, SW_HIDE);
        
        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) {

            if (HIWORD(wParam) == EN_CHANGE) { // ����ı��仯�¼�
                TCHAR buffer[256];
                GetWindowText(hEdit, buffer, 256); // ��ȡ�༭�ؼ�����
                save_name = buffer;
                OutputDebugString(buffer); // ��������Դ���
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
                MessageBox(hWnd, _T("��ѡ��һ���浵"), _T("����"), MB_OK | MB_ICONERROR);
            } else {
                TCHAR buffer[256] = { 0 };
                SendMessage(listbox, LB_GETTEXT, index, (LPARAM)buffer);
                save_name = std::string(SAVE_PATH) + buffer + "\\";
                OutputDebugString(save_name.c_str());
                start_window_mutex.lock();
                TextOut(GetDC(hWnd), 0, 0, ("���ڼ���" + save_name).c_str(), 8 + save_name.length());
                Sleep(2000);
                exit_msg = 2;
                start_window_mutex.unlock();
            }
        }
        if (LOWORD(wParam) == 8) {
            
            MessageBox(hWnd, _T(explain.c_str()), _T("�淨˵��"), MB_OK | MB_ICONINFORMATION);
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
        HDC hdcEdit = (HDC)wParam; // ��ȡ�༭����豸������
        SetBkMode(hdcEdit, TRANSPARENT); // ����͸��ģʽ
        SetTextColor(hdcEdit, RGB(255, 255, 255)); // ����������ɫΪ��ɫ
        return (LRESULT)GetStockObject(HOLLOW_BRUSH); // ���ؿջ�ˢ��ʹ����͸��
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ������
int start_screen(HINSTANCE hInstance)
{
    hInst = hInstance;
    explain = "��Ұ̽����һ����������޵�ͼ̽����Ϸ����ҿ�������Ϸ��̽����Ұ���ռ���Դ��������ʩ��\n";
    explain += "��Ϸ�������£�\n";
    explain += "WASD���ƶ�\n";
    explain += "���������ɼ���Դ �򹥻�����\n";
    explain += "����Ҽ���������Ʒ ��ʳ��ʳ��\n";
    explain += "E���򿪱���\n";
    explain += "S: �����Ʒ\n";
    explain += "Q: ������Ʒ\n";
    explain += "shift+Q:����һ����Ʒ\n";
    explain += "1-9: �л���Ʒ\n";
    explain += "ESC���˳���Ϸ\n";
    explain += "F1���򿪿���̨\n";
    explain += "F2�����淨˵��\n";
    explain += "F3���򿪵�����Ϣ\n";
    // ע�ᴰ����
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("TextInputApp");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return FALSE;

    // ����������
    HWND hWnd = CreateWindow(
        _T("TextInputApp"), // ��������
        _T("��Ұ̽��"), // ���ڱ���
        WS_OVERLAPPEDWINDOW, // ������ʽ
        CW_USEDEFAULT, CW_USEDEFAULT, 1240, 720,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return FALSE;
    main_win = hWnd;
    ShowWindow(hWnd, 1);
    UpdateWindow(hWnd);
    // ��Ϣѭ��
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
