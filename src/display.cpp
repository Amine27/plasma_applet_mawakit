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


#include "display.h"

#include <QDebug>

DisplayForm::DisplayForm(QWidget *parent) :
        QWidget(parent)
{
    m_displayUi.setupUi(this);
}

QList<bool> DisplayForm::prayersToShow()
{
    QList<bool> prayersToShowList;
    
    prayersToShowList.append(m_displayUi.showFajrCheckBox->isChecked());
    prayersToShowList.append(m_displayUi.showShroukCheckBox->isChecked());
    prayersToShowList.append(m_displayUi.showDuhrCheckBox->isChecked());
    prayersToShowList.append(m_displayUi.showAsrCheckBox->isChecked());
    prayersToShowList.append(m_displayUi.showMaghribCheckBox->isChecked());
    prayersToShowList.append(m_displayUi.showIshaCheckBox->isChecked());
    
    return prayersToShowList;
}

bool DisplayForm::showNextPrayer()
{
    return m_displayUi.showNextRadioButton->isChecked();
}

bool DisplayForm::showCountdown()
{
    return m_displayUi.countdownRadioButton->isChecked();
}

bool DisplayForm::ignoreHour()
{
    return m_displayUi.ignoreHourCheckBox->isChecked();
}

bool DisplayForm::ignoreSecs()
{
    return m_displayUi.ignoreSecsCheckBox->isChecked();
}

bool DisplayForm::showElapsed()
{
    return m_displayUi.showElapsedCheckBox->isChecked();
}

bool DisplayForm::iconEffect()
{
    return m_displayUi.iconEffectCheckBox->isChecked();
}

bool DisplayForm::ampmFormat()
{
    return m_displayUi.ampmCheckBox->isChecked();
}

void DisplayForm::setPrayersToShow(QList<bool> prayersToShowList)
{
    m_displayUi.showFajrCheckBox->setChecked(prayersToShowList.at(0));
    m_displayUi.showShroukCheckBox->setChecked(prayersToShowList.at(1));
    m_displayUi.showDuhrCheckBox->setChecked(prayersToShowList.at(2));
    m_displayUi.showAsrCheckBox->setChecked(prayersToShowList.at(3));
    m_displayUi.showMaghribCheckBox->setChecked(prayersToShowList.at(4));
    m_displayUi.showIshaCheckBox->setChecked(prayersToShowList.at(5));
}

void DisplayForm::setShowNextPrayer(bool enabled)
{
    m_displayUi.showNextRadioButton->setChecked(enabled);
}

void DisplayForm::setShowCountdown(bool enabled)
{
    m_displayUi.countdownRadioButton->setChecked(enabled);
}

void DisplayForm::setIgnoreHour(bool enabled)
{
    m_displayUi.ignoreHourCheckBox->setChecked(enabled);
}

void DisplayForm::setIgnoreSecs(bool enabled)
{
    m_displayUi.ignoreSecsCheckBox->setChecked(enabled);
}

void DisplayForm::setShowElapsed(bool enabled)
{
    m_displayUi.showElapsedCheckBox->setChecked(enabled);
}

void DisplayForm::setIconEffect(bool enabled)
{
    m_displayUi.iconEffectCheckBox->setChecked(enabled);
}

void DisplayForm::setAmpmFormat(bool enabled)
{
    m_displayUi.ampmCheckBox->setChecked(enabled);
}