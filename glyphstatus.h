#ifndef GLYPHSTATUS_H
#define GLYPHSTATUS_H

#include <QFrame>

class QWidget;
class QPixmap;
class QLabel;

class GlyphStatus : public QFrame
{
    Q_OBJECT
    Q_PROPERTY( QPixmap previewImage READ previewImage WRITE setPreviewImage )
/*
    Q_PROPERTY( quint32 uglIndex READ uglIndex WRITE setUglIndex )
    Q_PROPERTY( quint32 ucsIndex READ ucsIndex WRITE setUcsIndex )
    Q_PROPERTY( QString uglName READ uglName WRITE setUglName )
    Q_PROPERTY( QString ucsName READ ucsName WRITE setUcsName )
*/
    Q_PROPERTY( QPoint position READ position WRITE setPosition )

public:
    GlyphStatus( QWidget *parent = 0 );
    QSize sizeHint() const;

    void    setPreviewImage( const QPixmap &pixmap );
    QPixmap previewImage() const { return preview; }

    void    setPosition( const QPoint &position );
    QPoint  position() const { return QPoint( iXpos, iYpos ); }

//protected:

private:
    QLabel *lblPreview;
    QLabel *lblUglValue;
    QLabel *lblUcsValue;
    QLabel *lblUglName;
    QLabel *lblUcsName;
    QLabel *lblIncrement;
    QLabel *lblExtent;
    QLabel *lblCurPos;

    QPoint cursorPos;

    QPixmap preview;
    quint32 iUcsValue;
    quint32 iUglValue;
    QString strUglName;
    QString strUcsName;
    quint16 iWidth;
    quint16 iHeight;
    quint16 iXpos;
    quint16 iYpos;

};

#endif      // GLYPHSTATUS_H
