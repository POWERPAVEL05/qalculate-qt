
#ifndef H_HTW_TEXMANAGER
#define H_HTW_TEXMANAGER

#include "qobject.h"
#include "qobjectdefs.h"
#include <cstddef>
#include <vector>
#include <string>
#include <sstream>
#include "htw_texFile.h"
#include "qprocess.h"

#include <QFile>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>

using std::vector;
using std::string;
using std::stringstream;

class texManager : public QObject{
    
    Q_OBJECT

    QTemporaryFile *m_ftemp;
    QDir *m_currentDir;
    QString m_texProg;
    QProcess * m_proc;
    
    vector<texFile* > m_files;
    size_t m_total = 0;
    
    bool m_genTex = false;
    bool m_canGenTex = false;
    
    public:
        texManager();
        ~texManager();

        QString genFileat(size_t i);
        QString genFileat(texFile *, bool);
        size_t getFileCount();
        bool canGenerateTex();
        texFile * at(size_t i);
        texFile * newFile(const QString &fname);
        texFile * getNewestFile();

    public slots:
		void onhistoryCleared(vector<int>);
		void onhistoryMovedTop(int);
		void onhistoryRemoved(int);
        void onProcDone(int,QProcess::ExitStatus);

    signals:
        void texManDoneGenerating(int id, QString path);

};

static const bool default_tex_path = true;

#define DEFAULT_PATH (QDir::homePath().toStdString() + "/.texData/")

#define SCRIPT_GENCONV \
        "#! /usr/bin/bash\n\
        echo $1 >> ~/hallo.log\n\
        echo $2 >> ~/hallo.log\n\
        if [[ -z $1 || -z $2 ]] ; then\n\
            exit 1\n\
        fi\n\
        pdflatex -output-directory $2 \"$1.tex\" > /dev/zero\n\
        if [[ $? -ne 0 ]] ; then\n\
            exit 2\n\
        fi\n\
        pdftoppm \"$1.pdf\" -jpeg > $1.jpeg\n\
        if [[ $? -ne 0 ]] ; then\n\
            exit 3\n\
        fi\n\
        exit 0"

#endif