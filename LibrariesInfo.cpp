#include "LibrariesInfo.h"
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>
#include <QDir>

LibrariesInfo::LibrariesInfo()
    : elfClass(Unknown)
{
    splitPaths(getBasePaths(), basePaths32, basePaths64);
    splitPaths(getSystemEnvPaths(), envPaths32, envPaths64);
}

void LibrariesInfo::loadFile(const QString &path)
{
    LibPtr executable = Lib::create(path);
    cache[path] = executable;
    loadFileRecursive(path, *executable);
}

LibrariesStatistics LibrariesInfo::getStatistics() const
{
    LibrariesStatistics ret;
    ret.depsCount = cache.size();
    ret.depsSizeInBytes = 0;
    foreach (const QString &key, cache.keys()) {
        LibPtr lib = cache.value(key);
        if (!lib.isNull()) {
            QFileInfo fi(lib->path);
            ret.depsSizeInBytes += fi.size();
        }
    }
    return ret;
}

void LibrariesInfo::clear()
{
    elfClass = Unknown;
    cache.clear();
}

QString LibrariesInfo::getLibraryPath(const QString &name)
{
    LibPtr lib = cache.value(name);
    if (!lib.isNull())
        return lib->path;
    return QString();
}

QStringList LibrariesInfo::getLibraryChildren(const QString &name)
{
    LibPtr lib = cache.value(name);
    if (!lib.isNull())
        return lib->children;
    return QStringList();
}

QString LibrariesInfo::resolveLibraryPath(const QString &library, const LibSearchInfo &info)
{
    if (cache.contains(library))
        return cache[library]->path;

    // Create search path
    QStringList paths;

    // RPATH was set, RUNPATH was not, look in RPATH first.
    if(info.runPath.isEmpty() && !info.rPath.isEmpty())
        paths << info.rPath.split(":");

    if (elfClass == ELF64)
        paths << envPaths64;
    else
        paths << envPaths32;

    if(!info.runPath.isEmpty())
        paths << info.runPath.split(":");

    if (elfClass == ELF64)
        paths << basePaths64;
    else
        paths << basePaths32;
    paths << "/lib" << "/usr/lib";

    foreach(QString path, paths) {
        QFileInfo qfi(path + "/" + library);
        if (qfi.exists()) {
            // Resolve symlink.
            cache[library] = Lib::create(qfi.canonicalFilePath());

            return qfi.canonicalFilePath();
        }
    }

    return QString();
}

void LibrariesInfo::loadFileRecursive(const QString &path, Lib &parent)
{
    QProcess readelf;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QLatin1String("LC_ALL"), QLatin1String("en_US"));
    readelf.setProcessEnvironment(env);
    readelf.start("readelf", QStringList() << "-h" << "-d" << path);
    if (!readelf.waitForFinished())
    {
        return;
    }

    QTextStream ts(&readelf);
    QRegExp soPat("\\[(.*)\\]$");
    QRegExp classElf64("\\s*Class\\:\\s+ELF64\\s*");
    QRegExp classElfAny("\\s*Class\\:\\s+ELF\\w*\\s*");
    LibSearchInfo info;

    // List of libraries to search for.  We cannot search as soon as its
    // encountered because we must wait until we've seen rPath and runPath.
    QStringList libs;
    while(!ts.atEnd()) {
        QString str = ts.readLine();

        if (elfClass == Unknown) {
            if (classElf64.exactMatch(str))
                elfClass = ELF64;
            else if (classElfAny.exactMatch(str))
                elfClass = ELF32;
        }
        if(str.contains("(RPATH)") && soPat.indexIn(str) != -1) {
            info.rPath = soPat.cap(1);
        } else if(str.contains("(RUNPATH)") && soPat.indexIn(str) != -1) {
            info.runPath = soPat.cap(1);
        } else if(str.contains("(NEEDED)") && soPat.indexIn(str) != -1) {
            libs << soPat.cap(1);
        }
    }
    parent.children = libs;
    parent.loadedChildren = true;

    foreach (const QString &soname, libs) {
        QString sopath = resolveLibraryPath(soname, info);
        if (!sopath.isEmpty())
        {
            LibPtr lib = cache[soname];
            if (lib && !lib->loadedChildren)
            {
                loadFileRecursive(sopath, *lib);
            }
        }
    }
}

QStringList LibrariesInfo::getBasePaths() const
{
    QStringList basePaths = readLdConfig(QLatin1String("/etc/ld.so.conf"));
    basePaths.removeDuplicates();
    return basePaths;
}

/// Reads /etc/ld.so.conf or included file
QStringList LibrariesInfo::readLdConfig(const QString &path) const
{
    // Open system file defining standard library paths.
    QFile ldConf(path);

    if(!ldConf.open(QIODevice::ReadOnly))
        return QStringList();

    QTextStream in(&ldConf);
    QStringList paths;

    // Loop through each path.  Resolve it to the actual file, and if not
    // already in the path, add it at the end.
    while(!in.atEnd()) {
        QString lib = in.readLine();
        if (lib.isEmpty())
            continue;
        if (lib.startsWith(QLatin1String("include "))) {
            QStringList parts = lib.split(QLatin1Char(' '), QString::SkipEmptyParts);
            if (parts.size() > 1)
                paths << readLdConfigsByWildcard(parts[1]);
        } else {
            QFileInfo libInfo(lib);
            if (!libInfo.exists())
                continue;
            QString realPath = libInfo.canonicalFilePath();

            paths << realPath;
        }
    }

    return paths;
}

/// Reads all ld.so.conf-like files matching wildcard
/// Example - /etc/ld.so.conf.d/*.conf
QStringList LibrariesInfo::readLdConfigsByWildcard(const QString &wildcardPath) const
{
    int pos = wildcardPath.indexOf(QLatin1Char('*'));
    if (pos == -1)
        return readLdConfig(wildcardPath);

    const QString dirPath = wildcardPath.left(pos);
    const QString namesFilter = wildcardPath.right(wildcardPath.size() - pos);
    QDir dir(dirPath);
    if (!dir.exists())
        return QStringList();

    dir.setNameFilters(QStringList() << namesFilter);
    QStringList files = dir.entryList(QDir::Files);

    QStringList paths;
    foreach (const QString &configFile, files)
        paths << readLdConfig(dirPath + configFile);

    return paths;
}

void LibrariesInfo::splitPaths(const QStringList &all, QStringList &elf32, QStringList &elf64)
{
    foreach (const QString &path, all)
    {
        if (path.contains("i386") || path.contains("i686")
                || path.endsWith("lib32") || path.endsWith("libx32")) {
            elf32 << path;
        } else if (path.contains("x86_64") || path.endsWith("lib64")) {
            elf64 << path;
        } else {
            elf32 << path;
            elf64 << path;
        }
    }
}

QStringList LibrariesInfo::getSystemEnvPaths() const
{
    QStringList envPath = QProcess::systemEnvironment();
    foreach(QString s, envPath) {
        if(s.startsWith("LD_LIBRARY_PATH=")) {
            s.remove("LD_LIBRARY_PATH=");
            return s.split(":");
        }
    }

    return QStringList();
}

QSharedPointer<LibrariesInfo::Lib> LibrariesInfo::Lib::create(const QString &path)
{
    return QSharedPointer<LibrariesInfo::Lib>(new Lib(path));
}
