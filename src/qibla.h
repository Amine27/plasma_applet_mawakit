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


#ifndef QIBLA_H
#define QIBLA_H

#include <QDialog>

#include <Plasma/Svg>

#include "ui_qibladirectionform.h"

class MyCompass : public QWidget
{
    Q_OBJECT
public:
    MyCompass(QWidget *parent = 0);
    
    void setDegree(double);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    
protected:
    void paintEvent(QPaintEvent *);
     
private:
    double m_degree;
    
    Plasma::Svg *m_svg;
};

class QiblaDirectionForm : public QDialog
{
    Q_OBJECT
public:
    QiblaDirectionForm(QDialog *parent = 0);
    
    void setLocation(QString location);
    void setDirection(double direction);
    void setDirectionString(QString directionStr);
    
private:    
    Ui::QiblaDirectionForm m_qiblaDirectionUi;
    
    MyCompass *compassWidget;
};

#endif
