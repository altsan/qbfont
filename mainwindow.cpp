/******************************************************************************
** mainwindow.cpp
**
**  Copyright (C) 2023 Alexander Taylor
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

#include <QtGui>

#include "os2native.h"
#include "mainwindow.h"


// ---------------------------------------------------------------------------
// PUBLIC CONSTRUCTOR
//

FontEditor::FontEditor()
{
    QVBoxLayout *vLayout = new QVBoxLayout();

    rightPanel = new QFrame();
    rightPanel->setLayout( vLayout );

    infoBar = new GlyphStatus();
    vLayout->addWidget( infoBar );

    editor = new GlyphEditor();
    vLayout->addWidget( editor );

    vLayout->setStretchFactor( infoBar, 0 );
    vLayout->setStretchFactor( editor, 1 );
    vLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setSpacing( 3 );

    splitter = new QSplitter( Qt::Horizontal );
    splitter->addWidget( rightPanel );

    setCentralWidget( splitter );

    createActions();
    createMenus();
    createStatusBar();

//    setAcceptDrops( true );
    connect( editor, SIGNAL( positionChanged( const QPoint & )),
             this, SLOT( updatePosition( const QPoint & )));
//    connect( editor, SIGNAL( contentsChanged() ), this, SLOT( updateModified() ));

//    setMinimumWidth( statusBar()->minimumWidth() + 20 );
    setWindowTitle( tr("Font Editor") );

/*
    // Qt4 on OS/2 doesn't render PNGs well, so leave it with a native icon; otherwise...
#ifndef __OS2__
    QIcon icon;
    icon.addFile(":/images/app_16.png", QSize( 16, 16 ), QIcon::Normal, QIcon::On );
    icon.addFile(":/images/app_20.png", QSize( 20, 20 ), QIcon::Normal, QIcon::On );
    icon.addFile(":/images/app_32.png", QSize( 32, 32 ), QIcon::Normal, QIcon::On );
    icon.addFile(":/images/app_40.png", QSize( 40, 40 ), QIcon::Normal, QIcon::On );
    icon.addFile(":/images/app_64.png", QSize( 64, 64 ), QIcon::Normal, QIcon::On );
    icon.addFile(":/images/app_80.png", QSize( 80, 80 ), QIcon::Normal, QIcon::On );
    setWindowIcon( icon );
#endif
*/
    helpInstance = NULL;
    createHelp();

    currentDir = QDir::currentPath();
    setCurrentFile("");
}



// ---------------------------------------------------------------------------
// DESTRUCTOR
//
FontEditor::~FontEditor()
{
#ifdef __OS2__
    if ( helpInstance ) OS2Native::destroyNativeHelp( helpInstance );
#endif
}


// ---------------------------------------------------------------------------
// OVERRIDDEN EVENTS
//

