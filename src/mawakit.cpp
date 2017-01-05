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


#include "mawakit.h"

#include <QTextCodec>
#include <KIcon>
#include <KIconLoader>
#include <QGraphicsLinearLayout>
#include <plasma/tooltipmanager.h>
#include <Plasma/Service>
#include <KConfigDialog>
#include <KGlobal>
#include <KStandardDirs>
#include <KIconEffect>
#include <QPainter>

K_GLOBAL_STATIC_WITH_ARGS(KComponentData, mawakitComponent, ("mawakit",
                                                            QByteArray(),
                                                            KComponentData::SkipMainComponentRegistration))


Mawakit::Mawakit(QObject *parent, const QVariantList &args)
        : Plasma::Applet(parent, args)
{
    setAspectRatioMode(Plasma::ConstrainedSquare);
    int iconSize = IconSize(KIconLoader::Desktop);
    resize(iconSize * 2, iconSize * 2);

    m_today = QDate::currentDate();
    //m_tmpTime = QTime(4, 7, 25); // DE-BUG
    m_resourceDir = KGlobal::dirs()->findResourceDir("data", "mawakit/") + "mawakit/";
}

Mawakit::~Mawakit()
{
    writeConfig();
}

void Mawakit::init()
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_icon = new Plasma::IconWidget(KIcon("kaba"), QString(), this);
    layout->addItem(m_icon);

    Plasma::ToolTipManager::self()->registerWidget(this);
    
    m_qiblaDirectionAct = new QAction(i18n("Qibla Direction"), this);
    m_qiblaDirectionAct->setIcon(KIcon("rating"));
    addAction("qiblaDirection", m_qiblaDirectionAct);
    connect(m_qiblaDirectionAct, SIGNAL(triggered()), this, SLOT(slotQiblaDirection()));
    
    m_monthlyPrayerAct = new QAction(i18n("Monthly Prayer"), this);
    m_monthlyPrayerAct->setIcon(KIcon("view-calendar"));
    addAction("monthlyPrayer", m_monthlyPrayerAct);
    connect(m_monthlyPrayerAct, SIGNAL(triggered()), this, SLOT(slotMonthlyPrayer()));
    
    QAction * separator = new QAction(this);
    separator->setSeparator(true);
    
    m_actions.append(m_qiblaDirectionAct);
    m_actions.append(m_monthlyPrayerAct);
    m_actions.append(separator);
    
    m_prayerNameList << i18n("Fajr") << i18n("Sunrise") << i18n("Dhuhr")
                    << i18n("Asr") << i18n("Maghrib") << i18n("Isha'a");
                
    for(int i=0; i<6; i++)
    {
        m_prayerTimeList << QTime(0, 0, 0);
        m_prayerColorList << "#1c9a1c";
    }
    
    m_time = QTime::currentTime();

    readConfig();
    
    calculatePrayerTime();
    
    calculateManualTime();
    
    connectToEngine();

    //slotMonthlyPrayer(); // TEST
}

void Mawakit::connectToEngine()
{
    m_lastTimeSeen = QTime();

    timeEngine = dataEngine("time");
    timeEngine->connectSource("Local", this, 1000);
}

void Mawakit::createConfigurationInterface(KConfigDialog *parent)
{
    m_location = new LocationForm;
    m_notification = new NotificationForm;
    m_display = new DisplayForm;
    
    parent->addPage(m_location, i18n("Location"), "preferences-desktop-locale");
    parent->addPage(m_notification, i18n("Notifications"), "preferences-desktop-notification");
    parent->addPage(m_display, i18n("Display"), "preferences-desktop-display");
    
    m_location->setCurrentCountry(m_countryNumber);
    m_location->setCurrentCity(m_cityNumber);
    m_location->setLongitude(m_longitude);
    m_location->setLatitude(m_latitude);
    m_location->setTimeZone(m_timeZone);
    m_location->setCalcMethod(m_calcMethodIndex);
    m_location->setAsrJuristic(m_asrJuristicIndex);
    m_location->setHighLat(m_highLatIndex);
    m_location->setDayLight(m_dayLight);
    m_location->setManualCalculation(m_manualCalc);
    m_location->setManualTimeList(m_manualTimeList);
    m_location->setFixedTimeList(m_fixedTimeList);
    
    m_notification->setSoundFilePath(m_soundFilePath);
    m_notification->setSoundFajrFilePath(m_soundFajrFilePath);
    m_notification->setSoundDuaaFilePath(m_soundDuaaFilePath);
    m_notification->setAthanSoundEnabled(m_athanSoundEnabled);
    m_notification->setDuaaSoundEnabled(m_duaaSoundEnabled);
    m_notification->setSoundVolume(m_soundVolume);
    m_notification->setNotifyBeforeAthan(m_notifyBeforeAthan);
    m_notification->setNotifyBeforeAthanVal(m_notifyBeforeAthanVal);
    m_notification->setNotifyAfterAthan(m_notifyAfterAthan);
    m_notification->setNotifyAfterAthanVal(m_notifyAfterAthanVal);
    m_notification->setStopPlayers(m_stopPlayers);
    
    m_display->setPrayersToShow(m_prayersToShowList);
    m_display->setShowNextPrayer(m_showNextPrayer);
    m_display->setShowCountdown(m_showCountdown);
    m_display->setShowElapsed(m_showElapsed);
    m_display->setIgnoreHour(m_ignoreHour);
    m_display->setIgnoreSecs(m_ignoreSecs);
    m_display->setIconEffect(m_iconEffect);
    m_display->setAmpmFormat(m_ampmFormat);
    
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

void Mawakit::configAccepted()
{
    m_country = m_location->country();
    m_city = m_location->city();
    m_latitude = m_location->latitude();
    m_longitude = m_location->longitude();
    m_timeZone = m_location->timeZone();
    m_calcMethod = m_location->calcMethod();
    m_calcMethodIndex = m_location->calcMethodIndex();
    m_asrJuristic = m_location->asrJuristic();
    m_asrJuristicIndex = m_location->asrJuristicIndex();
    m_highLat = m_location->highLat();
    m_highLatIndex = m_location->highLatIndex();
    m_dayLight = m_location->dayLight();
    m_countryNumber = m_location->countryNumber();
    m_cityNumber = m_location->cityNumber();

    calculatePrayerTime();
    
    m_manualCalc = m_location->manualCalculation();
    m_manualTimeList = m_location->manualTimeList();
    m_fixedTimeList = m_location->fixedTimeList();
    
    if(m_manualCalc)
    {
        m_timeDiffList.clear();
        
        for(int i=0; i<6; i++)
        {
            m_timeDiffList.append(m_prayerTimeList.at(i).secsTo(QTime::fromString(m_manualTimeList.at(i), "hh:mm:ss")));
        }
    }
    
    m_soundFilePath = m_notification->soundFilePath();
    m_soundFajrFilePath = m_notification->soundFajrFilePath();
    m_soundDuaaFilePath = m_notification->soundDuaaFilePath();
    m_athanSoundEnabled = m_notification->athanSoundEnabled();
    m_duaaSoundEnabled = m_notification->duaaSoundEnabled();
    m_soundVolume = m_notification->soundVolume();
    m_notifyBeforeAthan = m_notification->notifyBeforeAthan();
    m_notifyBeforeAthanVal = m_notification->notifyBeforeAthanVal();
    m_notifyAfterAthan = m_notification->notifyAfterAthan();
    m_notifyAfterAthanVal = m_notification->notifyAfterAthanVal();
    m_stopPlayers = m_notification->stopPlayers();
    
    m_prayersToShowList = m_display->prayersToShow();
    m_showNextPrayer = m_display->showNextPrayer();
    m_showCountdown = m_display->showCountdown();
    m_showElapsed = m_display->showElapsed();
    m_ignoreHour = m_display->ignoreHour();
    m_ignoreSecs = m_display->ignoreSecs();
    m_iconEffect = m_display->iconEffect();
    m_ampmFormat = m_display->ampmFormat();
    
    if(!m_iconEffect)
        m_icon->setIcon(KIcon("kaba"));
    
    writeConfig();
    calculateManualTime();
}

void Mawakit::readConfig()
{
    KConfigGroup cg = config();
    
    m_country = cg.readEntry("country", "Algeria");
    m_city = cg.readEntry("city", "Mostaganem");
    m_latitude = cg.readEntry("latitude", 35.917);
    m_longitude = cg.readEntry("longitude", 0.1567);
    m_timeZone = cg.readEntry("timezone", 1.0);
    m_calcMethod = cg.readEntry("calcMethod", static_cast<int>(PrayerTimes::MWL));
    m_calcMethodIndex = cg.readEntry("calcMethodIndex", 2);
    m_asrJuristic = cg.readEntry("asrJuristic", static_cast<int>(PrayerTimes::Shafii));
    m_asrJuristicIndex = cg.readEntry("asrJuristicIndex", 0);
    m_highLat = cg.readEntry("highLat", static_cast<int>(PrayerTimes::MidNight));
    m_highLatIndex = cg.readEntry("highLatIndex", 1);
    m_dayLight = cg.readEntry("daylight", false);
    m_countryNumber = cg.readEntry("countryNumber", 4);
    m_cityNumber = cg.readEntry("cityNumber", 42);
    m_manualCalc = cg.readEntry("manualCalc", false);
    
    if(m_manualCalc)
    {   
        m_timeDiffList = cg.readEntry("timeDiff", QList<int>());
        m_fixedTimeList = cg.readEntry("fixedTime", QList<bool>());
        m_manualTimeList = cg.readEntry("manualTime", QStringList());
    }
    
    //m_soundFilePath = cg.readEntry("soundFilePath", m_resourceDir + "hayy.ogg");
    //m_soundFajrFilePath = cg.readEntry("soundFajrFilePath", m_resourceDir + "hayy.ogg");
    m_soundFilePath = cg.readEntry("soundFilePath", "/home/amine27/Son/Islamique/Azan-Masjidil-Haram.ogg");
    m_soundFajrFilePath = cg.readEntry("soundFajrFilePath", "/home/amine27/Son/Islamique/Azan-Masjidil-Haram.ogg");
    m_soundDuaaFilePath = cg.readEntry("soundDuaaFilePath", m_resourceDir + "duaa.ogg");
    m_athanSoundEnabled = cg.readEntry("athanSoundEnabled", true);
    m_duaaSoundEnabled = cg.readEntry("duaaSoundEnabled", true);
    m_soundVolume = cg.readEntry("soundVolume", 1.0);
    m_notifyBeforeAthan = cg.readEntry("notifyBeforeAthan", true);
    m_notifyBeforeAthanVal = cg.readEntry("notifyBeforeAthanVal", 5);
    m_notifyAfterAthan = cg.readEntry("notifyAfterAthan", false);
    m_notifyAfterAthanVal = cg.readEntry("notifyAfterAthanVal", 5);
    m_stopPlayers = cg.readEntry("stopPlayersEnabled", true);
    
    m_prayersToShowList = cg.readEntry("prayersToShowList", QList<bool>() << true << false << true
                                                                            << true << true << true);
    m_showNextPrayer = cg.readEntry("showNextPrayer", false);
    m_showCountdown = cg.readEntry("showCountdown", true);
    m_showElapsed = cg.readEntry("showElapsed", true);
    m_ignoreHour = cg.readEntry("ignoreHour", true);
    m_ignoreSecs = cg.readEntry("ignoreSecs", true);
    m_iconEffect = cg.readEntry("iconEffect", false);
    m_ampmFormat = cg.readEntry("ampmFormat", false);
}

void Mawakit::writeConfig()
{
    KConfigGroup cg = config();
    
    cg.writeEntry("country", m_country);
    cg.writeEntry("city", m_city);
    cg.writeEntry("latitude", m_latitude);
    cg.writeEntry("longitude", m_longitude);
    cg.writeEntry("timezone", m_timeZone);
    cg.writeEntry("calcMethod", m_calcMethod);
    cg.writeEntry("calcMethodIndex", m_calcMethodIndex);
    cg.writeEntry("asrJuristic", m_asrJuristic);
    cg.writeEntry("asrJuristicIndex", m_asrJuristicIndex);
    cg.writeEntry("highLat", m_highLat);
    cg.writeEntry("highLatIndex", m_highLatIndex);
    cg.writeEntry("daylight", m_dayLight);
    cg.writeEntry("countryNumber", m_countryNumber);
    cg.writeEntry("cityNumber", m_cityNumber);
    cg.writeEntry("manualCalc", m_manualCalc);
    
    if(m_manualCalc && (m_manualTimeList.size() > 0 || m_fixedTimeList.size() > 0))
    {
        cg.writeEntry("fixedTime", m_fixedTimeList);
        cg.writeEntry("timeDiff", m_timeDiffList);
        cg.writeEntry("manualTime", m_manualTimeList);
    }
    
    cg.writeEntry("soundFilePath", m_soundFilePath);
    cg.writeEntry("soundFajrFilePath", m_soundFajrFilePath);
    cg.writeEntry("soundDuaaFilePath", m_soundDuaaFilePath);
    cg.writeEntry("athanSoundEnabled", m_athanSoundEnabled);
    cg.writeEntry("duaaSoundEnabled", m_duaaSoundEnabled);
    cg.writeEntry("soundVolume", m_soundVolume);
    cg.writeEntry("notifyBeforeAthan", m_notifyBeforeAthan);
    cg.writeEntry("notifyBeforeAthanVal", m_notifyBeforeAthanVal);
    cg.writeEntry("notifyAfterAthan", m_notifyAfterAthan);
    cg.writeEntry("notifyAfterAthanVal", m_notifyAfterAthanVal);
    cg.writeEntry("stopPlayersEnabled", m_stopPlayers);
    
    cg.writeEntry("prayersToShowList", m_prayersToShowList);
    cg.writeEntry("showNextPrayer", m_showNextPrayer);
    cg.writeEntry("showCountdown", m_showCountdown);
    cg.writeEntry("showElapsed", m_showElapsed);
    cg.writeEntry("ignoreHour", m_ignoreHour);
    cg.writeEntry("ignoreSecs", m_ignoreSecs);
    cg.writeEntry("iconEffect", m_iconEffect);
    cg.writeEntry("ampmFormat", m_ampmFormat);
}

void Mawakit::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    Q_UNUSED(source);
    
    m_time = data["Time"].toTime();

    if (m_time.minute() == m_lastTimeSeen.minute() &&
        m_time.second() == m_lastTimeSeen.second()) {
        // avoid unnecessary repaints
        return;
    }
    
    //m_tmpTime = m_time = m_tmpTime.addSecs(1); // DE-BUG
    
    m_remainingTime = QTime(0, 0, 0);
    m_remainingTime = m_remainingTime.addSecs(m_time.secsTo(m_nextPrayerTime)+1);
        
    //qDebug() << m_time << m_previousPrayerTime << m_nextPrayerTime << m_remainingTime; // // DE-BUG
    
    if(m_remainingTime == QTime(0, 0, 0))
    {
        QString msg = i18n("It is time for %1 prayer.", m_nextPrayerName);
        
        notification = new KNotification("PrayerTime");
        notification->setTitle(i18n("Prayer Time"));
        notification->setPixmap(QPixmap());
        notification->setComponentData(*mawakitComponent);
        notification->setText(msg);
        notification->setFlags(KNotification::Persistent); // FIXME: fix possible mem leak!
        
        if(m_athanSoundEnabled)
        {
            notification->setActions(QStringList() << i18n("Stop Sound"));
            connect(notification, SIGNAL(activated(unsigned int )), this , SLOT(slotStopSound(unsigned int)) );
        
            player = new Phonon::MediaObject(this);
            if(m_nextPrayerTime == m_prayerTimeList.at(0)) // Fajr adhan
            {
                player->setCurrentSource(Phonon::MediaSource(m_soundFajrFilePath));
                qDebug() << "Fajr athan sound";
            }
            else
            {
                player->setCurrentSource(Phonon::MediaSource(m_soundFilePath));
                //qDebug() << "not Fajr athan sound";
            }
                
            audioOutput = new Phonon::AudioOutput(this);
            Phonon::createPath(player, audioOutput);
            audioOutput->setVolume(m_soundVolume);
        
            connect(player, SIGNAL(finished()), this, SLOT(slotSoundFiniched()));
            player->play();
        }

        nextPrayer();

        notification->sendEvent();
        
        qDebug() << msg;
    }
    
    if(m_stopPlayers)
    {
        if(m_remainingTime == QTime(0, 0, 5)) // Stop current players apps 5 secs before athan
            stopPlayers();
    }
    
    if(m_notifyBeforeAthan)
    {
        if(m_remainingTime.addSecs(-m_notifyBeforeAthanVal*60) == QTime(0, 0, 0))
        {
            QString msg = i18np("%1 minute until %2 prayer.", "%1 minutes until %2 prayer.", m_notifyBeforeAthanVal, m_nextPrayerName);
            showAlertNotification(msg);
        }
    }
    
    if(m_notifyAfterAthan)
    {
        if(m_previousPrayerTime.secsTo(m_time) == m_notifyAfterAthanVal*60)
        {
            QString msg = i18np("%1 minute after %2 prayer.", "%1 minutes after %2 prayer.", m_notifyAfterAthanVal, m_previousPrayerName);
            showAlertNotification(msg);
        }
    }
    
    if(m_today != QDate::currentDate())
    {
        qDebug() << "schedule for new day";
        m_today = QDate::currentDate();
        calculatePrayerTime();
        calculateManualTime();
    }
    
    if(m_iconEffect)
        paintIcon();
    
    // if the tooltip is visible, keep the displayed data up-to-date
    if (Plasma::ToolTipManager::self()->isVisible(this))
    {
        toolTipAboutToShow();
    }

    m_lastTimeSeen = m_time;
    update();
}

void Mawakit::showAlertNotification(QString msg)
{
    notification = new KNotification("PrayerTime");
    notification->setTitle(i18n("Prayer Time"));
    notification->setPixmap(QPixmap());
    notification->setComponentData(*mawakitComponent);
    notification->setText(msg);
    notification->setFlags(KNotification::CloseOnTimeout);
    notification->sendEvent();
            
    qDebug() << msg;
}

void Mawakit::toolTipAboutToShow()
{
    QString tip = i18n("<table>");
    QString clockFormat = "hh:mm";
    
    if(m_ampmFormat)
        clockFormat = "hh:mm AP";

    QTime elapsedTime = QTime(0, 0, 0);
    elapsedTime = elapsedTime.addSecs(m_previousPrayerTime.secsTo(m_time));

    for(int i=0; i<6; i++)
    {
        if(m_prayersToShowList.at(i))
        {
            tip += i18n("<tr><td>%1</td><td><font color='%2'>%3</font></td></tr>",
                        m_prayerNameList.at(i), m_prayerColorList.at(i),
                        m_prayerTimeList.at(i).toString(clockFormat)
                    );
        }
    }
    
    if(m_showNextPrayer)
    {
        tip += i18n("<tr><td>%1</td><td>%2</td></tr>", m_nextPrayerName, m_nextPrayerTime.toString(clockFormat));
        tip += i18n("</table>");
    }
    
    else
    {
        tip += i18n("</table>");
        
        if(m_showElapsed && elapsedTime < QTime(0, 10, 0)) // Show 10 minutes after prayer time
            tip += i18n("%1 from %2", elapsedTime.toString("mm:ss"), m_previousPrayerName);
            
        else if(m_showCountdown)
        {   
            if(m_ignoreHour && m_remainingTime.hour() == 0)
                tip += i18n("%1", m_remainingTime.toString("mm:ss"));
            
            else if(m_ignoreSecs && m_remainingTime.hour() > 0)
                tip += i18n("%1", m_remainingTime.toString("hh:mm"));
            
            else
                tip += i18n("%1", m_remainingTime.toString("hh:mm:ss"));
            
            tip += i18n(" to %1", m_nextPrayerName);
        }
    }

    Plasma::ToolTipContent toolTipData(i18n("Prayer Times"),
                                    tip,
                                    KIcon("kaba"));
    
    Plasma::ToolTipManager::self()->setContent(this, toolTipData);
}

