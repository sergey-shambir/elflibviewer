/* Copyright © 2007 Michael Pyne <michael.pyne@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef LIBRARIESINFO_H
#define LIBRARIESINFO_H

#include <QString>
#include <QHash>
#include <QStringList>
#include <QSharedPointer>

struct LibSearchInfo
{
    // Basically if runPath is set (at all), ignore rPath and search runPath
    // after LD_LIBRARY_PATH.  Otherwise, if rPath is set, search it before
    // LD_LIBRARY_PATH.  Both rPath and runPath are : separated.
    QString rPath;
    QString runPath;
};

struct LibrariesStatistics
{
    qint64 depsCount;
    qint64 depsSizeInBytes;
};

class LibrariesInfo
{
public:
    LibrariesInfo();
    void loadFile(const QString &path);
    LibrariesStatistics getStatistics() const;
    void clear();

    QString getLibraryPath(const QString &name);
    QStringList getLibraryChildren(const QString &name);

private:
    enum ELFClass {
        Unknown,
        ELF32,
        ELF64
    };

    class Lib
    {
    public:
        bool loadedChildren;
        QString path;
        QStringList children;
        static QSharedPointer<Lib> create(const QString &path);

    private:
        Lib(const QString &path)
            : loadedChildren(false)
            , path(path)
        {
        }
    };
    typedef QSharedPointer<Lib> LibPtr;

    void loadFileRecursive(const QString &path, Lib &parent);
    QString resolveLibraryPath(const QString &library,
                               const LibSearchInfo &info);
    QStringList getBasePaths() const;
    QStringList getSystemEnvPaths() const;
    QStringList readLdConfig(const QString &path) const;
    QStringList readLdConfigsByWildcard(const QString &wildcardPath) const;
    void splitPaths(const QStringList &all, QStringList &elf32, QStringList &elf64);

    QHash<QString, LibPtr> cache;
    QStringList basePaths32;
    QStringList envPaths32;
    QStringList basePaths64;
    QStringList envPaths64;
    ELFClass elfClass;
};

#endif // LIBRARIESINFO_H