void FontEditor::closeEvent( QCloseEvent *event )
{
    if ( okToContinue() ) {
        writeSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
}


// ---------------------------------------------------------------------------
// SLOTS
//

void FontEditor::newFile()
{
}


void FontEditor::open()
{
}


void FontEditor::clearGlyph()
{
    editor->clear();
}


void FontEditor::flipGlyphX()
{
    editor->mirror( Qt::Horizontal );
}


void FontEditor::flipGlyphY()
{
    editor->mirror( Qt::Vertical );
}


void FontEditor::insertColumn()
{
    // TODO prompt the user to select both position and direction
}


void FontEditor::addColumn()
{
    // TODO prompt the user to select both position and direction
}


void FontEditor::shiftLeft()
{
    editor->insertColumnShiftLeft( editor->increment()-1 );
}


void FontEditor::shiftRight()
{
    editor->insertColumnShiftRight( 0 );
}


void FontEditor::shiftUp()
{
    editor->insertRowUp( editor->glyphImage().height()-1 );
}


void FontEditor::shiftDown()
{
    editor->insertRowDown( 0 );
}


void FontEditor::widenLeft()
{
    /* To add a new column on the left, this call widens the image on the
     * right, then inserts a new column at position 0 (shifting everything
     * one pixel over).
     */
    editor->insertColumnShiftRight( 0, true );
}


void FontEditor::widenRight()
{
    /* To add a new column on the right, this call widens the image on the
     * right and fills the new column with blank pixels.
     */
    editor->insertColumnShiftRight( editor->increment(), true );
}


void FontEditor::widenBoth()
{
    editor->widenLeftAndRight();
}


void FontEditor::about()
{
    QMessageBox::about( this,
                        tr("Product Information"),
                        tr("<b>QBFont - Bitmap Font Editor</b><br>Version %1<hr>"
                           "Copyright &copy;2023 Alexander Taylor"
                           "<p>Licensed under the GNU General Public License "
                           "version 3.0&nbsp;<br>"
                           "<a href=\"https://www.gnu.org/licenses/gpl.html\">"
                           "https://www.gnu.org/licenses/gpl.html</a>"
                           "<br></p>").arg( PROGRAM_VERSION )
                      );
}


void FontEditor::showGeneralHelp()
{
#ifdef __OS2__
    OS2Native::showHelpPanel( helpInstance, HELP_PANEL_GENERAL );
//#else
//    launchAssistant( HELP_HTML_GENERAL );
#endif
}


void FontEditor::showKeysHelp()
{
#ifdef __OS2__
    OS2Native::showHelpPanel( helpInstance, HELP_PANEL_KEYS );
//#else
//    launchAssistant( HELP_HTML_KEYS );
#endif
}


void FontEditor::openRecentFile()
{
/*
    if ( okToContinue() ) {
        QAction *action = qobject_cast<QAction *>( sender() );
        if ( action ) {
            loadFile( action->data().toString(), false );
        }
    }
*/
}


void FontEditor::clearRecentFiles()
{
    int r = QMessageBox::question( this,
                                   tr("Clear List?"),
                                   tr("Clear the list of recent files?"),
                                   QMessageBox::Yes | QMessageBox::No
                                 );
    if ( r == QMessageBox::Yes ) {
        recentFiles.clear();
        updateRecentFileActions();
    }
}



// ---------------------------------------------------------------------------
//

void FontEditor::createActions()
{

    // File menu actions
    newAction = new QAction( tr("&New"), this );
    newAction->setShortcut( QKeySequence::New );
    newAction->setStatusTip( tr("Create a new file") );
    connect( newAction, SIGNAL( triggered() ), this, SLOT( newFile() ));

    openAction = new QAction( tr("&Open..."), this );
    openAction->setShortcut( QKeySequence::Open );
    openAction->setStatusTip( tr("Open a file") );
    connect( openAction, SIGNAL( triggered() ), this, SLOT( open() ));

    saveAction = new QAction( tr("&Save"), this );
#ifdef __OS2__
    saveAction->setShortcut( tr("F2"));
#else
    saveAction->setShortcut( QKeySequence::Save );
#endif
    saveAction->setStatusTip( tr("Save the current file") );
    connect( saveAction, SIGNAL( triggered() ), this, SLOT( save() ));

    saveAsAction = new QAction( tr("Save &as..."), this );
    saveAsAction->setShortcut( QKeySequence::SaveAs );
    saveAsAction->setStatusTip( tr("Save the current file under a new name") );
    connect( saveAsAction, SIGNAL( triggered() ), this, SLOT( saveAs() ));

    for ( int i = 0; i < MaxRecentFiles; i++ )
    {
        recentFileActions[ i ] = new QAction( this );
        recentFileActions[ i ]->setVisible( false );
        connect( recentFileActions[ i ], SIGNAL( triggered() ), this, SLOT( openRecentFile() ));
    }

    clearRecentAction = new QAction( tr("&Clear list"), this );
    clearRecentAction->setVisible( false );
    clearRecentAction->setStatusTip( tr("Clear the list of recent files") );
    connect( clearRecentAction, SIGNAL( triggered() ), this, SLOT( clearRecentFiles() ));

    exitAction = new QAction( tr("E&xit"), this );
    exitAction->setShortcut( tr("F3") );
    exitAction->setStatusTip( tr("Exit the program") );
    connect( exitAction, SIGNAL( triggered() ), this, SLOT( close() ));


    // Edit menu actions

    undoAction = new QAction( tr("&Undo"), this );
    redoAction = new QAction( tr("&Redo"), this );
    revertAction = new QAction( tr("Re&vert"), this );

    selectAction = new QAction( tr("&Select..."), this );
    selectAction->setStatusTip( tr("Activate selection mode") );
    selectAction->setCheckable( true );
    connect( selectAction, SIGNAL( triggered() ), this, SLOT( setSelect() ));

    selectAllAction = new QAction( tr("Select &all"), this );
    connect( selectAllAction, SIGNAL( triggered() ), this, SLOT( setSelectAll() ));

    deselectAction = new QAction( tr("&Deselect"), this );
    connect( deselectAction, SIGNAL( triggered() ), this, SLOT( setDeselect() ));

    cutAction = new QAction( tr("&Cut"), this );
    copyAction = new QAction( tr("C&opy"), this );
    pasteAction = new QAction( tr("&Paste"), this );
    pasteMaskAction = new QAction( tr("Paste as &mask"), this );

    clearAction = new QAction( tr("&Clear"), this );
    clearAction->setStatusTip( tr("Clear the current glyph") );
    connect( clearAction, SIGNAL( triggered() ), this, SLOT( clearGlyph() ));

    // Glyph menu actions

    // Column actions
    insertColumnAction = new QAction( tr("&Insert..."), this );
    insertColumnAction->setStatusTip( tr("Insert an empty column without changing the increment") );

    addColumnAction = new QAction( tr("Insert and &widen..."), this );
    addColumnAction->setStatusTip( tr("Insert an empty column, increasing the increment by one") );

    deleteColumnAction = new QAction( tr("&Delete..."), this );
    deleteColumnAction->setStatusTip( tr("Delete a column without changing the increment") );

    removeColumnAction = new QAction( tr("Delete and &narrow..."), this );
    removeColumnAction->setStatusTip( tr("Delete a column and reduce the increment by one") );

    // Row actions
    insertRowAction = new QAction( tr("&Insert..."), this );
    insertRowAction->setStatusTip( tr("Insert an empty row") );

    deleteRowAction = new QAction( tr("&Delete..."), this );
    deleteRowAction->setStatusTip( tr("Delete a row") );

    // Width actions
    widenLeftAction = new QAction( tr("Wider &left"), this );
    connect( widenLeftAction, SIGNAL( triggered() ), this, SLOT( widenLeft() ));

    widenRightAction = new QAction( tr("Wider &right"), this );
    connect( widenRightAction, SIGNAL( triggered() ), this, SLOT( widenRight() ));

    widenBothAction = new QAction( tr("&Wider &both"), this );
    connect( widenBothAction, SIGNAL( triggered() ), this, SLOT( widenBoth() ));

    narrowLeftAction = new QAction( tr("&Narrower left"), this );
    narrowRightAction = new QAction( tr("Narr&ower right"), this );
    narrowBothAction = new QAction( tr("&Narro&wer both"), this );

    shiftUpAction = new QAction( tr("&Up"), this );
    shiftUpAction->setShortcut( QKeySequence( Qt::Key_Up | Qt::SHIFT ));
    shiftUpAction->setStatusTip( tr("Shift all pixels up by one") );
    connect( shiftUpAction, SIGNAL( triggered() ), this, SLOT( shiftUp() ));

    shiftDownAction = new QAction( tr("&Down"), this );
    shiftDownAction->setShortcut( QKeySequence( Qt::Key_Down | Qt::SHIFT ));
    shiftDownAction->setStatusTip( tr("Shift all pixels down by one") );
    connect( shiftDownAction, SIGNAL( triggered() ), this, SLOT( shiftDown() ));

    shiftLeftAction = new QAction( tr("&Left"), this );
    shiftLeftAction->setShortcut( QKeySequence( Qt::Key_Left | Qt::SHIFT ));
    shiftLeftAction->setStatusTip( tr("Shift all pixels left by one") );
    connect( shiftLeftAction, SIGNAL( triggered() ), this, SLOT( shiftLeft() ));

    shiftRightAction = new QAction( tr("&Right"), this );
    shiftRightAction->setShortcut( QKeySequence( Qt::Key_Right | Qt::SHIFT ));
    shiftRightAction->setStatusTip( tr("Shift all pixels right by one") );
    connect( shiftRightAction, SIGNAL( triggered() ), this, SLOT( shiftRight() ));

    flipXAction = new QAction( tr("Flip &horizontally"), this );
    flipXAction->setStatusTip( tr("Flip (i.e. mirror) the glyph horizontally") );
    connect( flipXAction, SIGNAL( triggered() ), this, SLOT( flipGlyphX() ));

    flipYAction = new QAction( tr("Flip &vertically"), this );
    flipYAction->setStatusTip( tr("Flip (i.e. mirror) the glyph vertically") );
    connect( flipYAction, SIGNAL( triggered() ), this, SLOT( flipGlyphY() ));

    aboutAction = new QAction( tr("&Product information"), this );
    aboutAction->setStatusTip( tr("Show product information") );
    connect( aboutAction, SIGNAL( triggered() ), this, SLOT( about() ));

    compareAction = new QAction( tr("&Compare..."), this );
}


void FontEditor::createMenus()
{
    fileMenu = menuBar()->addMenu( tr("&File"));
    fileMenu->addAction( newAction );
    fileMenu->addAction( openAction );
    fileMenu->addAction( saveAction );
    fileMenu->addAction( saveAsAction );
    fileMenu->addSeparator();
    for ( int i = 0; i < MaxRecentFiles; i++ )
        fileMenu->addAction( recentFileActions[ i ] );
    fileMenu->addAction( clearRecentAction );
    fileMenu->addSeparator();
    fileMenu->addAction( exitAction );

    editMenu = menuBar()->addMenu( tr("&Edit"));
    editMenu->addAction( undoAction );
    editMenu->addAction( redoAction );
    editMenu->addAction( revertAction );
    editMenu->addSeparator();
    editMenu->addAction( selectAction );
    editMenu->addAction( selectAllAction );
    editMenu->addAction( deselectAction );
    editMenu->addSeparator();
    editMenu->addAction( cutAction );
    editMenu->addAction( copyAction );
    editMenu->addAction( pasteAction );
    editMenu->addAction( pasteMaskAction );
    editMenu->addSeparator();
    editMenu->addAction( clearAction );

    glyphMenu = menuBar()->addMenu( tr("&Glyph"));
    columnMenu = glyphMenu->addMenu( tr("&Column"));
    columnMenu->addAction( insertColumnAction );
    columnMenu->addAction( addColumnAction );
    columnMenu->addAction( deleteColumnAction );
    columnMenu->addAction( removeColumnAction );

    rowMenu = glyphMenu->addMenu( tr("&Row"));
    rowMenu->addAction( insertRowAction );
    rowMenu->addAction( deleteRowAction );

    widthMenu = glyphMenu->addMenu( tr("&Width"));
    widthMenu->addAction( narrowLeftAction );
    widthMenu->addAction( narrowRightAction );
    widthMenu->addAction( narrowBothAction );
    widthMenu->addAction( widenLeftAction );
    widthMenu->addAction( widenRightAction );
    widthMenu->addAction( widenBothAction );

    shiftMenu = glyphMenu->addMenu( tr("&Shift"));
    shiftMenu->addAction( shiftUpAction );
    shiftMenu->addAction( shiftDownAction );
    shiftMenu->addAction( shiftLeftAction );
    shiftMenu->addAction( shiftRightAction );

    glyphMenu->addSeparator();
    glyphMenu->addAction( flipXAction );
    glyphMenu->addAction( flipYAction );
    glyphMenu->addSeparator();
    glyphMenu->addAction( compareAction );

    menuBar()->addSeparator();
    helpMenu = menuBar()->addMenu( tr("&Help"));
//    helpMenu->addAction( helpGeneralAction );
//    helpMenu->addAction( helpKeysAction );
//    helpMenu->addSeparator();
    helpMenu->addAction( aboutAction );

}


void FontEditor::createStatusBar()
{
    messagesLabel = new QLabel("                                       ", this );
    messagesLabel->setIndent( 3 );
    messagesLabel->setMinimumSize( messagesLabel->sizeHint() );

    modifiedLabel = new QLabel(" Modified ", this );
    modifiedLabel->setAlignment( Qt::AlignHCenter );
    modifiedLabel->setMinimumSize( modifiedLabel->sizeHint() );

    statusBar()->addWidget( messagesLabel, 1 );
    statusBar()->addWidget( modifiedLabel );
    statusBar()->setMinimumSize( statusBar()->sizeHint() );

    messagesLabel->setForegroundRole( QPalette::ButtonText );
    modifiedLabel->setForegroundRole( QPalette::ButtonText );

//    updateStatusBar();
}


void FontEditor::createHelp()
{
#ifdef __OS2__
    helpInstance = OS2Native::setNativeHelp( this, QString("qfonted"), tr("QFontEd Help") );
#else
    helpProcess = new QProcess( this );
#endif
}



void FontEditor::updateRecentFileActions()
{
/*
    QMutableStringListIterator i( recentFiles );
    int fileCount = 0;
    while ( i.hasNext() ) {
        fileCount++;
        if ( !QFile::exists( i.next() ) || ( fileCount > MaxRecentFiles )) {
            i.remove();
        }
    }
    for ( int j = 0; j < MaxRecentFiles; j++ ) {
        if ( j < recentFiles.count() ) {
            QString text = tr("&%1 %2").arg( j+1 ).arg( QFileInfo( recentFiles[j] ).fileName() );
            recentFileActions[ j ]->setText( text );
            recentFileActions[ j ]->setData( recentFiles[ j ] );
            recentFileActions[ j ]->setStatusTip( QDir::toNativeSeparators( recentFiles[ j ] ));
            recentFileActions[ j ]->setVisible( true );
        }
        else {
            recentFileActions[ j ]->setVisible( false );
        }
    }
    separatorAction->setVisible( !recentFiles.isEmpty() );
    clearRecentAction->setVisible( !recentFiles.isEmpty() );
*/
}


void FontEditor::setCurrentFile( const QString &fileName )
{
    QString shownName = tr("(New)");

    currentFile = fileName;
    updateModified( false );
    currentModifyTime = QDateTime::currentDateTime();

    if ( !currentFile.isEmpty() ) {
        currentModifyTime = QFileInfo( fileName ).lastModified();
        currentDir = QDir::cleanPath( QFileInfo( fileName ).absolutePath() );
        shownName = QFileInfo( currentFile ).fileName();
        recentFiles.removeAll( currentFile );
        recentFiles.prepend( currentFile );
        updateRecentFileActions();
    }
    setWindowTitle( tr("Font Editor - %1 [*]").arg( shownName ));
}


void FontEditor::updatePosition( const QPoint &newPos )
{
    infoBar->setPosition( newPos );
}


void FontEditor::updateModified()
{
    //updateModified( editor->document()->isModified() );
    updateModified( false );
}
void FontEditor::updateModified( bool isModified )
{
    //editor->document()->setModified( isModified );
    setWindowModified( isModified );
    modifiedLabel->setText( isModified? tr("Modified"): "");
    if ( isModified ) messagesLabel->setText("");
}


void FontEditor::setSelect()
{
    editor->setSelectMode( true );
    selectAction->setChecked( true );
}


void FontEditor::setSelectAll()
{
    editor->selectAll();
    selectAction->setChecked( true );
}


void FontEditor::setDeselect()
{
    editor->setSelectMode( false );
    selectAction->setChecked( false );
}


void FontEditor::readSettings()
{
}


void FontEditor::writeSettings()
{
}


bool FontEditor::okToContinue()
{
    if ( isWindowModified() ) {
        // This approach allows us to set a shortcut on the Discard button
        QMessageBox confirm( QMessageBox::Warning,
                             tr("Text Editor"),
                             tr("There are unsaved changes.<p>Do you want to save the changes?"),
                             QMessageBox::Save, this );
        confirm.addButton( tr("&Discard"), QMessageBox::DestructiveRole );
        confirm.addButton( QMessageBox::Cancel );
        int r = confirm.exec();
        if ( r == QMessageBox::Save )
            return save();
        else if ( r == QMessageBox::Cancel )
            return false;
    }
    return true;
}


bool FontEditor::save()
{
    if ( currentFile.isEmpty() )
        return saveAs();
    else
        return saveFile( currentFile );
}


bool FontEditor::saveAs()
{
#ifndef __OS2__
    QString fileName = QFileDialog::getSaveFileName( this,
                                                     tr("Save File"),
                                                     currentDir,
                                                     tr("All files (*)"));
#else
    QString fileName = OS2Native::getSaveFileName( this,
                                                   tr("Save File"),
                                                   currentDir,
                                                   tr("All files (*)"));
#endif
    if ( fileName.isEmpty() )
        return false;
    return saveFile( fileName );
}


bool FontEditor::saveFile( const QString &fileName )
{
    QFile *file = new QFile( fileName );
    bool bExists = ( file->exists() );

    if ( bExists ) {
        QDateTime fileTime = QFileInfo( fileName ).lastModified();
        if ( fileTime > currentModifyTime ) {
            int r = QMessageBox::warning( this,
                                          tr("File Modified"),
                                          tr("The modification time on %1 has changed."
                                             "<p>This file may have been modified by another "
                                             "application or process. If you save now, any "
                                             "such modifications will be lost.</p>"
                                             "<p>Save anyway?</p>").arg( QDir::toNativeSeparators( fileName )),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes
                                        );
            if ( r == QMessageBox::No )
                return false;
        }
    }

    // Always open in read/write mode, as it seems to preserve EAs on existing files.
    if ( !file->open( QIODevice::ReadWrite | QFile::Text )) {
        QMessageBox::critical( this, tr("Error"), tr("Error writing file"));
        return false;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    /*
    QDataStream out( file );
    // Use to write data structures:
    out.writeRawData( pchData, iBytes )
    if ( iSize != -1 ) file->resize( iSize );
    file->flush();
    file->close();

    showMessage( tr("Saved file: %1 (%2 bytes written)").arg( QDir::toNativeSeparators( fileName )).arg( iSize ));
    */
    setCurrentFile( fileName );

    if ( !bExists ) {
#ifdef __OS2__
        // If this is a new file, get rid of the useless default EAs added by klibc
        OS2Native::deleteEA( fileName.toLocal8Bit().data(), "UID");
        OS2Native::deleteEA( fileName.toLocal8Bit().data(), "GID");
        OS2Native::deleteEA( fileName.toLocal8Bit().data(), "MODE");
        OS2Native::deleteEA( fileName.toLocal8Bit().data(), "INO");
        OS2Native::deleteEA( fileName.toLocal8Bit().data(), "RDEV");
        OS2Native::deleteEA( fileName.toLocal8Bit().data(), "GEN");
        OS2Native::deleteEA( fileName.toLocal8Bit().data(), "FLAGS");
#endif
    }

    QApplication::restoreOverrideCursor();
    return true;
}