void Mawakit::paintIcon()
{
    static qint64 oldMergePos = -1;
    
    if( m_baseIcon.isNull() )
    {
        m_baseIcon = KIconLoader::global()->loadIcon("kaba", KIconLoader::Desktop);
    }

    if( m_grayedIcon.isNull() )
    {
        m_grayedIcon = m_baseIcon; // copies object
        KIconEffect::semiTransparent( m_grayedIcon );
    }
    
    qint64 position = QTime(0, 0, 0).secsTo(m_remainingTime);
    qint64 length;
    
    if(m_previousPrayerTime == m_prayerTimeList.at(5)) // Isha in the next day
        length = abs(m_previousPrayerTime.secsTo(m_nextPrayerTime)+24*60*60);
    else    
        length = abs(m_previousPrayerTime.secsTo(m_nextPrayerTime));
    
    const qint64 mergePos = ( float( position ) / length ) * m_tmpIcon.height();
    
    // return if pixmap would stay the same
    if( oldMergePos == mergePos )
        return;
    
    // draw m_baseIcon on top of the gray version
    m_tmpIcon = m_grayedIcon; // copies object
    QPainter p(&m_tmpIcon);
    p.drawPixmap(0, 0, m_baseIcon, 0, 0, 0, m_tmpIcon.height()-mergePos);
    p.end();
    
    oldMergePos = mergePos;

    m_icon->setIcon(m_tmpIcon);
}

