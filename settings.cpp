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

template <> int Settings::get<int>(const QString &key, int defValue)
{
    return m_settings->value(key, QVariant(defValue)).toInt();
}

template <> bool Settings::get<bool>(const QString &key, bool defValue)
{
    return m_settings->value(key, QVariant(defValue)).toBool();
}

template <> QString Settings::get<QString>(const QString &key, QString defValue)
{
    return m_settings->value(key, QVariant(defValue)).toString();
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
