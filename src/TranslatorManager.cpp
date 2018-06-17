#include <QApplication>
#include <QLocale>
#include <QDebug>
#include <QTranslator>
#include "Settings.h"
#include "TranslatorManager.h"

using namespace WalletGui;

TranslatorManager* TranslatorManager::m_Instance = 0;

TranslatorManager::TranslatorManager()
{
    QString lang = Settings::instance().getLanguage();
    if(lang.isEmpty()) {
        lang = QLocale::system().name();
    }

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");
    QDir dir(m_langPath);
    QStringList resources = dir.entryList(QStringList("??.qm"));

    for (int j = 0; j < resources.size(); j++)
    {
        QTranslator* pTranslator = new QTranslator;
        if (pTranslator->load(resources[j], m_langPath))
        {
            QString locale;
            locale = resources[j];
            locale.truncate(locale.lastIndexOf('.'));
            if (locale == lang)
            {
                qApp->installTranslator(pTranslator);
                m_keyLang = locale;
            }

            m_translators.insert(locale, pTranslator);
        }
    }
}

TranslatorManager::~TranslatorManager()
{
    TranslatorMap::const_iterator i = m_translators.begin();
    while (i != m_translators.end())
    {
        QTranslator* pTranslator = i.value();
        delete pTranslator;
        ++i;
    }

    m_translators.clear();
}

TranslatorManager* TranslatorManager::instance()
{
    static QMutex mutex;
    if (!m_Instance)
    {
        mutex.lock();

        if (!m_Instance)
            m_Instance = new TranslatorManager;

        mutex.unlock();
    }

    return m_Instance;
}

bool TranslatorManager::setTranslator(const QString &lang)
{
    bool rc = false;
    if (lang != m_keyLang && m_translators.contains(lang))
    {
        QTranslator* pTranslator = m_translators[m_keyLang];
        QCoreApplication::removeTranslator(pTranslator);
        //qApp->removeTranslator(pTranslator);
        pTranslator = m_translators[lang];
        QCoreApplication::installTranslator(pTranslator);
        //qApp->installTranslator(pTranslator);
        m_keyLang = lang;
        rc = true;
    }

    return rc;
}

void TranslatorManager::switchTranslator(QTranslator& translator, const QString& filename)
{
  // remove the old translator
  qApp->removeTranslator(&translator);

  // load the new translator
  if(translator.load(filename))
   qApp->installTranslator(&translator);
}