void Mawakit::calculatePrayerTime()
{   
    double times[PrayerTimes::TimesCount];

    PrayerTimes prayerTimes(static_cast<PrayerTimes::CalculationMethod>(m_calcMethod),
                            static_cast<PrayerTimes::JuristicMethod>(m_asrJuristic),
                            static_cast<PrayerTimes::AdjustingMethod>(m_highLat)
                           );
    prayerTimes.get_prayer_times(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().day(),
                                m_latitude, m_longitude, m_timeZone, m_dayLight, times);
    
    for(int i=0, j=0; i < PrayerTimes::TimesCount; ++i)
    {
        //qDebug() << QString::fromStdString(PrayerTimes::float_time_to_time24(times[i]));
        if(i != PrayerTimes::Sunset) // Skip Sunset
        {
            m_prayerTimeList.replace(j, QTime::fromString(QString::fromStdString(PrayerTimes::float_time_to_time24(times[i])), "hh:mm:ss"));
            j++;
        }
    }
    
    qDebug() << m_country << m_city << m_longitude << m_latitude << m_calcMethod
			<< m_prayerTimeList.at(0).toString("hh:mm:ss") << m_prayerTimeList.at(1).toString("hh:mm:ss")
			<< m_prayerTimeList.at(2).toString("hh:mm:ss") << m_prayerTimeList.at(3).toString("hh:mm:ss")
			<< m_prayerTimeList.at(4).toString("hh:mm:ss") << m_prayerTimeList.at(5).toString("hh:mm:ss");

    nextPrayer();
}

