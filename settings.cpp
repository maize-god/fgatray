#include "settings.h"
#include "defaults.h"

#include <QSettings>
#include <QCryptographicHash>
#include <QNetworkInterface>
#include <QtGlobal>
#include <foreign/simplecrypt.h>

Settings* Settings::_instance = 0;

union Uint64Container {
    quint64 _u;
    char _c[8];
    qint32 _l[2];

    Uint64Container() : _u(0) {}
};

Settings::Settings()
{
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                               FGA_ORGID, FGA_APPID);
    QString strKey = FGA_CRYPTO_KEY;
    foreach (QNetworkInterface netif, QNetworkInterface::allInterfaces()) {
        if(!(netif.flags() & QNetworkInterface::IsLoopBack)) {
            strKey.append(netif.hardwareAddress());
            break;
        }
    }

    Uint64Container key;
    QByteArray ba = QCryptographicHash::hash(
                strKey.toUtf8(), QCryptographicHash::Md5);
    int i = 0, j = 0;
    while(i < ba.length()) {
        key._c[j++] ^= ba.at(i++);
        j &= 7;
    }
    m_simpleCrypt = new SimpleCrypt(key._u);
}

Settings::~Settings()
{
    delete m_settings;
    delete m_simpleCrypt;
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

void Settings::setEncrypted(const QString &key, QString value)
{
    int num = qrand() % 8 + 1;
    Uint64Container cntr;
    cntr._l[0] = (qint32) qrand();
    if(num > 4)
        cntr._l[1] = (qint32) qrand();
    QByteArray arr;
    arr.append((char) num)
            .append(cntr._c, num)
            .append(value.toUtf8());
    QByteArray hash = QCryptographicHash::hash(
                arr, QCryptographicHash::Md5);
    QByteArray cypher = m_simpleCrypt->encryptToByteArray(arr);
    cntr._l[0] = cypher.length();
    hash.prepend(cypher);
    hash.prepend(cntr._c, 4);
    set(key, QString::fromLatin1(hash.toBase64()));
}

QString Settings::getEncrypted(const QString &key, QString defValue)
{
    QByteArray data = QByteArray::fromBase64(
                get<QString>(key, defValue).toLatin1());
    int length = *((qint32*) data.data());
    QByteArray decrypt = m_simpleCrypt->decryptToByteArray(
                data.mid(4, length));
    QByteArray hash = QCryptographicHash::hash(
                decrypt, QCryptographicHash::Md5);
    if(hash == data.mid(4 + length, hash.length())) {
        int skip = 1 + (int) (*(decrypt.data()));
        return QString::fromUtf8(decrypt.mid(skip, decrypt.length() - skip));
    } else {
        return defValue;
    }
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
