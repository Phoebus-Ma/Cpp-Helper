/**
 * Win32 Run command sample code.
 * ref: [https://docs.microsoft.com/en-us/windows/win32/shell/launch].
 * 
 * License - MIT.
*/

#include <iostream>
#include <windows.h>


#define BUFF_SIZE                   256


using namespace std;


/**
 * RunCmd - Launch app by cmd.
*/
int RunCmd(wstring app, wstring args, wstring opt)
{
    SHELLEXECUTEINFO ShExecInfo;

    ShExecInfo.cbSize       = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask        = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd         = NULL;
    ShExecInfo.lpVerb       = opt.c_str();
    ShExecInfo.lpFile       = app.c_str();
    ShExecInfo.lpParameters = args.c_str();
    ShExecInfo.lpDirectory  = NULL;
    ShExecInfo.nShow        = SW_HIDE;
    ShExecInfo.hInstApp     = NULL;

    ShellExecuteEx(&ShExecInfo);

    if (0 != ShExecInfo.hProcess)
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    return 0;
}

/**
 * Main function.
*/
int main(void)
{
    wstring app  = L"cmd";
    wstring path = L"";
    wstring args = L"/c winsat formal >> ";
    wstring opt  = L"runas";                    // admin

    WIN32_FIND_DATA FindData;

    /* Get current path. */
    wchar_t cur_path[BUFF_SIZE] = { 0 };

    GetCurrentDirectory(BUFF_SIZE, cur_path);

    path += cur_path;
    path += L"\\result.txt";

    args += path;

    /* Find if the file exists. */
    if (INVALID_HANDLE_VALUE != FindFirstFile(path.c_str(), &FindData))
    {
        /* Delete the file. */
        DeleteFile(path.c_str());
    }

    /* Launch app by cmd as admin. */
    RunCmd(app, args, opt);

    return 0;
}
