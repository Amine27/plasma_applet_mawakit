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


#include "monthlyprayer.h"
#include "prayertimes.hpp"

#include <kdeversion.h>
#include <KCalendarSystem>
#include <KFileDialog>
#include <KColorScheme>

#include <QFile>
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>


MonthlyPrayerForm::MonthlyPrayerForm(QDialog *parent) :
        QDialog(parent)
{
    m_monthlyPrayerUi.setupUi(this);
    
    if (this->layoutDirection() == Qt::LeftToRight)
    {
        m_monthlyPrayerUi.previousMonthToolBtn->setIcon(KIcon("arrow-left"));
        m_monthlyPrayerUi.nextMonthToolBtn->setIcon(KIcon("arrow-right"));
        m_monthlyPrayerUi.previousYearToolBtn->setIcon(KIcon("arrow-left-double"));
        m_monthlyPrayerUi.nextYearToolBtn->setIcon(KIcon("arrow-right-double"));
    }
    else
    {
        m_monthlyPrayerUi.previousMonthToolBtn->setIcon(KIcon("arrow-right"));
        m_monthlyPrayerUi.nextMonthToolBtn->setIcon(KIcon("arrow-left"));
        m_monthlyPrayerUi.previousYearToolBtn->setIcon(KIcon("arrow-right-double"));
        m_monthlyPrayerUi.nextYearToolBtn->setIcon(KIcon("arrow-left-double"));
    }
    m_monthlyPrayerUi.savePushButton->setIcon(KIcon("document-save"));
    m_monthlyPrayerUi.printPushBoutton->setIcon(KIcon("document-print"));
    
    connect(m_monthlyPrayerUi.previousMonthToolBtn, SIGNAL(clicked()), this, SLOT(privousMonth()));
    connect(m_monthlyPrayerUi.nextMonthToolBtn, SIGNAL(clicked()), this, SLOT(nextMonth()));
    connect(m_monthlyPrayerUi.previousYearToolBtn, SIGNAL(clicked()), this, SLOT(privousYear()));
    connect(m_monthlyPrayerUi.nextYearToolBtn, SIGNAL(clicked()), this, SLOT(nextYear()));
    connect(m_monthlyPrayerUi.hijriCheckBox, SIGNAL(clicked()), this, SLOT(calculatePrayerTime()));
    connect(m_monthlyPrayerUi.savePushButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(m_monthlyPrayerUi.printPushBoutton, SIGNAL(clicked()), this, SLOT(print()));

    m_prayerTimeList.clear();
    m_manualTimeList.clear();
    for(int i=0; i<6; i++)
    {
        m_prayerTimeList << QTime(0, 0, 0);
        m_manualTimeList << "";
        m_timeDiffList << 0;
        m_fixedTimeList << false;
    }
}

void MonthlyPrayerForm::init(QStringList infoList, QStringList manualTimeList, QList<int> timeDiffList,
        QList<bool> fixedTimeList, bool manualCalc, bool ampmFormat, QString location)
{
    m_info = infoList;
    m_manualCalc = manualCalc;
    m_date = QDate::currentDate();
    //m_date = QDate(1997, 1, 1);
    m_location = location;

    if(ampmFormat)
        m_clockFormat = "hh:mm AP";
    else
        m_clockFormat = "hh:mm";
    
    if(m_manualCalc)
    {
        for(int i=0; i<timeDiffList.size(); i++)
        {
            m_timeDiffList.replace(i, timeDiffList.at(i));
            m_manualTimeList.replace(i, manualTimeList.at(i));
            m_fixedTimeList.replace(i, fixedTimeList.at(i));
        }
    }
    
    m_eventColor = KColorScheme( QPalette::Normal, KColorScheme::View ).foreground( KColorScheme::NegativeText ).color();
    m_currentDayColor = KColorScheme( QPalette::Normal, KColorScheme::View ).foreground( KColorScheme::PositiveText ).color();
}

void MonthlyPrayerForm::calculatePrayerTime()
{
    QList<QTreeWidgetItem *> items;
    QString calenderName;
        
    m_date.setDate(m_date.year(), m_date.month(), m_date.day());
    QDate tmpDate = m_date;
    QDate reportDate = m_date;
    QDate currentDate = QDate::currentDate();

    qDebug() << "calculatePrayerTime: m_date" << tmpDate;


    if(m_monthlyPrayerUi.hijriCheckBox->isChecked())
    {
#if KDE_IS_VERSION(4, 6, 0)
        m_calendarSys = KCalendarSystem::create(KLocale::IslamicCivilCalendar);
#else
        m_calendarSys = KCalendarSystem::create("hijri");
#endif
        //qDebug() << KDE::versionString();
        tmpDate = tmpDate.addDays(-m_calendarSys->day(tmpDate)+1);
        qDebug() << tmpDate;
        
        calenderName = i18n("Gregorian");
    }
    else
    {
#if KDE_IS_VERSION(4, 6, 0)
        m_calendarSys = KCalendarSystem::create(KLocale::GregorianCalendar);
#else
        m_calendarSys = KCalendarSystem::create("gregorian");
#endif
        tmpDate.setDate(tmpDate.year(), tmpDate.month(), 1);
        
        calenderName = i18n("Hijri");
    }
    
    int dayNbr = m_calendarSys->daysInMonth(tmpDate);
    QString text;
#if KDE_IS_VERSION(4, 4, 0)
    text = m_calendarSys->formatDate(tmpDate, "%B %Y");
#else
    text = m_calendarSys->monthName(tmpDate) + " " + m_calendarSys->yearString(tmpDate);
#endif
    m_monthlyPrayerUi.currentMonthLabel->setText(i18n("<h3>%1</h3>", text));
    
    double times[PrayerTimes::TimesCount];
    PrayerTimes prayerTimes(static_cast<PrayerTimes::CalculationMethod>(m_info.at(3).toInt()),
                            static_cast<PrayerTimes::JuristicMethod>(m_info.at(4).toInt()),
                            static_cast<PrayerTimes::AdjustingMethod>(m_info.at(5).toInt())
                           );

    KCalendarSystem *tmpCal;
        
    if(m_monthlyPrayerUi.hijriCheckBox->isChecked())
    {
#if KDE_IS_VERSION(4, 6, 0)
        m_gregorianCalendar = KCalendarSystem::create(KLocale::GregorianCalendar);
#else
        m_gregorianCalendar = KCalendarSystem::create("gregorian");
#endif
        tmpCal = m_calendarSys;
    }
    else
    {
#if KDE_IS_VERSION(4, 6, 0)
        m_islamicCalendar = KCalendarSystem::create(KLocale::IslamicCivilCalendar);
#else
        m_islamicCalendar = KCalendarSystem::create("hijri");
#endif
        tmpCal = m_islamicCalendar;
        reportDate = tmpDate.addDays(-m_calendarSys->day(tmpDate)+1);
    }
    
    tmpDate = tmpDate.addDays(-1);
    reportDate = reportDate.addDays(-1);
    
    for(int i=0; i < dayNbr; i++)
    {
        tmpDate = tmpDate.addDays(1);
        
        prayerTimes.get_prayer_times(tmpDate.year(), tmpDate.month(), tmpDate.day(), m_info.at(0).toDouble(), m_info.at(1).toDouble(),
                                    m_info.at(2).toDouble(), QVariant(m_info.at(6)).toBool(), times);
        
        for(int i=0, j=0; i < PrayerTimes::TimesCount; ++i)
        {
            if(i != PrayerTimes::Sunset) // Skip Sunset
            {
                m_prayerTimeList.replace(j, QTime::fromString(QString::fromStdString(PrayerTimes::float_time_to_time24(times[i])), "hh:mm:ss"));
                j++;
            }
        }
    
        if(m_manualCalc)
            calculateManualTime();
        
        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0);
        item->setTextAlignment(0, Qt::AlignHCenter);
        item->setTextAlignment(1, Qt::AlignHCenter);
#if KDE_IS_VERSION(4, 4, 0)
        item->setText(0, m_calendarSys->formatDate(tmpDate, "%e %a"));
#else
        item->setText(0, QString::number(m_calendarSys->day(tmpDate)) +" "+
                         m_calendarSys->weekDayName(tmpDate, KCalendarSystem::ShortDayName));
#endif
        
        if(m_monthlyPrayerUi.hijriCheckBox->isChecked())
        {
            reportDate = tmpDate;
#if KDE_IS_VERSION(4, 4, 0)
            item->setText(1, m_gregorianCalendar->formatDate(reportDate, "%e/%n"));
#else
            item->setText(1, QString::number(m_gregorianCalendar->day(reportDate)) +"/"+
                             QString::number(m_gregorianCalendar->month(reportDate)));
#endif
        }
        else
        {
            reportDate = reportDate.addDays(1);
#if KDE_IS_VERSION(4, 4, 0)
            item->setText(1, m_islamicCalendar->formatDate(tmpDate, "%e/%n"));
#else
            item->setText(1, QString::number(m_islamicCalendar->day(tmpDate)) +"/"+
                             QString::number(m_islamicCalendar->month(tmpDate)));
#endif
        }
        
        switch(tmpCal->month(tmpDate))
        {
            case 1:
                if(tmpCal->day(tmpDate) == 1)
                {
                    setEventText(item, i18n("Islamic new year"), m_eventColor);
                }
                else if(tmpCal->day(tmpDate) == 10)
                {
                    setEventText(item, i18n("Aashoraa day"), m_eventColor);
                }
            break;
            case 3:
                if(tmpCal->day(tmpDate) == 12)
                {
                    setEventText(item, i18n("Birth of the prophet"), m_eventColor);
                }
            break;
            case 7:
                if(tmpCal->day(tmpDate) == 27)
                {
                    setEventText(item, i18n("Isra and Mi'raj"), m_eventColor);
                }
            break;
            case 9:
                if(tmpCal->day(tmpDate) == 1)
                {
                    setEventText(item, i18n("First day of fasting month"), m_eventColor);
                }
            break;
            case 10:
                if(tmpCal->day(tmpDate) == 1)
                {
                    setEventText(item, i18n("Eid al-fitr"), m_eventColor);
                }
            break;
            case 12:
                if(tmpCal->day(tmpDate) == 10)
                {
                    setEventText(item, i18n("Eid al-adha"), m_eventColor);
                }
            break;
        }
        
        if(tmpCal->year(tmpDate) == tmpCal->year(currentDate))
        {
            if(tmpCal->month(tmpDate) == tmpCal->month(currentDate))
            {
                if(tmpCal->day(tmpDate) == tmpCal->day(currentDate))
                {
                    setEventText(item, i18n("Today"), m_currentDayColor);
                }
            }
        }
        
        for(int j=0; j<6; j++)
        {
            item->setText(j+2, m_prayerTimeList.at(j).toString(m_clockFormat));
            item->setTextAlignment(j+2, Qt::AlignHCenter);
        }
        
        items.append(item);
    }

    m_monthlyPrayerUi.prayerTreeWidget->headerItem()->setText(1, calenderName);
    m_monthlyPrayerUi.prayerTreeWidget->clear();
    m_monthlyPrayerUi.prayerTreeWidget->insertTopLevelItems(0, items);
    
    delete m_calendarSys;
    //delete m_islamicCalendar;
    //delete m_gregorianCalendar;
}

