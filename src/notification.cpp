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


#include "notification.h"

#include <kstandarddirs.h>
#include <KMessageBox>
#include <KMimeType>
#include <Phonon/BackendCapabilities>

#include <QDebug>

NotificationForm::NotificationForm(QWidget *parent) :
        QWidget(parent)
{
    m_notificationUi.setupUi(this);
    
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    
    Phonon::createPath(mediaObject, audioOutput);
    
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    
    connect(mediaObject, SIGNAL(finished()), this, SLOT(soundFinished()));
    
    volumeSlider = new Phonon::VolumeSlider(this);
    volumeSlider->setAudioOutput(audioOutput);
    volumeSlider->setMuteVisible(false);
    volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    
    m_notificationUi.horizontalLayout_2->addWidget(volumeSlider);
    
    m_notificationUi.playStopToolButton->setIcon(KIcon("media-playback-start"));
    m_notificationUi.playStopFajrToolButton->setIcon(KIcon("media-playback-start"));
    m_notificationUi.playStopDuaaToolButton->setIcon(KIcon("media-playback-start"));
    
    connect(m_notificationUi.playStopToolButton, SIGNAL(clicked()), this, SLOT(playStop()));
    connect(m_notificationUi.playStopFajrToolButton, SIGNAL(clicked()), this, SLOT(playStop()));
    connect(m_notificationUi.playStopDuaaToolButton, SIGNAL(clicked()), this, SLOT(playStop()));
    
    connect(m_notificationUi.soundSelect, SIGNAL(urlSelected(const KUrl &)), this, SLOT(checkSoundFile(const KUrl &)));
    connect(m_notificationUi.soundFajrSelect, SIGNAL(urlSelected(const KUrl &)), this, SLOT(checkSoundFile(const KUrl &)));
    connect(m_notificationUi.soundDuaaSelect, SIGNAL(urlSelected(const KUrl &)), this, SLOT(checkSoundFile(const KUrl &)));
    
    connect(m_notificationUi.enableSoundCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableVolumeSlider(bool)));
}

// Check if the fileformat is supported by Phonon
bool NotificationForm::checkSoundFile( const KUrl &url )
{
    qDebug() << "Checking filetype";
    
    KUrlRequester *urlSelector = qobject_cast<KUrlRequester *>(sender());

    if(Phonon::BackendCapabilities::availableMimeTypes().contains(KMimeType::findByUrl(url.url())->name()))
    {
        return true;
    }

    KMessageBox::sorry(this, i18n("The selected filetype is not supported by Phonon."), i18n("Unsupported filetype"));
    
    if(urlSelector == m_notificationUi.soundSelect)
    {
        m_notificationUi.soundSelect->setUrl(KUrl(KGlobal::dirs()->findResourceDir("data", "mawakit/") + "mawakit/hayy.ogg"));
        mediaObject->setCurrentSource(m_notificationUi.soundSelect->url());
    }
    else if(urlSelector == m_notificationUi.soundFajrSelect)
    {
        m_notificationUi.soundFajrSelect->setUrl(KUrl(KGlobal::dirs()->findResourceDir("data", "mawakit/") + "mawakit/hayy.ogg"));
        mediaObject->setCurrentSource(m_notificationUi.soundFajrSelect->url());
    }
    else if(urlSelector == m_notificationUi.soundDuaaSelect)
    {
        m_notificationUi.soundDuaaSelect->setUrl(KUrl(KGlobal::dirs()->findResourceDir("data", "mawakit/") + "mawakit/duaa.ogg"));
        mediaObject->setCurrentSource(m_notificationUi.soundDuaaSelect->url());
    }
    
    return false;
}

void NotificationForm::stateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
    switch (newState)
    {
        case Phonon::ErrorState:
            if (mediaObject->errorType() == Phonon::FatalError)
            {
                KMessageBox::sorry(this, i18n("Fatal Error"), i18n("%1", mediaObject->errorString()));
            }
            else
            {
                KMessageBox::sorry(this, i18n("Error"), i18n("%1", mediaObject->errorString()));
            }
            break;
        case Phonon::PlayingState:
            break;
        case Phonon::StoppedState:
            break;
        case Phonon::PausedState:
            break;
        case Phonon::BufferingState:
            break;
        default:
        ;
    }
}

