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

#include <QMainWindow>
#include <QHash>
#include "ui_elflibviewer.h"

class QStandardItem;
class QStandardItemModel;
class QStringList;

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

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

protected slots:
    void on_actionQuit_triggered();
    void on_actionOpen_triggered();
    void on_actionAbout_triggered();

private:
    void addFile(const QString &name, QStandardItem *root);
    void resetItems(QStandardItem *root);
    QString statusBarStats() const;

    Ui::MainWindow *m_ui;
    QStandardItemModel *m_model;

    class Private;
    Private *d;
};

#endif

// vim: set ts=8 sw=4 et encoding=utf8:
