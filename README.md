ELF Library Viewer purpose
============
This program shows a list of libraries that a given application or library
depends on.  This is done recursively as well, so instead of the flat list given by tools
like ldd(1), you can tell what libraries are depended upon directly, and indirectly.
You could use this to determine what library is causing link errors for your application,
for instance.

Status
============
It's patched version of original app: http://www.purinchu.net/software/elflibviewer.php

Improvements:
* Better usability (added drag&drop support, altered table rows, etc);
* Added Russian translation;
* Added information about dependencies count and size in bytes to status bar;
* Fixes for recent Ubuntu/Debian (with multiarch mechanism);
* Fixed libraries caching bug: library dependencies hadn't been loaded if library was added to tree somewhere else before;
* Fixed paths resolving bug: 32-bit search paths had been used to resolve 64-bit executable dependencies;
* Fixed resolving libraries with non-latin paths.

How to build
============
```
  $ git clone git@github.com:sergey-shambir/elflibviewer.git
  $ cd elflibviewer
  $ mkdir elflibviewer-build && cd elflibviewer-build
  $ qmake-qt4 ../elflibviewer/elflibviewer.pro && make
```

Usage
============

![app screenshot][screenshot]

[screenshot]: https://github.com/sergey-shambir/elflibviewer/raw/master/github/elflibviewer-screnshot.png "Ubuntu, 1 Sep 2013, sample cocos2dx game opened in ElfLibViewer"

You can open a file to view after running the program, or drag-and-drop it, or pass the file name on
the command line. When the program is open, you can type into the search line to highlight
libraries with matching names, and any other libraries depending on that one.

How to make yet one translation
============
1. Fork this repository and clone it to your computer
2. Go to `efllibviewer/translations` directory and copy `i18n_ru.ts`, change `ru` to your language code
3. Open `efllibviewer/efllibviewer.pro` in QtCreator and add your translation to `TRANSLATIONS` variable
4. In QtCreator select menu action `Tools->External->Linguist->Update Translations (lupdate)`
5. Open `i18n_**.ts` in QtLinguist and translate all strings.
6. In QtCreator select menu action `Tools->External->Linguist->Release Translations (lrelease)`
7. Commit changes, push to fork and make pull request
