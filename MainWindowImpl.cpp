/* Copyright © 2007, 2009 Michael Pyne <michael.pyne@kdemail.net>

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

#include <QtCore>
#include <QtGui>

#include "MainWindowImpl.h"
#include "LibrariesInfo.h"

const char *versionString = "0.9";

class MainWindowImpl::Private
{
    friend class MainWindowImpl;

    QStringList basePaths;
    QStringList envPaths;
    QTimer highlightLibrariesTimer;
    LibrariesInfo libs;
    LibrariesStatistics stats;
};

MainWindowImpl::MainWindowImpl(QWidget *parent) :
  QMainWindow(parent), m_ui(new Ui::MainWindow), m_model(0), d(new Private)
{
    m_ui->setupUi(this);
    setAcceptDrops(true);

    m_model = new QStandardItemModel(this);
    m_ui->libView->setModel(m_model);
    m_ui->libView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Used to automatically highlight libraries if the user stops typing but hasn't
    // hit enter or return.
    d->highlightLibrariesTimer.setInterval(1000);
    d->highlightLibrariesTimer.setSingleShot(true);
    connect(&d->highlightLibrariesTimer, SIGNAL(timeout()), SLOT(highlightMatchingLibraries()));
}

void MainWindowImpl::openFile(const QString &fileName)
{
    if (fileName.isEmpty())
        return;

    m_model->clear();
    QStringList list;
    list << tr("Shared Object") << tr("Resolved Path");
    m_model->setHorizontalHeaderLabels(list);

    QFileInfo fi(fileName);

    QList<QStandardItem *> items;
    items << new QStandardItem(fi.fileName());
    items << new QStandardItem(fileName);

    m_model->invisibleRootItem()->appendRow(items);
    d->libs.loadFile(fileName);
    d->stats = d->libs.getStatistics();
    addFile(fileName, m_model->item(0));
    d->libs.clear();

    m_ui->libView->expand(m_model->indexFromItem(items[0]));
    m_ui->libView->resizeColumnToContents(0);

    m_ui->statusbar->showMessage(statusBarStats());

    setWindowFilePath(fileName);
}

void MainWindowImpl::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open executable or library"));
    openFile(fileName);
}

void MainWindowImpl::on_actionQuit_triggered()
{
    emit quit();
}

void MainWindowImpl::addFile(const QString &name, QStandardItem *root)
{
    QList<QStandardItem *> items;

    QStringList libs = d->libs.getLibraryChildren(name);

    foreach(QString soname, libs) {
        QString lib = d->libs.getLibraryPath(soname);
        items << new QStandardItem(soname);

        if (lib.isEmpty()) {
            items << new QStandardItem(tr("not found"));

            QFont f = items[0]->font();
            f.setItalic(true);

            items[0]->setFont(f);
            items[1]->setFont(f);
        }
        else {
            items << new QStandardItem(lib);
        }

        root->appendRow(items);

        if(!lib.isEmpty())
        {
            addFile(soname, root->child(root->rowCount() - 1, 0));
        }

        items.clear();
    }
}

void MainWindowImpl::resetItems(QStandardItem *root)
{
    for(int i = 0; i < root->rowCount(); ++i)
        resetItems(root->child(i, 0));

    root->setForeground(Qt::black);
}

QString MainWindowImpl::statusBarStats() const
{
    double count = d->stats.depsCount;
    double bytes = d->stats.depsSizeInBytes;
    if (bytes > 1024 * 1024)
    {
        // We need at maximum 2 digits in the fractional part.
        bytes = 0.01 * (qint64(bytes) * 100 / (1024 * 1024));
        return tr("%1 dependencies with total size %2 MB.").arg(count).arg(bytes);
    }
    if (bytes > 1024)
    {
        // We need at maximum 2 digits in the fractional part.
        bytes = 0.01 * (qint64(bytes) * 100 / 1024);
        return tr("%1 dependencies with total size %2 KB.").arg(count).arg(bytes);
    }
    return tr("%1 dependencies with total size %2 bytes.").arg(count).arg(bytes);
}

void MainWindowImpl::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About elflibviewer"),
        tr("elflibviewer is a program to display the required library dependencies of"
        " a program or shared library (in ELF format).  It requires the readelf"
        " tool.\n\nCopyright © 2007, 2009 Michael Pyne.  This program may be distributed"
        " under the terms of the GNU GPL v2 (or any later version).\n\n"
        "Icons are from the KDE 4 Oxygen icon set, distributed under the "
        "LGPL, version 2."
        "\n\nVersion: %1").arg(versionString));
}

void MainWindowImpl::highlightMatchingLibraries()
{
    QString findText = m_ui->libSearchName->text();

    resetItems(m_model->invisibleRootItem());
    if(findText.isEmpty())
        return;

    QList<QStandardItem *> results = m_model->findItems(findText, Qt::MatchContains | Qt::MatchRecursive);
    foreach(QStandardItem *i, results) {
        i->setForeground(Qt::red);
        QStandardItem *item = i->parent();
        while(item) {
            item->setForeground(Qt::red);
            item = item->parent();
        }
    }
}

void MainWindowImpl::restartTimer()
{
    d->highlightLibrariesTimer.start();
}

void MainWindowImpl::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QLatin1String("text/plain")))
        event->acceptProposedAction();
}

void MainWindowImpl::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    openFile(urls.first().toLocalFile());
}

// vim: set ts=8 sw=4 et encoding=utf8:
