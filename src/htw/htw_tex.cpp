#include "htw_tex.h"

#include <sstream>
#include <fstream>
#include <vector>

using std::stringstream;
using std::ofstream;

void texDoc::addInlineMath(const string &m)
{
    m_data << "$\n"  <<  m  << "\n$";
}

void texDoc::addAlignedMath(const vector<string> &m)
{
    if(m.empty())
    {
        return;
    }

    m_data << "$\\begin{aligned}\n";

    for(auto line : m)
    {
        m_data << line << "\n";
    }

    m_data <<  "\n\\end{aligned}$";
}

void texDoc::addSimpleText(const string &s)
{
    m_data << s << "\n";
}

void texDoc::addPackages(const vector<string> &packages)
{
    if(packages.empty())
        return;

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

string texDoc::getContent()
{
    return m_content.str();
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

    m_content << m_data.str();

    m_content << "\\end{document}" << '\n';

    m_isGenerated = true;
}

int texDoc::saveToFile(const string &fname)
{
    if(!m_isGenerated)
        generateContent();

    ofstream file(fname);

    if(!file.is_open())
        return 1;

    file << m_content.str();

    file.close();

    return 0;
}