void Mawakit::calculateManualTime()
{
    if(m_manualCalc)
    {
        
        for(int i=0; i<6; i++)
        {
            if(m_fixedTimeList.at(i))
                m_prayerTimeList.replace(i, QTime::fromString(m_manualTimeList.at(i), "hh:mm:ss"));
            
            else
            {
                m_prayerTimeList.replace(i, m_prayerTimeList.at(i).addSecs(m_timeDiffList.at(i)));
                m_manualTimeList.replace(i, m_prayerTimeList.at(i).toString("hh:mm:ss"));
            }
        }
        
        qDebug() << m_country << m_city << m_longitude << m_latitude << m_calcMethod
			<< m_prayerTimeList.at(0).toString("hh:mm:ss") << m_prayerTimeList.at(1).toString("hh:mm:ss")
			<< m_prayerTimeList.at(2).toString("hh:mm:ss") << m_prayerTimeList.at(3).toString("hh:mm:ss")
			<< m_prayerTimeList.at(4).toString("hh:mm:ss") << m_prayerTimeList.at(5).toString("hh:mm:ss");
            
        nextPrayer();
    }
}

void Mawakit::nextPrayer()
{
    QTime currentTime = m_time;
    //QTime currentTime = m_tmpTime; // DE-BUG

    qDebug() << "currentTime:" << currentTime.toString("hh:mm:ss");
    
    m_nextPrayerName = m_prayerNameList.at(0);
    m_nextPrayerTime = m_prayerTimeList.at(0);
    m_previousPrayerName = m_prayerNameList.at(5);
    m_previousPrayerTime = m_prayerTimeList.at(5);
    
    int i;
    for(i=0; i<6; i++)
        m_prayerColorList.replace(i, "#1c9a1c"); // Not prayed
    
    for(i=0; i<6; i++)
    {
        if(currentTime < m_prayerTimeList.at(i) && m_prayersToShowList.at(i) && i!=1)
        {
            m_nextPrayerName = m_prayerNameList.at(i);
            m_nextPrayerTime = m_prayerTimeList.at(i);
            m_prayerColorList.replace(i, "#0000ff"); // Current prayer
            break;
        }
    }
    
    for(int j=0; j<i; j++)
        m_prayerColorList.replace(j, "#990000"); // Prayed
    
    if(i) 
    {
        if(i!=2)
        {
            m_previousPrayerName = m_prayerNameList.at(i-1);
            m_previousPrayerTime = m_prayerTimeList.at(i-1);
        }
        
        else // Avoid Sunrise case
        {
            m_previousPrayerName = m_prayerNameList.at(i-2);
            m_previousPrayerTime = m_prayerTimeList.at(i-2);
        }
        
        if(i==6)
            m_prayerColorList.replace(0, "#0000ff"); // Current prayer in next day
    }
    
    qDebug() << "m_nextPrayerTime:" << m_nextPrayerTime.toString("hh:mm:ss")
            << "m_previousPrayerTime:" << m_previousPrayerTime.toString("hh:mm:ss");
}