void MonthlyPrayerForm::setEventText(QTreeWidgetItem *item, QString eventStr, QColor color)
{
    for(int i=0; i<8; ++i)
    {
        item->setForeground(i, color);
        item->setToolTip(i, eventStr);
    }
}

void MonthlyPrayerForm::calculateManualTime()
{
    for(int i=0; i<6; i++)
    {
        if(m_fixedTimeList.at(i))
            m_prayerTimeList.replace(i, QTime::fromString(m_manualTimeList.at(i), "hh:mm:ss"));

        else
            m_prayerTimeList.replace(i, m_prayerTimeList.at(i).addSecs(m_timeDiffList.at(i)));
    }
}

void MonthlyPrayerForm::privousMonth()
{
    m_date = m_date.addMonths(-1);
    calculatePrayerTime();
}

void MonthlyPrayerForm::nextMonth()
{
    m_date = m_date.addMonths(1);
    calculatePrayerTime();
}

void MonthlyPrayerForm::privousYear()
{
    m_date = m_date.addYears(-1);
    calculatePrayerTime();
}

void MonthlyPrayerForm::nextYear()
{
    m_date = m_date.addYears(1);
    calculatePrayerTime();
}

void MonthlyPrayerForm::save()
{
    toHtml();

    //KFileDialog::setConfirmOverwrite();
    QString path = KFileDialog::getSaveFileName(KUrl("kfiledialog:///"), "*.html *.htm|Web Page (*.html)",
                                                this, i18n("Save File")
                                                );
    if( path.isEmpty() )
    return;

    // Create and open the file.
    QFile file( path );
    if(!file.open( QIODevice::WriteOnly))
    {
        qWarning() << "File save failed - couldn't open file.";
    }

    // Output the HTML with the right text encoding
    QTextStream textStream( &file );
    textStream << m_document.toHtml();
    file.close();
}

