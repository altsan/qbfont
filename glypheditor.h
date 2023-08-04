/******************************************************************************
** glypheditor.h
**
**  Copyright (C) 2023 Alexander Taylor
**  Based in part on sample code from Blanchette & Summerfield, "C++ GUI
**  Programming with Qt4" (Second Edition), (C) 2007 Pearson.
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

#ifndef GLYPHEDITOR_H
#define GLYPHEDITOR_H

#include <QColor>
#include <QImage>
#include <QWidget>

class GlyphEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( QImage glyphImage READ glyphImage WRITE setGlyphImage )
    Q_PROPERTY( int zoomFactor READ zoomFactor WRITE setZoomFactor )
    Q_PROPERTY( int baseLine READ baseLine WRITE setBaseLine )
    Q_PROPERTY( int increment READ increment WRITE setIncrement )
    Q_PROPERTY( bool selectMode READ selectMode WRITE setSelectMode )

public:
    GlyphEditor( QWidget *parent = 0 );

    QSize   sizeHint() const;

    void    setZoomFactor( int newZoom );
    int     zoomFactor() const { return iZoom; }

    void    setGlyphImage( const QImage &newImage );
    QImage  glyphImage() const { return image; }

    void    setBaseLine( int offset );
    int     baseLine() const { return iBaseLine; }

    bool    isChanged() const { return bChanged; }

    void    setSelectMode( bool on );
    bool    selectMode() const { return bSelectionOn; }

    void    setIncrement( int increment );
    int     increment() const { return image.width(); }

    void    selectAll();
    void    clear();
    void    mirror( Qt::Orientation direction );

    void    insertColumnLeft( int pos, bool widen=false );
    void    insertColumnRight( int pos, bool widen=false );
/*
    void    deleteColumnLeft( int pos, bool narrow=false );
    void    deleteColumnRight( int pos, bool narrow=false );

    void    widenBoth();
    void    narrowBoth();

    void    insertRowUp( int pos );
    void    deleteRowDown( int pos );
*/




signals:
    void positionChanged( const QPoint &newPosition );

protected:
    void mousePressEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void paintEvent( QPaintEvent *event );
    void resizeEvent( QResizeEvent *event );

private:
    void  setImagePixel( const QPoint &pos, bool opaque );
    void  setModified( bool modified );
    void  startSelection( const QPoint &pos );
    void  expandSelection( const QPoint &pos );
    QRect pixelRect( int i, int j ) const;
    bool  showGrid() const;

    const QRgb rgbOn  = qRgba( 0, 0, 0, 255 );
    const QRgb rgbOff = qRgba( 255, 255, 255, 0 );

    QImage  image;
    QPoint  curPosition;
    QRect   curSelection;

    int     iZoom;
    int     iBaseLine;
    bool    bChoiceOn;
    bool    bSelectionOn;
    bool    bChanged;
};

#endif
