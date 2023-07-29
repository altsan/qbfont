/******************************************************************************
** mainwindow.h
**
**  Copyright (C) 2022 Alexander Taylor
**
**  This program is free software; you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation; either version 2.1 of the License, or (at
**  your option) any later version.
**
**  This program is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
**  License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this library; if not, write to the Free Software Foundation,
**  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
******************************************************************************/
#ifndef QBFONT_MAINWINDOW_H
#define QBFONT_MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>

#include "glypheditor.h"
#include "glyphstatus.h"
#include "qbf_const.h"


class QAction;
class QActionGroup;
class QLabel;
class QSplitter;


class FontEditor : public QMainWindow
{
    Q_OBJECT

public:
    FontEditor();
    ~FontEditor();

    bool loadFile( const QString &fileName, bool createIfNew );
    void showUsage();

protected:
    void closeEvent( QCloseEvent *event );
//  void dragEnterEvent( QDragEnterEvent *event );
//  void dropEvent( QDropEvent *event );

private slots:
    void newFile();
    void open();

    bool save();
    bool saveAs();

    void about();
    void showGeneralHelp();
    void showKeysHelp();
    void openRecentFile();
    void clearRecentFiles();
/*
    void updateStatusBar();
*/
    void updatePosition( const QPoint &newPos );
    void updateModified();
    void updateModified( bool isModified );

    void setSelect();
    void setSelectAll();
    void setDeselect();

    void clearGlyph();
    void flipGlyphX();
    void flipGlyphY();


private:
    // Setup methods
    void createActions();
    void createMenus();
    void createStatusBar();
    void createHelp();
    void readSettings();
    void writeSettings();

    // Action methods
    bool okToContinue();
    bool saveFile( const QString &fileName );

    // Misc methods
    void setCurrentFile( const QString &fileName );
    void updateRecentFileActions();
    void showMessage( const QString &message );
    void launchAssistant( const QString &panel );

    // GUI objects
    QSplitter *splitter;
    QFrame *rightPanel;
    GlyphStatus *infoBar;
    GlyphEditor *editor;

    QLabel *messagesLabel;
    QLabel *modifiedLabel;

    // Menus
    enum { MaxRecentFiles = 5 };

    QMenu   *fileMenu;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *recentFileActions[ MaxRecentFiles ];
    QAction *clearRecentAction;
    QAction *separatorAction;
    QAction *exitAction;

    QMenu   *editMenu;
    QAction *revertAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *selectAction;
    QAction *selectAllAction;
    QAction *deselectAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *pasteMaskAction;

    QMenu   *glyphMenu;
    QAction *flipXAction;
    QAction *flipYAction;
    QAction *clearAction;
    QAction *compareAction;

    QMenu   *columnMenu;
    QAction *insertColumnAction;
    QAction *addColumnAction;
    QAction *deleteColumnAction;
    QAction *removeColumnAction;

    QMenu   *rowMenu;
    QAction *insertRowAction;
    QAction *deleteRowAction;

    QMenu   *widthMenu;
    QAction *widenLeftAction;
    QAction *widenRightAction;
    QAction *widenBothAction;
    QAction *narrowLeftAction;
    QAction *narrowRightAction;
    QAction *narrowBothAction;

    QMenu   *shiftMenu;
    QAction *shiftUpAction;
    QAction *shiftDownAction;
    QAction *shiftLeftAction;
    QAction *shiftRightAction;

    QMenu   *helpMenu;
    QAction *helpGeneralAction;
    QAction *helpKeysAction;
    QAction *aboutAction;

    // Other class variables
    QStringList recentFiles;
    QString     currentFile;
    QString     currentDir;
    QDateTime   currentModifyTime;

    // Program help (platform specific implementation)
    void *helpInstance;

    // QtAssistant process
//    QProcess *helpProcess;

};

#endif  // QBFONT_MAINWINDOW_H

