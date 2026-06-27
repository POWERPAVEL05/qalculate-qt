
#ifndef H_HTW_TEXMANAGER
#define H_HTW_TEXMANAGER

#include "qobjectdefs.h"
#include <cstddef>
#include <vector>
#include <string>
#include <sstream>

#include <QFile>
#include <QDir>
#include <QProcess>

using std::vector;
using std::string;
using std::stringstream;


class texFile
{
    QString m_filePath = "";
    QString m_genFilePath = "";
    QFile *m_file = nullptr;

    QString m_preamble;
    QString m_expr {""};
    QString m_result {""};

    bool m_isGenerated = false;
    bool m_fileOpen = false;

    public:
        texFile(const QString &fname, const QString &filepath,const QString &type = "standalone",const QString &preamble = {R"(\usepackage{amsmath}\usepackage{siunitx})"});
        ~texFile();

        void addInPreamble(const QString &lines, bool package);
        void addInMain(const QString &expr,const QString &result);
        
        QString getExpr();
        QString getResult();
        bool generate();
};

class texManager{
    
    Q_OBJECT


    vector<texFile* > m_files;
    QDir *m_currentDir;
    QString texProg;
    QProcess * proc;
    bool genTex = false;

    public:
        texManager();
        ~texManager();

        void genFileat(size_t i);
        size_t getFileCount();
        bool canGenerateTex();
        texFile * at(size_t i);

        texFile * newFile(const QString &fname);

    signals:

        void doneGenerating(int i);
};

static const string DEFAULT_PATH = QDir::homePath().toStdString() + "texData";
static const bool default_tex_path = true;

#endif