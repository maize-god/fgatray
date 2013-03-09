#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
private:
    Settings();
    ~Settings();

    static Settings* _instance;

    QSettings* m_settings;

public:
    static Settings& instance() {
        if(_instance == 0) {
            _instance = new Settings();
        }

        return *_instance;
    }

    static void shutdown() {
        if(_instance != 0) {
            delete _instance;
            _instance = 0;
        }
    }

    template <typename T> get(const QString& key, T defValue);
    template <typename T> set(const QString& key, T value) {
        m_settings->setValue(key, QVariant(value));
    }

    int getInt(const QString& key, int defValue = 0) {
        return get<int>(key, defValue);
    }

    bool getBool(const QString& key, bool defValue = false) {
        return get<bool>(key, defValue);
    }

    QString getString(const QString& key, QString defValue = QString()) {
        return get<QString>(key, defValue);
    }

    void context(const QString& ctx = QString());
};

#endif // SETTINGS_H
