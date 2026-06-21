#include "htw_tex.h"

#include <algorithm>
#include <sstream>
#include <vector>

using std::stringstream;

string texEnv::begin()
{
    stringstream sst;
    sst << R"(\begin{)" << m_name << "}";
    return sst.str();
}

string texEnv::end()
{
    stringstream sst;
    sst << R"(\end{)" << m_name << "}";
    return sst.str();
}

void texEnv::clear()
{
    m_content.str(std::string());
}

string texEnv::generate()
{
    return m_content.str();
}

void texEnv::addContent(const string &s)
{
    m_content << s;
}

string mathInlineEnv::generate()
{
    stringstream sst;
    sst << "$\n"  << m_content.str()  << "\n$";
    return sst.str();
}

void mathAlignedEnv::addContent(const string &s)
{
    m_content << s << '\n';
}

//this sorta has to change
string mathAlignedEnv::generate()
{
    stringstream sst;
    sst << "$" << begin() <<  m_content.str() << end() << "$";
    return sst.str();
}

void texDoc::addPackages(const vector<string> &packages)
{
    stringstream sst;
    for(auto package : packages)
    {
        sst << R"(\usepackage{)" << package << "}\n";
        m_preamble.emplace_back(sst.str());
        sst.str("");
    }
}


void texDoc::addInPreamble(const vector<string> &lines)
{
    for(auto line : lines)
    {
        m_preamble.emplace_back(line);
    }
}

int texDoc::addEnv(shared_ptr<texEnv> &env)
{
    if(env == nullptr)
        return 1;

    m_envs.push_back(env);
    return 0;
}

void texDoc::generateContent()
{
    m_content.str("");

    m_content << R"(\documentclass{)" << m_type << "}\n";

    for(auto line : m_preamble)
    {
        m_content << line << '\n';
    }

    m_content << "\\begin{document}" << '\n';

    for(auto env : m_envs)
    {
        m_content << env->generate() << '\n';
    }

    m_content << "\\end{document}" << '\n';

}

//todo
int texDoc::saveToFile(const string &fname)
{
   return 0;
}