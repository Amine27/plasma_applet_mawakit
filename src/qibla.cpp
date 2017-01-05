/***************************************************************************
 *   Copyright (C) 2010-2011 by Amine Roukh     <amineroukh@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QPainter>
#include <QDebug>

#include <KStandardDirs>

#include "qibla.h"

QiblaDirectionForm::QiblaDirectionForm(QDialog *parent) :
        QDialog(parent)
{
    m_qiblaDirectionUi.setupUi(this);
    
    compassWidget = new MyCompass;
    m_qiblaDirectionUi.verticalLayout->insertWidget(1, compassWidget);
}

void QiblaDirectionForm::setLocation(QString location)
{
    m_qiblaDirectionUi.locationLabel->setText(location);
}

void QiblaDirectionForm::setDirection(double direction)
{
    compassWidget->setDegree(direction);
}

void QiblaDirectionForm::setDirectionString(QString directionStr)
{
    m_qiblaDirectionUi.qiblaDirectionLabel->setText(directionStr);
}
 
MyCompass::MyCompass(QWidget *parent) : QWidget(parent)
{
    m_degree = 0;
    m_svg = new Plasma::Svg(this);
    m_svg->setImagePath(KGlobal::dirs()->findResourceDir("data", "mawakit/") + "mawakit/compass.svg");
    m_svg->resize(QSize(247, 312));
}

void MyCompass::setDegree(double degree)
{
    m_degree = degree;
    update();
}

void MyCompass::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.translate((width()/2), (height()/2));
    
    QRectF compassRect = m_svg->elementRect("compass");
    QRectF needleRect = m_svg->elementRect("needle");
    
    p.save();
    m_svg->paint(&p, -(compassRect.width()/2), -(compassRect.height()/2), "compass");
    p.translate(1.2, -6.8); // Manual adjustment for the needle
    p.rotate(m_degree);
    m_svg->paint(&p, -(needleRect.width()/2), -(needleRect.height()/2), "needle");
    p.restore();
}

QSize MyCompass::minimumSizeHint() const
{
    return QSize(350, 350);
}

QSize MyCompass::sizeHint() const
{
    return QSize(350, 350);
}

