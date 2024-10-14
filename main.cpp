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

void runAsAdmin() {
    if (!isRunAsAdmin()) {
        wchar_t szPath[MAX_PATH];
        GetModuleFileNameW(NULL, szPath, ARRAYSIZE(szPath));

        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (!ShellExecuteExW(&sei)) {
            DWORD dwError = GetLastError();
            if (dwError == ERROR_CANCELLED) {
                // 用户取消了 UAC 提示
                qWarning("用户取消了权限提升");
            }
        } else {
            QCoreApplication::quit();  // 提升权限成功后退出当前实例
        }
    }
}

int main(int argc, char *argv[])
{
    runAsAdmin();
	QApplication a(argc, argv);
	CaptureApplication w;
    w.switchLanguage("en");
	QApplication::setQuitOnLastWindowClosed(false);
	return QApplication::exec();
}
