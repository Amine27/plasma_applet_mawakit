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


#include "location.h"

#include <kstandarddirs.h>

#include <QDebug>

LocationForm::LocationForm(QWidget *parent) :
        QWidget(parent)
{
    m_locationUi.setupUi(this);
    
    db = new Database;
    m_manualCalc = new ManualCalcForm;
    
    connect(m_locationUi.countriesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCities(int)));
    connect(m_locationUi.citiesComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateInfo(const QString&)));
    connect(m_locationUi.manualCalcButton, SIGNAL(clicked()), this, SLOT(manualCalcForm()));
    
    if (!db->checkDatabase())
    {
        qDebug() << "Unable to Read Database File";
        return;
    }

    if (!db->makeConnection())
    {
        qDebug() << "Unable to establish a database connection, this Application needs SQLITE support.";
        return;
    }
    
    QStringList countryCodeList;
    
    QStringList countriesList = db->getCountriesList(&countryCodeList);
    
    for(int i=0;i < countriesList.count();++i)
    {
        QIcon icon(getPixmapForCountryCode(countryCodeList.at(i)));
        m_locationUi.countriesComboBox->addItem(icon, countriesList.at(i), i+1);
    }
}

QPixmap LocationForm::getPixmapForCountryCode(const QString & countryCode)
{
    if( countryCode.isEmpty() )
        return NULL;

    QString flag;
    QPixmap pm = NULL;
    
    flag = KStandardDirs::locate("locale", QString("l10n/%1/flag.png").arg(countryCode));
    if( !flag.isEmpty() )
    {
        pm = QPixmap(flag);
    }
    
    return pm;
}

void LocationForm::updateCities(int index)
{
    m_locationUi.citiesComboBox->clear();
    int countryNo = m_locationUi.countriesComboBox->itemData(index).toInt();
    QStringList citiesList = db->getCitiesList(countryNo);
    
    m_locationUi.citiesComboBox->addItems(citiesList);
}

void LocationForm::updateInfo(QString cityName)
{
    QStringList infoList = db->getInfo(cityName);
    
    if(infoList.size() == 4)
    {
        m_locationUi.latitudeLineEdit->setText(QString::number(infoList.at(0).toDouble()/10000));
        m_locationUi.longitudeLineEdit->setText(QString::number(infoList.at(1).toDouble()/10000));
        m_locationUi.timeZoneSpinBox->setValue(infoList.at(2).toDouble()/100);
        (infoList.at(3).toInt() == 1) ? m_locationUi.dayLightCheckBox->setChecked(true) :
                                       m_locationUi.dayLightCheckBox->setChecked(false);
    }
}

QString LocationForm::country() const
{
    return m_locationUi.countriesComboBox->currentText();
}

QString LocationForm::city() const
{
    return m_locationUi.citiesComboBox->currentText();
}

double LocationForm::longitude() const
{
    return m_locationUi.longitudeLineEdit->text().toDouble();
}

double LocationForm::latitude() const
{
    return m_locationUi.latitudeLineEdit->text().toDouble();
}

double LocationForm::timeZone() const
{
    return m_locationUi.timeZoneSpinBox->value();
}

int LocationForm::countryNumber() const
{
    return m_locationUi.countriesComboBox->currentIndex();
}

int LocationForm::cityNumber() const
{
    return  m_locationUi.citiesComboBox->currentIndex();
}

int LocationForm::calcMethodIndex() const
{
    return m_locationUi.calcMethodComboBox->currentIndex();
}

int LocationForm::calcMethod() const
{
    int calcMethod = PrayerTimes::MWL;
    int index = m_locationUi.calcMethodComboBox->currentIndex();
    
    switch(index)
    {
        case 0:
            calcMethod = PrayerTimes::Karachi;
            break;
        case 1:
            calcMethod = PrayerTimes::ISNA;
            break;         
        case 2:
            calcMethod = PrayerTimes::MWL;
            break;
        case 3:
            calcMethod = PrayerTimes::Makkah;
            break;
        case 4:
            calcMethod = PrayerTimes::Egypt;
            break;
		case 5:
            calcMethod = PrayerTimes::UOIF;
            break;
		case 6:
            calcMethod = PrayerTimes::JAKIM;
            break;
		case 7:
            calcMethod = PrayerTimes::Fazilet;
            break;
		case 8:
            calcMethod = PrayerTimes::TPRA;
            break;
	}
    
    return calcMethod;
}

