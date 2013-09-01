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

class LibrariesInfo
{
public:
    LibrariesInfo();
    void loadFile(const QString &path);
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
