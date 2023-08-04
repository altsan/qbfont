/******************************************************************************
** glypheditor.cpp
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

#include <QtGui>

#include "glypheditor.h"

// ---------------------------------------------------------------------------
// PUBLIC CONSTRUCTOR
//

GlyphEditor::GlyphEditor( QWidget *parent ): QWidget( parent )
{
    setAttribute( Qt::WA_StaticContents );
    setMouseTracking( true );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

    curPosition = QPoint( 0, 0 );
    curSelection = QRect( 0, 0, 0, 0 );
    iBaseLine = 8;
    bChoiceOn = false;
    bSelectionOn = false;

    image = QImage( 32, 32, QImage::Format_ARGB32_Premultiplied );
    clear();

    iZoom = width() / image.width();
    bChanged = false;
}


// ---------------------------------------------------------------------------
// OVERRIDDEN EVENTS
//

void GlyphEditor::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );

    painter.fillRect( event->rect(), QColor("whiteSmoke"));

    if ( showGrid() ) {
        painter.setPen( Qt::lightGray );
        for ( int i = 0; i <= image.width(); ++i )
            painter.drawLine( iZoom * i, 0,
                              iZoom * i, iZoom * image.height() );
        for ( int j = 0; j <= image.height(); ++j ) {
            if ( j == ( image.height() - iBaseLine ))
                painter.setPen( QColor("royalBlue"));
            else
                painter.setPen( Qt::lightGray );
            painter.drawLine( 0, iZoom * j,
                              iZoom * image.width(), iZoom * j );
        }
    }

    for ( int i = 0; i < image.width(); ++i ) {
        for ( int j = 0; j < image.height(); ++j ) {
            QRect rect = pixelRect( i, j );
            if ( !event->region().intersect( rect ).isEmpty() ) {
                QColor pixelColor = QColor::fromRgba( image.pixel( i, j ));
                QColor paintColor = pixelColor.value()? Qt::white: Qt::black;
                if ( curSelection.contains( i, j )) {
                    paintColor.setAlpha( 127 );
                    paintColor.setBlue( 127 );
                }
                painter.fillRect( rect, paintColor );
            }
        }
    }
}


void GlyphEditor::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton ) {
        if ( bSelectionOn )
            startSelection( event->pos() );
        else
            setImagePixel( event->pos(), true );
    }
    else if ( event->button() == Qt::RightButton ) {
        if ( bSelectionOn )
            expandSelection( event->pos() );
        else
            setImagePixel( event->pos(), false );
    }
}


void GlyphEditor::mouseMoveEvent( QMouseEvent *event )
{
    QPoint newPos;
    int cellX,
        cellY;

    if ( event->buttons() & Qt::LeftButton ) {
        if ( bSelectionOn ) {
            expandSelection( event->pos() );
        }
        else
            setImagePixel( event->pos(), true );
    }
    else if ( event->buttons() & Qt::RightButton )
        setImagePixel( event->pos(), false );

    // Translate mouse position into cell coordinates
    if ( showGrid() ) {
        cellX = ( 1 + event->pos().x() ) / iZoom;
        cellY = ( 1 + event->pos().y() ) / iZoom;
    }
    else {
        cellX = event->pos().x() / iZoom;
        cellY = event->pos().y() / iZoom;
    }

    // Now convert the coordinates so that bottom left is (1, 1)
    newPos.setX( cellX >= image.width() ? image.width() : 1 + cellX );
    newPos.setY( cellY >= image.height() ? 1 : image.height() - cellY );
    if ( newPos != curPosition ) {
        curPosition = newPos;
        emit positionChanged( curPosition );
    }
}


void GlyphEditor::resizeEvent( QResizeEvent *event )
{
    iZoom = std::min( width() / image.width(), height() / image.height() );
}


// ---------------------------------------------------------------------------
// OTHER OVERRIDDEN METHODS
//

QSize GlyphEditor::sizeHint() const
{
    QSize size = iZoom * image.size();
    if ( showGrid() )
        size += QSize( 1, 1 );
    return size;
}


// ---------------------------------------------------------------------------
// OTHER PUBLIC METHODS
//

void GlyphEditor::clear()
{
    image.fill( rgbOff );
    setModified( true );
    update();
}


void GlyphEditor::mirror( Qt::Orientation direction )
{
	setGlyphImage( image.mirrored( direction & Qt::Horizontal,
	                               direction & Qt::Vertical   ));
    setModified( true );
    update();
}


/* Insert an empty column at the given position, shifting everything
 * to the left of that position over by one.
 */
