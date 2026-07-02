#! /bin/usr/bash
if [[ -z $1 || -z $2 ]] ; then
	exit 1
fi

pdflatex -output-directory $2 "$1.tex" > /dev/zero

if [[ $? -ne 0 ]] ; then
	exit 2
fi

pdftoppm "$2/$1.pdf" -jpeg > $1.jpeg

if [[ $? -ne 0 ]] ; then
	exit 3
fi
