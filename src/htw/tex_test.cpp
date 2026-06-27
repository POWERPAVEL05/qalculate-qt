#include "htw_tex.h"

int main()
{
    texDoc d1;

    d1.addInlineMath("a + b = c");

    d1.saveToFile("output.tex");
}