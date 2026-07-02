#include <QFile>
#include <QDebug>
#include <cstddef>
#include <sstream>

#include "qalculateqtsettings.h"
#include "htw_texManager.h"
#include "htw_texFile.h"
#include "qdebug.h"
#include "qfiledevice.h"
#include "qfileinfo.h"
#include "qglobal.h"
#include "qobject.h"
#include "qprocess.h"

using std::string;

texFile::texFile(const QString &fname,QDir * dir,const QString &type ,const QString &preamble)
{
    static int total = 0;
    m_fname = QString("%1_%2").arg(fname).arg(total++);
    m_path = dir->absolutePath();
    m_preamble += "\\documentclass{" + type  + "}";
    m_preamble += preamble;

    // m_file = new QFile(dir->filePath(m_fname+".tex"),this);
    m_file = new QFile(this);
    m_file->setFileName(dir->filePath(m_fname+".tex"));

    if(m_file->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        //TODO: add rubbish files
        settings->tempfiles.push_back(m_file->fileName());/*kill file later*/ 
        m_fileOpen = true;
        QDebug(QtDebugMsg) << "Opened" << m_file->fileName()<< "\n";
    } 
}

bool texFile::generate()
{
    if(m_isGenerated || !m_fileOpen) return false;/*file not processed correctly*/

    QTextStream out(m_file);
    out << m_preamble << "\\begin{document}" <<  m_expr <<  "\\quad";
    for(auto s : m_results) out << s;
    out << "\\end{document}\n";

    return true;
}

texFile::~texFile()
{
    if(m_file) delete m_file;
}

void texFile::addInPreamble(const QString &lines, bool package)
{
    m_preamble += package ? "\\usepackage{" + lines + "}" : lines;
}

void texFile::addInMain(const QString &expr,const QStringList &results)
{
    m_expr = expr;
    m_results = results;
    if(m_results.size() > 1) m_manyResult = true;
}

QString texFile::getFilePath(bool noExt)
{
    if(!m_fileOpen) return "";
    return  noExt ? m_path+"/"+m_fname : m_file->fileName();
}

texManager::texManager()
{
    if(default_tex_path)
    {
        m_currentDir = new QDir(QString::fromStdString(DEFAULT_PATH));  
        bool suc = false;
        if(!m_currentDir->exists()) suc = m_currentDir->mkpath(m_currentDir->absolutePath());

        QDebug(QtDebugMsg) << suc << QString::fromStdString(DEFAULT_PATH);
    }

    //find texprog; assume pdflatex
    if(true)
    {
        m_texProg = "pdflatex";
        m_genTex = true;
    }

    m_ftemp = new QTemporaryFile(this);

    if(!m_ftemp->open()){
        QDebug(QtDebugMsg) << "Tempfile Error\n";
    }
    m_ftemp->setAutoRemove(false);
    m_ftemp->write(SCRIPT_GENCONV);
    m_ftemp->close();

    QFile::Permissions perms = m_ftemp->permissions();
    perms |= QFileDevice::ExeOwner | QFileDevice::ExeUser | QFileDevice::ExeOther | QFileDevice::ReadOwner;
    m_ftemp->setPermissions(perms);

    m_proc =  new QProcess;
}

texManager::~texManager()
{
    for(auto file : m_files)
    {
        delete file;
    }
}

QString texManager::genFileat(texFile * file)
{
    if(!canGenerateTex() || !file) return "";
    if(!file->generate()) return "";

    QStringList args;
    args << m_ftemp->fileName() << file->getFilePath(true) << m_currentDir->absolutePath();
    QDebug(QtDebugMsg) << args;

    m_ftemp->open();
    m_proc->start("bash",args);

    m_proc->waitForFinished();
    QDebug(QtDebugMsg) << m_proc->exitCode() << file->getFilePath() << "\n";

    // if(!m_proc->waitForStarted()){
    //     qDebug() << "Failed to start:" << m_proc->errorString();
    //     return "";
    // }
    // m_proc->waitForFinished();
    // qDebug() << "exit code:" << m_proc->exitCode();
    // qDebug() << "exit status:" << m_proc->exitStatus(); // NormalExit vs CrashExit
    // qDebug() << "stdout:" << m_proc->readAllStandardOutput();
    // qDebug() << "stderr:" << m_proc->readAllStandardError();

    // m_ftemp->open();
    // QTextStream in(m_ftemp);
    // QDebug(QtDebugMsg) << m_ftemp->size() << in.readAll();

    QDebug(QtDebugMsg) << m_ftemp->fileName();

    return file->getFilePath(true)+".jpeg";
}

texFile * texManager::newFile(const QString &fname)
{
    texFile *ret = new texFile(fname,m_currentDir);
    m_files.push_back(ret);
    return ret;
}
QString texManager::genFileat(size_t i)
{
    texFile * file = at(i);
    return genFileat(file);
}

size_t texManager::getFileCount()
{
    return m_files.size();
}

bool texManager::canGenerateTex()
{
    return m_genTex;
}

texFile * texManager::at(size_t i)
{
   texFile *ret;
    try{
        ret  = m_files.at(i);
    }
    catch(const std::out_of_range& ex){
        ret = nullptr;
    }
    return ret;
}