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


#ifndef MAWAKIT_HEADER
#define MAWAKIT_HEADER

#undef QT_NO_STL // To use QString::toStdString()

#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <plasma/widgets/iconwidget.h>
#include <KNotification>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <QAction>

#include "location.h"
#include "notification.h"
#include "display.h"
#include "qibla.h"
#include "monthlyprayer.h"


class Mawakit : public Plasma::Applet
{
    Q_OBJECT
public:
    Mawakit(QObject *parent, const QVariantList &args);
    ~Mawakit();

    void init();
    void readConfig();
    void writeConfig();
    
    QList<QAction *> contextualActions() { return m_actions; }

signals:
    void settingsChanged(bool);

public slots:
    void configAccepted();
    void toolTipAboutToShow();
    void dataUpdated(const QString& source, const Plasma::DataEngine::Data &data);
    void slotStopSound(unsigned int);
    void slotSoundFiniched();
    void slotQiblaDirection();
    void slotMonthlyPrayer();

protected:
    void createConfigurationInterface(KConfigDialog *parent);

private:
    void paintIcon();
    
    void calculatePrayerTime();
    void calculateManualTime();
    
    void connectToEngine();
    void showAlertNotification(QString msg);
    void stopPlayers(); // Stop now playing
    void nextPrayer();
    
private:
    
    QString m_country;
    QString m_city;
    double m_longitude;
    double m_latitude;
    double m_timeZone;
    int m_calcMethod;
    int m_asrJuristic;
    int m_highLat;
    bool m_dayLight;
    int m_countryNumber;
    int m_cityNumber;
    int m_calcMethodIndex;
    int m_asrJuristicIndex;
    int m_highLatIndex;
    
    QList<QTime> m_prayerTimeList;
    QStringList m_prayerNameList;
    
    QList<int> m_timeDiffList;
    QStringList m_manualTimeList;
    QList<bool> m_fixedTimeList;
    bool m_manualCalc;
    
    QString m_resourceDir;
    QString m_soundFilePath;
    QString m_soundFajrFilePath;
    QString m_soundDuaaFilePath;
    bool m_athanSoundEnabled;
    bool m_duaaSoundEnabled;
    qreal m_soundVolume;
    bool m_notifyBeforeAthan;
    int m_notifyBeforeAthanVal;
    bool m_notifyAfterAthan;
    int m_notifyAfterAthanVal;
    bool m_stopPlayers;
    
    QList<bool> m_prayersToShowList;
    bool m_showNextPrayer;
    bool m_showCountdown;
    bool m_showElapsed;
    bool m_ignoreHour;
    bool m_ignoreSecs;
    bool m_iconEffect;
    bool m_ampmFormat;
    
    QStringList m_prayerColorList;
    
    QString m_nextPrayerName;
    QString m_previousPrayerName;
    QTime m_nextPrayerTime;
    QTime m_previousPrayerTime;
    QTime m_remainingTime;
    
    QTime m_time;
    QTime m_lastTimeSeen;
    QTime m_tmpTime;
    QDate m_today;
    
    QPixmap m_baseIcon;
    QPixmap m_tmpIcon;
    QPixmap m_grayedIcon;
    Plasma::IconWidget *m_icon;
    
    Plasma::DataEngine *timeEngine;
    KNotification *notification;
    
    Phonon::MediaObject* player;
    Phonon::AudioOutput* audioOutput;
    
    QList<QAction *> m_actions;
    QAction *m_qiblaDirectionAct;
    QAction *m_monthlyPrayerAct;
    
    LocationForm *m_location;
    NotificationForm *m_notification;
    DisplayForm *m_display;
    QiblaDirectionForm *m_qiblaDirection;
    MonthlyPrayerForm *m_monthlyPrayer;
};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(mawakit, Mawakit)

#endif
