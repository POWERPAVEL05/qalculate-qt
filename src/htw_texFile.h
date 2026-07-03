
#ifndef H_HTW_TEXFILE
#define H_HTW_TEXFILE

#include "qglobal.h"
#include "qobject.h"
#include "qobjectdefs.h"
#include <cstddef>
#include <vector>
#include <string>
#include <sstream>

#include <QFile>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>

using std::vector;
using std::string;
using std::stringstream;


class texFile : QObject{
    
    Q_OBJECT

    size_t id;

    QString m_fname;
    QString m_path;
    QFile *m_file = nullptr;

    QString m_preamble;
    QString m_expr;
    QStringList m_results;

    bool m_isGenerated = false;
    bool m_fileOpen = false;
    bool m_manyResult = false;


    public:
        texFile(const QString &, QDir *,const QString & = "standalone",const QString & = {R"(\usepackage{amsmath}\usepackage{siunitx})"});
        ~texFile();

        void addInPreamble(const QString &, bool);
        void addInMain(const QString &,const QStringList &);
        void appendResult(const QString &);
        
        QString getFilePath(bool = false);
        bool generate();
};

#endif