void GlyphEditor::insertColumnLeft( int pos, bool widen )
{
    if ( widen ) {
        QImage newImage = image.copy( 0, 0, image.width()+1, image.height() );
        image = newImage;
        updateGeometry();
    }

    for ( int y = 0; y < image.height(); y++ ) {
        QRgb *row = (QRgb *) image.scanLine( y );
        for ( int x = 0; x < image.width(); x++ ) {
            if ( x < pos )
                row[ x ] = row[ x+1 ];
            else if ( x == pos )
                row[ x ] = rgbOff;
            // else leave pixel unchanged
        }
    }

    update();
}


/* Insert an empty column at the given position, shifting everything
 * to the right of that position over by one.
 */
void GlyphEditor::insertColumnRight( int pos, bool widen )
{
    if ( widen ) {
        QImage newImage = image.copy( 0, 0, image.width()+1, image.height() );
        image = newImage;
        updateGeometry();

    }

    for ( int y = 0; y < image.height(); y++ ) {
        QRgb *row = (QRgb *) image.scanLine( y );
        for ( int x = image.width() - 1; x >= 0; x-- ) {
            if ( x > pos )
                row[ x ] = row[ x-1 ];
            if ( x == pos )
                row[ x ] = rgbOff;
            // else leave pixel unchanged
        }
    }

    update();
}


void GlyphEditor::selectAll()
{
    bSelectionOn = true;
    curSelection.setCoords( 0, 0, image.width(), image.height() );
    update();
}


// ---------------------------------------------------------------------------
// PROPERTY HANDLERS
//

void GlyphEditor::setGlyphImage( const QImage &newImage )
{
    if ( newImage != image ) {
        image = newImage.convertToFormat( QImage::Format_ARGB32_Premultiplied );
        update();
        updateGeometry();
    }
}


void GlyphEditor::setZoomFactor( int newZoom )
{
    if ( newZoom < 1 )
        newZoom = 1;

    if ( newZoom != iZoom ) {
        iZoom = newZoom;
        update();
        updateGeometry();
    }
}


void GlyphEditor::setBaseLine( int offset )
{
    iBaseLine = offset;
    update();
}


void GlyphEditor::setIncrement( int increment )
{
    QImage newImage = image.copy( 0, 0, increment, image.height() );
    setGlyphImage( newImage );
    update();
    updateGeometry();
}


void GlyphEditor::setSelectMode( bool on )
{
    bSelectionOn = on;
    curSelection = QRect( 0, 0, 0, 0 );
    if ( on )
        setCursor( Qt::CrossCursor );
    else
        unsetCursor();
    update();
}



// ---------------------------------------------------------------------------
// PRIVATE METHODS
//

void GlyphEditor::setModified( bool modified )
{
    bChanged = modified;
}


void GlyphEditor::setImagePixel( const QPoint &pos, bool opaque )
{
    int i = pos.x() / iZoom;
    int j = pos.y() / iZoom;

    if ( image.rect().contains( i, j )) {
        if ( opaque )
            image.setPixel( i, j, rgbOn );
        else
            image.setPixel( i, j, rgbOff );

        update( pixelRect( i, j ));
        setModified( true );
    }
}


void GlyphEditor::startSelection( const QPoint &pos )
{
    int i = pos.x() / iZoom;
    int j = pos.y() / iZoom;

    curSelection = QRect( i, j, 1, 1 );
    update();
}


void GlyphEditor::expandSelection( const QPoint &pos )
{
    curSelection.setBottomRight( pos / iZoom );
    update();
}


bool GlyphEditor::showGrid() const
{
    return ( iZoom >= 3 );
}


QRect GlyphEditor::pixelRect( int i, int j ) const
{
    if ( showGrid() ) {
        return QRect( iZoom * i + 1, iZoom * j + 1, iZoom - 1, iZoom - 1 );
    }
    else {
        return QRect( iZoom * i, iZoom * j, iZoom, iZoom );
    }
}



