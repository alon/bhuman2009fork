/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QTEXTDOCUMENTWRITER_H
#define QTEXTDOCUMENTWRITER_H

#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QTextDocumentWriterPrivate;
class QIODevice;
class QByteArray;
class QTextDocument;
class QTextDocumentFragment;

class Q_GUI_EXPORT QTextDocumentWriter
{
public:
    QTextDocumentWriter();
    QTextDocumentWriter(QIODevice *device, const QByteArray &format);
    QTextDocumentWriter(const QString &fileName, const QByteArray &format = QByteArray());
    ~QTextDocumentWriter();

    void setFormat (const QByteArray &format);
    QByteArray format () const;

    void setDevice (QIODevice *device);
    QIODevice *device () const;
    void setFileName (const QString &fileName);
    QString fileName () const;

    bool write(const QTextDocument *document);
    bool write(const QTextDocumentFragment &fragment);

#ifndef QT_NO_TEXTCODEC
    void setCodec(QTextCodec *codec);
    QTextCodec *codec() const;
#endif

    static QList<QByteArray> supportedDocumentFormats();

private:
    Q_DISABLE_COPY(QTextDocumentWriter)
    QTextDocumentWriterPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
