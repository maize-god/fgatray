#include "settings.h"
#include "defaults.h"

#include <QSettings>

Settings* Settings::_instance = 0;

Settings::Settings()
{
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                               FGA_ORGID, FGA_APPID);
}

Settings::~Settings()
{
    delete m_settings;
}

int Settings::getInt(const QString& key, int defValue) {
    return get<int>(key, defValue);
}

bool Settings::getBool(const QString& key, bool defValue) {
    return get<bool>(key, defValue);
}

QString Settings::getString(const QString& key, QString defValue) {
    return get<QString>(key, defValue);
}

void Settings::context(const QString &ctx)
{
    if(ctx.compare(m_settings->group(), Qt::CaseInsensitive) == 0)
        return;

    if(!m_settings->group().isEmpty())
        m_settings->endGroup();

    if(!ctx.isEmpty()) {
        m_settings->beginGroup(ctx);
    }
}
