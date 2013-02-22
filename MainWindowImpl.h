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

#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QtGui/QMainWindow>
#include <QtCore/QHash>
#include "ui_elflibviewer.h"

class QStandardItem;
class QStandardItemModel;
class QStringList;

typedef QHash<QString,QString> LibHash;

struct LibSearchInfo
{
    // Basically if runPath is set (at all), ignore rPath and search runPath
    // after LD_LIBRARY_PATH.  Otherwise, if rPath is set, search it before
    // LD_LIBRARY_PATH.  Both rPath and runPath are : separated.
    QString rPath;
    QString runPath;
};

class MainWindowImpl : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindowImpl (QWidget *parent);
    void openFile(const QString &fileName);

    public slots:
    void highlightMatchingLibraries();
    void restartTimer();

    signals:
    void quit();

    protected slots:
    void on_actionQuit_triggered();
    void on_actionOpen_triggered();
    void on_actionAbout_triggered();

    private:
    void addFile(const QString &fileName, QStandardItem *root);
    QString resolveLibrary(const QString &library, const LibSearchInfo &searchInfo);
    void resetItems(QStandardItem *root);
    QStringList getBasePaths() const;
    QStringList getSystemEnvPaths() const;

    QStringList readLdConfig(const QString &path) const;
    QStringList readLdConfigsByWildcard(const QString &wildcardPath) const;

    Ui::MainWindow *m_ui;
    QStandardItemModel *m_model;
    LibHash m_libs;

    class Private;
    Private *d;
};

#endif

// vim: set ts=8 sw=4 et encoding=utf8:
