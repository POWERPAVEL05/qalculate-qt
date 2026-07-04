#include <QFile>
#include <QDebug>
#include <cstddef>

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
    // static int total = 0;
    // m_fname = QString("%1_%2").arg(fname).arg(total++);
    m_fname = fname;
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
        settings->tempfiles.push_back(getFilePath(true)+".jpeg");/*kill file later*/ 
        m_fileOpen = true;
    } 
}

bool texFile::generate()
{
    if(m_isGenerated || !m_fileOpen) return false;/*file not processed correctly*/
    m_file->resize(0);//avoid bad write one regeneratoin

    QTextStream out(m_file);
    out << m_preamble;
    out << "\\begin{document}$";
    out << "\\begin{aligned}\n";
    out << m_expr;

    for(auto s : m_results) out << s;


    out << "\n\\end{aligned}\n";
    out << "$\\end{document}\n";

    QDebug(QtDebugMsg) << m_expr << m_results;
    return true;
}

void texFile::appendResult(const QString &res,bool isExact)
{
    QString str {isExact ? "= " : "\\approx "};
    m_results << "\\\\ " << (isExact ? "&= " : "&\\approx") << res;
    m_isGenerated = false;//regeneration possible
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
    connect(m_proc,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(onProcDone(int,QProcess::ExitStatus)));

    for(auto i : settings->v_expression) m_files.push_back(nullptr);
}

void texManager::onProcDone(int exitCode,QProcess::ExitStatus status){
    if(exitCode > 0 || status == QProcess::ExitStatus::CrashExit) return;//crash and burn
    emit texManDoneGenerating(0,"");
}

texManager::~texManager()
{
    for(auto file : m_files)
    {
        delete file;
    }
}

QString texManager::genFileat(texFile * file,bool isNew)
{
    if(!canGenerateTex() || !file) return "";
    if(!file->generate()) return "";

    QStringList args;
    args << m_ftemp->fileName() << file->getFilePath(true) << m_currentDir->absolutePath();
    QDebug(QtDebugMsg) << args;

    m_ftemp->open();
    m_proc->start("bash",args);//script runs weird else

    // m_proc->waitForFinished();
    // QDebug(QtDebugMsg) << m_proc->exitCode() << file->getFilePath() << "\n";

    /* debug */
    if(!m_proc->waitForStarted()){
        qDebug() << "Failed to start:" << m_proc->errorString();
        return "";
    }
    // m_proc->waitForFinished();
    // qDebug() << "exit code:" << m_proc->exitCode();
    // qDebug() << "exit status:" << m_proc->exitStatus(); // NormalExit vs CrashExit
    // qDebug() << "stdout:" << m_proc->readAllStandardOutput();
    // qDebug() << "stderr:" << m_proc->readAllStandardError();

    //sync to settings

    QString jpeg_path = file->getFilePath(true)+".jpeg";
    if(isNew) settings->v_tex_files.back() = jpeg_path;//dont add already existing file

    for(auto i : settings->v_tex_files) QDebug(QtDebugMsg) << i;
    for(auto i : m_files) if(i) QDebug(QtDebugMsg) << i->getFilePath();
    return jpeg_path;
}

texFile * texManager::newFile(const QString &fname)
{
    static int total = 0;
    texFile *ret = new texFile(QString("%1_%2").arg(fname).arg(total++),m_currentDir);
    m_files.push_back(ret);
    settings->v_tex_files.push_back("");//signal no file for this slot
    return ret;
}
QString texManager::genFileat(size_t i)
{
    texFile * file = at(i);
    return genFileat(file,true);
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

void texManager::onhistoryCleared(vector<int> v){
    return;
    QDebug(QtDebugMsg) << "texMan files" << m_files.size();
    std::reverse(v.begin(),v.end());
    for(auto i : v){
        QDebug(QtDebugMsg) << i;
        if(i < (int)m_files.size()) m_files.erase(m_files.begin()+i);
    }
}
//later
void texManager::onhistoryMovedTop(int i1){
    QDebug(QtDebugMsg) << "texMan files" << m_files.size();
    return;
}
void texManager::onhistoryRemoved(int i1){

    m_proc->kill();
    QDebug(QtDebugMsg) << "rem: " << i1;
    if(i1 < (int)m_files.size()){
        m_files.erase(m_files.begin()+i1);
    }

    return;
}