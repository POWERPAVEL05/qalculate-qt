#ifndef H_HTW_TEX
#define H_HTW_TEX

#include <vector>
#include <string>
#include <sstream>

using std::vector;
using std::string;
using std::stringstream;


class texDoc
{
    string m_type;
    vector<string> m_preamble;
    stringstream m_data;
    stringstream m_content;
    bool m_isGenerated;

    public:
        explicit texDoc(const string &type = "standalone",const vector<string> &preamble = {R"(\usepackage{amsmath})"})
            : m_type(type), m_preamble(preamble), m_isGenerated(false){};

        void addPackages(const vector<string> &packages);
        void addInPreamble(const vector<string> &lines);

        void addInlineMath(const string &m);
        void addAlignedMath(const vector<string> &m);
        void addSimpleText(const string &s);
        
        //todo
        void addMatrixMath(void);
        void addTimeDate(void);

        string getContent();
        void generateContent();
        int saveToFile(const string &fname);
};

#endif