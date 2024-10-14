#include "CaptureApplication.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <Windows.h>

bool isRunAsAdmin() {
    BOOL fIsRunAsAdmin = FALSE;
    PSID pAdministratorsGroup = NULL;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                                 &pAdministratorsGroup)) {
        CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin);
        FreeSid(pAdministratorsGroup);
    }

    return fIsRunAsAdmin;
}

bool runAsAdmin() {
    QStringList arguments = QCoreApplication::arguments();

    // 如果程序是通过管理员权限启动的，且包含 --elevated 参数，跳过权限提升
    if (arguments.contains("--elevated")) {
        return false;
    }

    if (!isRunAsAdmin()) {
        wchar_t szPath[MAX_PATH];
        GetModuleFileNameW(NULL, szPath, ARRAYSIZE(szPath));

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        // 添加 --elevated 参数，标识为提升权限后的运行
        QStringList args = QCoreApplication::arguments();
        args << "--elevated";
        QString argStr = args.join(' ');

        sei.lpParameters = reinterpret_cast<LPCWSTR>(argStr.utf16());

        if (!ShellExecuteExW(&sei)) {
            DWORD dwError = GetLastError();
            if (dwError == ERROR_CANCELLED) {
                // 用户取消了 UAC 提示
                qWarning("用户取消了权限提升");
            }

            return false;
        } else {
            return true;
        }
    }
}


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    if (runAsAdmin()) return 0;

    CaptureApplication w;

    w.switchLanguage("en");
    QApplication::setQuitOnLastWindowClosed(false);
  	return QApplication::exec();
}
