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


#ifndef MONTHLYPRAYER_H
#define MONTHLYPRAYER_H

#undef QT_NO_STL // To use QString::toStdString()

#include <QDialog>
#include <QTime>
#include <QTextDocument>

#include "ui_monthlyprayerform.h"


class MonthlyPrayerForm : public QDialog
{
    Q_OBJECT
public:
    MonthlyPrayerForm(QDialog *parent = 0);
    
    void init(QStringList infoList, QStringList manualTimeList, QList<int> timeDiffList,
              QList<bool> fixedTimeList, bool m_manualCalc, bool ampmFormat, QString location);

public slots:
    void calculatePrayerTime();
    
private slots:
    void privousMonth();
    void nextMonth();
    void privousYear();
    void nextYear();
    void save();
    void print();
    
private:
    void calculateManualTime();
    void toHtml();
    void setEventText(QTreeWidgetItem *, QString, QColor);
    
private:    
    Ui::MonthlyPrayerForm m_monthlyPrayerUi;
    
    KCalendarSystem *m_calendarSys;
    KCalendarSystem *m_islamicCalendar;
    KCalendarSystem *m_gregorianCalendar;
    
    QColor m_eventColor;
    QColor m_currentDayColor;
    
    QList<QTime> m_prayerTimeList;
    QDate m_date;
    QTextDocument m_document;
    
    QStringList m_info;
    QList<int> m_timeDiffList;
    QStringList m_manualTimeList;
    QList<bool> m_fixedTimeList;
    QString m_clockFormat;
    QString m_location;
    bool m_manualCalc;
    //int m_month;
};

#endif