int LocationForm::asrJuristicIndex() const
{
     return m_locationUi.asrJuristicComboBox->currentIndex();
}

int LocationForm::asrJuristic() const
{
    if(m_locationUi.asrJuristicComboBox->currentIndex() == 0 )
        return PrayerTimes::Shafii;
    else
        return PrayerTimes::Hanafi;
}

int LocationForm::highLatIndex() const
{
     return m_locationUi.highLatComboBox->currentIndex();
}

int LocationForm::highLat() const
{
    int highLat = PrayerTimes::MWL;
    int index = m_locationUi.highLatComboBox->currentIndex();
    
    switch(index)
    {
        case 0:
            highLat = PrayerTimes::None;
            break;
        case 1:
            highLat = PrayerTimes::MidNight;
            break;
        case 2:
            highLat = PrayerTimes::OneSeventh;
            break;
        case 3:
            highLat = PrayerTimes::AngleBased;
            break;
    }
    
    return highLat;
}

bool LocationForm::dayLight() const
{
    return m_locationUi.dayLightCheckBox->isChecked();
}

QStringList LocationForm::manualTimeList() const
{    
    return m_manualTimeList;
}

QList<bool> LocationForm::fixedTimeList() const
{
    return m_fixedTimeList;
}

bool LocationForm::manualCalculation() const
{
    return m_manualCalculation;
}

void LocationForm::setCurrentCountry(int countryNumber)
{
    m_locationUi.countriesComboBox->setCurrentIndex(countryNumber);
}

void LocationForm::setCurrentCity(int cityNumber)
{
    m_locationUi.citiesComboBox->setCurrentIndex(cityNumber);
}

void LocationForm::setLongitude(double longitude)
{
    m_locationUi.longitudeLineEdit->setText(QString::number(longitude));
}

void LocationForm::setLatitude(double latitude)
{
    m_locationUi.latitudeLineEdit->setText(QString::number(latitude));
}

void LocationForm::setTimeZone(double timeZone)
{
    m_locationUi.timeZoneSpinBox->setValue(timeZone);
}

void LocationForm::setCalcMethod(int calcMethodIndex)
{
    m_locationUi.calcMethodComboBox->setCurrentIndex(calcMethodIndex);
}

void LocationForm::setAsrJuristic(int asrJuristicIndex)
{
    m_locationUi.asrJuristicComboBox->setCurrentIndex(asrJuristicIndex);
}

void LocationForm::setHighLat(int highLat)
{
    m_locationUi.highLatComboBox->setCurrentIndex(highLat);
}

void LocationForm::setDayLight(bool dayLight)
{
    m_locationUi.dayLightCheckBox->setChecked(dayLight);
}

void LocationForm::setManualCalculation(bool manualCalc)
{
    m_manualCalculation = manualCalc;
}

void LocationForm::setManualTimeList(QStringList manualTimeList)
{
    m_manualTimeList = manualTimeList;
}

void LocationForm::setFixedTimeList(QList<bool> fixedTimeList)
{
    m_fixedTimeList = fixedTimeList;
}

void LocationForm::manualCalcForm()
{
    m_manualCalc->setManualCalc(m_manualCalculation);
    m_manualCalc->setManualTimeList(m_manualTimeList);
    m_manualCalc->setFixedTimeList(m_fixedTimeList);
    
    if(m_manualCalc->exec())
    {   
        m_manualCalculation = m_manualCalc->manualCalc();
        
        if(m_manualCalculation)
        {     
            m_manualTimeList = m_manualCalc->manualTimeList();
            m_fixedTimeList = m_manualCalc->fixedTimeList();
        }
        
    }
}