void Mawakit::stopPlayers()
{
    Plasma::Service* service = 0;
    Plasma::DataEngine* nowPlayingEngine = dataEngine("nowplaying");
    QStringList players = nowPlayingEngine->sources();
    
    qDebug() << "Looking for players.  Possibilities:" << players;

    for(int i=0; i<players.size(); i++)
    {
        service = dataEngine("nowplaying")->serviceForSource(players.at(i));
        qDebug() << "call stop in" << players.at(i) << "watched player";
        service->startOperationCall(service->operationDescription("stop"));
    }
    
    service->setParent(this);
}

void Mawakit::slotStopSound(unsigned int action)
{
    Q_UNUSED(action);
    qDebug() << "slotStopSound";
    notification->close();
    player->stop();
}

void Mawakit::slotSoundFiniched()
{
    qDebug() << "slotSoundFiniched";
    
    if(m_duaaSoundEnabled)
    {
        player->disconnect();
        player->setCurrentSource(Phonon::MediaSource(m_soundDuaaFilePath));
        player->play();
        
        qDebug() << "play duaa sound";
    }
    //notification->close(); // BUG: sometimes it crash
}

void Mawakit::slotQiblaDirection()
{   
    PrayerTimes prayerTimes;
    
    double dir = prayerTimes.get_qibla_direction(m_latitude, m_longitude);
    double dist = prayerTimes.get_qibla_distance(m_latitude, m_longitude);

    if(dir < 0)
        dir += 360;
    
    QString str = i18n("Qibla direction: %1° from True North", dir) +
                i18n("\nDistance: %1 km", dist);
    
    qDebug() << str << dir;
    
    m_qiblaDirection = new QiblaDirectionForm();
    
    m_qiblaDirection->setLocation(m_country+", "+m_city);
    m_qiblaDirection->setDirection(dir);
    m_qiblaDirection->setDirectionString(str);
    
    m_qiblaDirection->show();
}

void Mawakit::slotMonthlyPrayer()
{
    QString dst = m_dayLight ? "1" : "0";
    
    QStringList infoList;
    infoList << QString::number(m_latitude) << QString::number(m_longitude) << QString::number(m_timeZone)
            << QString::number(m_calcMethod) << QString::number(m_asrJuristic) << QString::number(m_highLat) << dst;
    
    m_monthlyPrayer = new MonthlyPrayerForm();
    
    m_monthlyPrayer->init(infoList, m_manualTimeList, m_timeDiffList, m_fixedTimeList, m_manualCalc, m_ampmFormat,
                        "<td>"+m_country+", </td><td>"+m_city+"</td>");
    m_monthlyPrayer->calculatePrayerTime();
    m_monthlyPrayer->show();
}

#include "mawakit.moc"
