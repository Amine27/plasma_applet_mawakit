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


#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QWidget>
#include <Phonon/MediaObject>
#include <Phonon/VolumeSlider>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>

#include "ui_notificationform.h"

class NotificationForm : public QWidget
{
    Q_OBJECT
public:
    NotificationForm(QWidget *parent = 0);
    
    QString soundFilePath();
    QString soundFajrFilePath();
    QString soundDuaaFilePath();
    bool athanSoundEnabled();
    bool duaaSoundEnabled();
    qreal soundVolume();
    bool notifyBeforeAthan();
    int notifyBeforeAthanVal();
    bool notifyAfterAthan();
    int notifyAfterAthanVal();
    bool stopPlayers();
    
    void setSoundFilePath(QString path);
    void setSoundFajrFilePath(QString path);
    void setSoundDuaaFilePath(QString path);
    void setAthanSoundEnabled(bool enbaled);
    void setDuaaSoundEnabled(bool enbaled);
    void setSoundVolume(qreal vol);
    void setNotifyBeforeAthan(bool enabled);
    void setNotifyBeforeAthanVal(int val);
    void setNotifyAfterAthan(bool enabled);
    void setNotifyAfterAthanVal(int val);
    void setStopPlayers(bool enabled);
    
private slots:
    bool checkSoundFile(const KUrl &url);
    void enableVolumeSlider(bool enabled);
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void playStop();
    void soundFinished();
    
private:
    
    Phonon::VolumeSlider *volumeSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    
    QToolButton *tmpToolBtn;
    
    Ui::NotificationForm m_notificationUi;
    
};

#endif