void NotificationForm::playStop()
{
    QToolButton *toolBtn = qobject_cast<QToolButton *>(sender());
    
    // If the state is playing, stop the sound media and reset the icon
    if( mediaObject->state() == Phonon::PlayingState )
    {
        mediaObject->stop();
        toolBtn->setIcon(KIcon("media-playback-start"));
        tmpToolBtn->setIcon(KIcon("media-playback-start"));
        return;
    }
    
    if(toolBtn == m_notificationUi.playStopToolButton)
    {
        mediaObject->setCurrentSource(m_notificationUi.soundSelect->url());
    }
    else if(toolBtn == m_notificationUi.playStopFajrToolButton)
    {
        mediaObject->setCurrentSource(m_notificationUi.soundFajrSelect->url());
    }
    else if(toolBtn == m_notificationUi.playStopDuaaToolButton)
    {
        mediaObject->setCurrentSource(m_notificationUi.soundDuaaSelect->url());
    }
    
    // Play the sound and change the icon
    mediaObject->play();
    toolBtn->setIcon(KIcon("media-playback-stop"));
    tmpToolBtn = toolBtn;
}

void NotificationForm::soundFinished()
{
    tmpToolBtn->setIcon(KIcon("media-playback-start"));
}

void NotificationForm::enableVolumeSlider(bool enabled)
{
    volumeSlider->setEnabled(enabled);
    if(mediaObject->state() == Phonon::PlayingState)
    {
        mediaObject->stop();
        tmpToolBtn->setIcon(KIcon("media-playback-start"));
    }
    
    if(m_notificationUi.enableDuaaCheckBox->isChecked())
    {
        m_notificationUi.soundDuaaSelect->setEnabled(enabled);
        m_notificationUi.playStopDuaaToolButton->setEnabled(enabled);
    }
}

QString NotificationForm::soundFilePath()
{
    return m_notificationUi.soundSelect->url().url();
}

QString NotificationForm::soundFajrFilePath()
{
    return m_notificationUi.soundFajrSelect->url().url();
}

QString NotificationForm::soundDuaaFilePath()
{
    return m_notificationUi.soundDuaaSelect->url().url();
}

bool NotificationForm::athanSoundEnabled()
{
    return m_notificationUi.enableSoundCheckBox->isChecked();
}

bool NotificationForm::duaaSoundEnabled()
{
    return m_notificationUi.enableDuaaCheckBox->isChecked();
}

qreal NotificationForm::soundVolume()
{
    return audioOutput->volume();
}

bool NotificationForm::notifyBeforeAthan()
{
    return m_notificationUi.notifyBeforeCheckBox->isChecked();
}

int NotificationForm::notifyBeforeAthanVal()
{
    return m_notificationUi.notifyBeforeSpinBox->value();
}

bool NotificationForm::notifyAfterAthan()
{
    return m_notificationUi.notifyAfterCheckBox->isChecked();
}

int NotificationForm::notifyAfterAthanVal()
{
    return m_notificationUi.notifyAfterSpinBox->value();
}

bool NotificationForm::stopPlayers()
{
    return m_notificationUi.stopPlayersCheckBox->isChecked();
}

void NotificationForm::setSoundFilePath(QString path)
{
    m_notificationUi.soundSelect->setUrl(KUrl(path));
}

void NotificationForm::setSoundFajrFilePath(QString path)
{
    m_notificationUi.soundFajrSelect->setUrl(KUrl(path));
}

void NotificationForm::setSoundDuaaFilePath(QString path)
{
    m_notificationUi.soundDuaaSelect->setUrl(KUrl(path));
}

void NotificationForm::setAthanSoundEnabled(bool enabled)
{
    m_notificationUi.enableSoundCheckBox->setChecked(enabled);
}

void NotificationForm::setDuaaSoundEnabled(bool enabled)
{
    m_notificationUi.enableDuaaCheckBox->setChecked(enabled);
}

void NotificationForm::setSoundVolume(qreal vol)
{
    audioOutput->setVolume(vol);
}

void NotificationForm::setNotifyBeforeAthan(bool enabled)
{
    m_notificationUi.notifyBeforeCheckBox->setChecked(enabled);
}

void NotificationForm::setNotifyBeforeAthanVal(int val)
{
    m_notificationUi.notifyBeforeSpinBox->setValue(val);
}

void NotificationForm::setNotifyAfterAthan(bool enabled)
{
    m_notificationUi.notifyAfterCheckBox->setChecked(enabled);
}

void NotificationForm::setNotifyAfterAthanVal(int val)
{
    m_notificationUi.notifyAfterSpinBox->setValue(val);
}

void NotificationForm::setStopPlayers(bool enabled)
{
    m_notificationUi.stopPlayersCheckBox->setChecked(enabled);
}
