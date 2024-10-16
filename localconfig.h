#ifndef LOCALCONFIG_H
#define LOCALCONFIG_H

#include <QString>

class LocalConfig
{
public:
    LocalConfig();

    QString hotkey;
    bool copyWithMd;
    bool runWhenLogin;
    QString language;

    // 保存配置
    void saveConfig();

    // 读取配置
    void loadConfig();

private:
    QString configFilePath(); // 获取配置文件路径
};
extern LocalConfig localConfig;

#endif // LOCALCONFIG_H
