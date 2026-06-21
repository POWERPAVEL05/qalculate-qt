#ifndef H_HTW_TEX
#define H_HTW_TEX

#include <vector>
#include <string>
#include <sstream>
#include <memory>

using std::vector;
using std::string;
using std::stringstream;
using std::shared_ptr;

class texEnv
{
    protected: 
        string m_name;
        stringstream m_content;

    public:
        texEnv(const string &name = ""): m_name(name) {};    

        string begin();
        string end();
        void clear();
        virtual void addContent(const string &s);
        virtual string generate() = 0;
};

/*actual envs to use*/
class mathInlineEnv : public texEnv
{
    public:
        mathInlineEnv() : texEnv(){};
        string generate();
};

class mathAlignedEnv : public texEnv
{
    public:
        mathAlignedEnv() : texEnv("aligned"){};
        void addContent(const string &s);
        void addContent(const vector<string> &vs);

        string generate();
};

typedef texEnv writerEnv;

class texDoc
{
    string m_type;
    vector<string> m_preamble;
    vector<shared_ptr<texEnv> > m_envs;
    stringstream m_content;

    public:
        explicit texDoc(const string &type = "standalone") : m_type(type), m_preamble({"amsmath"}){};

        void addPackages(const vector<string> &packages);
        void addInPreamble(const vector<string> &lines);
        int addEnv(shared_ptr<texEnv> &env);

        void generateContent();

        int saveToFile(const string &fname);
};

#endif