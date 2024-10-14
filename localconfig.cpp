#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

#include "localconfig.h"

#define __CONFIG_NAME__ "/.Dian-Captrue-config"

LocalConfig::LocalConfig()
    : hotkey("F1"), copyWithMd(true), runWhenLogin(false), language("中文") {
    loadConfig();
}

QString LocalConfig::configFilePath()
{
    // 获取当前目录的路径
    QString dirPath = QDir::currentPath();
    return dirPath + __CONFIG_NAME__;
}

void LocalConfig::saveConfig()
{
    QString path = configFilePath();
    QFile configFile(path);

    if (configFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&configFile);
        out << "hotkey=" << hotkey << "\n";
        out << "copyWithMd=" << (copyWithMd ? "true" : "false") << "\n";
        out << "runWhenLogin=" << (runWhenLogin ? "true" : "false") << "\n";
        out << "language=" << language << "\n";
        configFile.close();
    }
    else
    {
        qDebug() << "写出配置失败";
    }
}

void LocalConfig::loadConfig()
{
    QString path = configFilePath();
    QFile configFile(path);

    if (!configFile.exists())
    {
        // 如果文件不存在，初始化为默认值
        hotkey = "F1";
        copyWithMd = true;
        runWhenLogin = false;
        saveConfig(); // 初始化时保存默认值
        return;
    }

    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&configFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList parts = line.split("=");

            if (parts.size() == 2)
            {
                QString key = parts[0].trimmed();
                QString value = parts[1].trimmed();

                if (key == "hotkey")
                {
                    hotkey = value;
                }
                else if (key == "copyWithMd")
                {
                    copyWithMd = (value == "true");
                }
                else if (key == "runWhenLogin")
                {
                    runWhenLogin = (value == "true");
                }
                else if (key == "language")
                {
                    language = value;
                }
            }
        }
        configFile.close();
    }
    else
    {
        qDebug() << "读取配置失败";
    }
}

LocalConfig localConfig;