void MonthlyPrayerForm::print()
{
    toHtml();
    
    QPrinter printer;
    
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    m_document.print(&printer);
}

void MonthlyPrayerForm::toHtml()
{
    QString html, str, calendar;

    str = m_monthlyPrayerUi.currentMonthLabel->text().replace( QRegExp("<[^>]*>"), "" );
    calendar = m_monthlyPrayerUi.prayerTreeWidget->headerItem()->text(1);

    html = "<html><head><title>Prayer Times</title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"></head>"
        "<body><table dir=\"ltr\" align=\"center\" cellspacing=0 cellpadding=0>"
            "<tr>"
                "<td><font color=green size=+1><b><u>"+i18n("Prayer Times Schedule")+"</u></b></font>"
                "<table dir=\"ltr\" cellspacing=0 cellpadding=0>"
                    "<tr>"
                    "<td><b>"+i18n("Month: ")+"</b></td>"
                    "<td>"+ str +"</td>"
                    "</tr>"
                "</table>"
                "<table dir=\"ltr\">"
                    "<tr>"
                    "<td><b>"+i18n("Location: ")+"</b></td>"
                    "<td>"+m_location+"</td>"
                    "</tr>"
                "</table>"
            "</tr>"
            "</table><br /><br />";

    //qDebug() << html;
    
    html += "<table border=0 bordercolor=\"#00000\" width=500 dir=\"ltr\" align=\"center\" cellspacing=0 cellpadding=0>"
                "<tr bgcolor=\"#CCCCCC\">"
                "<td align=CENTER valign=\"top\">"+i18n("Day")+"</td>"
                "<td align=CENTER valign=\"top\">"+calendar+"</td>"
                "<td align=CENTER valign=\"top\">"+i18n("Fajr")+"</td>"
                "<td align=CENTER valign=\"top\">"+i18n("Sunrise")+"</td>"
                "<td align=CENTER valign=\"top\">"+i18n("Dhuhr")+"</td>"
                "<td align=CENTER valign=\"top\">"+i18n("Asr")+"</td>"
                "<td align=CENTER valign=\"top\">"+i18n("Maghrib")+"</td>"
                "<td align=CENTER valign=\"top\">"+i18n("Isha'a")+"</td>"
                "</tr>";
    
    QTreeWidgetItemIterator it(m_monthlyPrayerUi.prayerTreeWidget);
    while (*it)
    {
        str = ((*it)->text(0).toInt()%2) ? "<tr bgcolor=#ffffff>" : "<tr bgcolor=#ffffcc>";
        
        //qDebug() << (*it)->text(0) << (*it)->text(0).toInt()%2 << c;

        html +=   str +
                "<td align=CENTER>"+(*it)->text(0)+"</td>"
                "<td align=CENTER>"+(*it)->text(1)+"</td>"
                "<td align=CENTER>"+(*it)->text(2)+"</td>"
                "<td align=CENTER>"+(*it)->text(3)+"</td>"
                "<td align=CENTER>"+(*it)->text(4)+"</td>"
                "<td align=CENTER>"+(*it)->text(5)+"</td>"
                "<td align=CENTER>"+(*it)->text(6)+"</td>"
                "<td align=CENTER>"+(*it)->text(7)+"</td>"
                "</tr>";
        
        ++it;
    }

    html += "</table></body></html>";

    //qDebug() << html;

    m_document.setHtml(html);
}
