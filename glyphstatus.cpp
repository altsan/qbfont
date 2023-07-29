/******************************************************************************
** glyphstatus.cpp
**
** A status panel for the glyph editor.
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

#include "glyphstatus.h"


GlyphStatus::GlyphStatus( QWidget *parent ): QFrame( parent )
{
    QGridLayout *layout = new QGridLayout();

    setLayout( layout );
    setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    preview = QPixmap( 40, 40 );
    preview.fill( Qt::white );

    iUglValue = 0;
    iUcsValue = 0;
    iWidth = 0;
    iHeight = 0;
    iXpos = 0;
    iYpos = 0;

    lblPreview = new QLabel();
    lblPreview->setPixmap( preview );
    lblPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );

    lblUglValue = new QLabel( tr("UGL Value: %1").arg( iUglValue ));
    lblUcsValue = new QLabel( tr("UCS Value: %1").arg( iUcsValue ));
    lblUglName = new QLabel("<UGL name>");
    lblUcsName = new QLabel("<UCS name>");
    lblIncrement = new QLabel( tr("Increment: %1").arg( iWidth ));
    lblExtent = new QLabel( tr("Max Extent: %1").arg( iHeight ));
    lblCurPos = new QLabel( QString("%1 , %2").arg( iXpos ).arg( iYpos ));
    lblCurPos->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    lblCurPos->setMargin( 4 );


    layout->addWidget( lblPreview, 0, 0, 2, 1 );
    layout->addWidget( lblUglValue, 0, 1 );
    layout->addWidget( lblUcsValue, 1, 1 );
    layout->addWidget( lblUglName, 0, 2 );
    layout->addWidget( lblUcsName, 1, 2 );
    layout->addWidget( lblIncrement, 0, 3 );
    layout->addWidget( lblExtent, 1, 3 );
    layout->addWidget( lblCurPos, 0, 4, 2, 1 );
    layout->setAlignment( lblCurPos, Qt::AlignRight );

    layout->setContentsMargins( 8, 6, 8, 6 );
    layout->setColumnMinimumWidth( 0, preview.width() * 1.2 );
    layout->setColumnStretch( 0, 0 );
    layout->setColumnStretch( 1, 1 );
    layout->setColumnStretch( 2, 1 );
    layout->setColumnStretch( 3, 1 );
    layout->setColumnStretch( 4, 1 );
}


void GlyphStatus::setPreviewImage( const QPixmap &pixmap )
{
    preview = pixmap;
    update();
    updateGeometry();
}


QSize GlyphStatus::sizeHint() const
{
    QSize size = preview.size();
    //size += QSize( lblUglValue->width() + lblUglName->width() + lblIncrement->width() + lblCurPos->width(), 0 );
    return size;
}

void GlyphStatus::setPosition( const QPoint &position )
{
    iXpos = position.x();
    iYpos = position.y();
    lblCurPos->setText( QString("%1 , %2").arg( iXpos ).arg( iYpos ));
}

