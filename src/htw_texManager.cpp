#include <QFile>
#include <QDebug>
#include <cstddef>

#include "htw_texManager.h"
#include "qfiledevice.h"
#include "qglobal.h"
#include "qprocess.h"

using std::string;

texFile::texFile(const QString &fname, const QString &filepath,const QString &type ,const QString &preamble)
{
    m_filePath = filepath + "/" + fname + ".tex";
    m_genFilePath = filepath + "/" + fname + ".pdf";

    m_preamble += "\\documentclass{" + type  + "}";
    m_preamble += preamble;

    m_file = new QFile(filepath);
    if(m_file->open(QIODevice::WriteOnly)) m_fileOpen = true;
}

texFile::~texFile()
{
    if(m_file) delete m_file;
}


void texFile::addInPreamble(const QString &lines, bool package)
{
    for(QString line : lines)
    {
        if(package) line = "\\usepackage{" + line + "}";
        m_preamble += line;
    }

}

void texFile::addInMain(const QString &expr,const QString &result)
{
    m_expr = expr;
    m_result = result;
}

QString texFile::getExpr()
{
    return m_expr;
}

QString texFile::getResult()
{
    return m_result;
}

QString texFile::getFilePath()
{
    return m_filePath;
}

QString texFile::getGenPath()
{
    return m_genFilePath;
}

bool texFile::generate()
{
    if(m_isGenerated || !m_fileOpen) return false;
    QTextStream out(m_file);
    out << m_preamble << "\\begin{document}" <<  m_expr <<  "\\quad" << m_result << "\\end{document}";
    m_file->flush();
    return true;
}

string findTexProg()
{
    return "";
}

texManager::texManager()
{
    if(default_tex_path)
    {
        m_currentDir = new QDir(QString::fromStdString(DEFAULT_PATH));  
    }

    //find texprog; assume pdflatex
    if(true)
    {
        m_texProg = "pdflatex";
        m_genTex = true;
    }

    m_proc =   new QProcess;
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
    file->generate();
    QStringList args;
    args << file->getFilePath() << "-output-directory" << file->getGenPath();
    m_proc->start(m_texProg,args);
    //add here thread stuff
    // m_proc->waitForFinished();
    return file->getGenPath();
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

texFile * texManager::newFile(const QString &fname)
{
    QString name = fname; 
    QDebug(QtDebugMsg) << name;
    texFile *ret = new texFile(fname,m_currentDir->absolutePath());
    m_files.push_back(ret);
    return ret;
}