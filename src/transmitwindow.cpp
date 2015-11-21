// Copyright (c) 2015 Tom Barthel-Steer, http://www.tomsteer.net
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "transmitwindow.h"
#include "ui_transmitwindow.h"
#include "consts.h"
#include "sacn/ACNShare/deftypes.h"
#include "sacn/ACNShare/ipaddr.h"
#include "sacn/streamcommon.h"
#include "sacn/sacnsender.h"
#include "configureperchanpriodlg.h"
#include <QToolButton>

transmitwindow::transmitwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::transmitwindow), m_sender(0)
{
    ui->setupUi(this);
    memset(m_levels, 0, sizeof(m_levels));
    on_cbPriorityMode_currentIndexChanged(ui->cbPriorityMode->currentIndex());

    ui->sbUniverse->setMinimum(1);
    ui->sbUniverse->setMaximum(MAX_SACN_UNIVERSE);

    ui->sbPriority->setMinimum(MIN_SACN_PRIORITY);
    ui->sbPriority->setMaximum(MAX_SACN_PRIORITY);
    ui->sbPriority->setValue(DEFAULT_SACN_PRIORITY);

    ui->leSourceName->setText(DEFAULT_SOURCE_NAME);

    // Create preset buttons
    QHBoxLayout *layout = new QHBoxLayout();
    for(int i=0; i<PRESET_COUNT; i++)
    {
        QToolButton *button = new QToolButton(this);
        button->setText(QString::number(i+1));
        layout->addWidget(button);
    }
    ui->gbPresets->setLayout(layout);

    // Create faders
    QVBoxLayout *mainLayout = new QVBoxLayout();
    for(int row=0; row<2; row++)
    {
        QHBoxLayout *rowLayout = new QHBoxLayout();
        for(int col=0; col<24; col++)
        {
            QVBoxLayout *faderVb = new QVBoxLayout();
            QSlider *slider = new QSlider(this);
            m_sliders << slider;
            slider->setMinimum(MIN_SACN_LEVEL);
            slider->setMaximum(MAX_SACN_LEVEL);
            connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_sliderMoved(int)));
            QLabel *label = new QLabel(this);
            m_sliderLabels << label;
            label->setText(QString("%1\r\n%2")
                           .arg(row*24 + col + 1)
                           .arg(0)
                           );
            faderVb->addWidget(slider);
            faderVb->addWidget(label);
            rowLayout->addLayout(faderVb);
        }
        mainLayout->addLayout(rowLayout);
        rowLayout->update();
    }
    ui->gbFaders->setLayout(mainLayout);

    // Set up fader start spinbox
    ui->sbFadersStart->setMinimum(1);
    ui->sbFadersStart->setMaximum(MAX_DMX_ADDRESS - 48 + 1);


    mainLayout->update();
    ui->gbFaders->adjustSize();
    adjustSize();


    for(int i=0; i<MAX_DMX_ADDRESS; i++)
        m_perAddressPriorities[i] = DEFAULT_SACN_PRIORITY;

    // Set up slider for channel check
    ui->slChannelCheck->setMinimum(0);
    ui->slChannelCheck->setMaximum(MAX_SACN_LEVEL);
    ui->slChannelCheck->setValue(MAX_SACN_LEVEL);

    QTimer::singleShot(30, Qt::CoarseTimer, this, SLOT(fixSize()));
}

void transmitwindow::fixSize()
{
    // Update the window size
    resize(sizeHint());
}

transmitwindow::~transmitwindow()
{
    if(m_sender)
        delete m_sender;
    delete ui;
}

void transmitwindow::on_sbUniverse_valueChanged(int value)
{
    CIPAddr address;
    GetUniverseAddress(value, address);

    char string[CIPAddr::ADDRSTRINGBYTES];
    CIPAddr::AddrIntoString(address, string, false, false);

    ui->rbMulticast->setText(tr("Multicast to %1").arg(string));
}

void transmitwindow::on_sliderMoved(int value)
{
    int index = m_sliders.indexOf(qobject_cast<QSlider*>(sender()));
    if(index<0) return;

    int address = index + ui->sbFadersStart->value() - 1; // 0-based address

    QLabel *label = m_sliderLabels[index];

    label->setText(QString("%1\r\n%2")
    .arg(address+1)
    .arg(value)
    );

    m_levels[address] = value;

    if(m_sender)
        m_sender->setLevel(address, value);
}

void transmitwindow::on_sbFadersStart_valueChanged(int value)
{
    for(int i=0; i<m_sliders.count(); i++)
    {
        QSlider *slider = m_sliders[i];
        QLabel *label = m_sliderLabels[i];

        slider->blockSignals(true);
        slider->setValue(m_levels[value + i - 1]);
        slider->blockSignals(false);
        label->setText(QString("%1\r\n%2")
                   .arg(i + value)
                   .arg(m_levels[i + value - 1])
                   );
    }
}

void transmitwindow::setUniverseOptsEnabled(bool enabled)
{
    ui->leSourceName->setEnabled(enabled);
    ui->sbUniverse->setEnabled(enabled);
    ui->sbPriority->setEnabled(enabled);
    ui->btnEditPerChan->setEnabled(enabled);
    ui->cbPriorityMode->setEnabled(enabled);
    ui->gbProtocolMode->setEnabled(enabled);
    ui->gbProtocolVersion->setEnabled(enabled);

    if(enabled)
        ui->btnStart->setText(tr("Start"));
    else
        ui->btnStart->setText(tr("Stop"));

}

void transmitwindow::on_btnStart_pressed()
{
    if(!m_sender)
        m_sender = new sACNSentUniverse(ui->sbUniverse->value());

    if(m_sender->isSending())
    {
        m_sender->stopSending();
        setUniverseOptsEnabled(true);
    }
    else
    {
        m_sender->setName(ui->leSourceName->text());
        if(ui->cbPriorityMode->currentIndex() == pmPER_ADDRESS_PRIORITY)
        {
            m_sender->setPriorityMode(pmPER_ADDRESS_PRIORITY);
            m_sender->setPerChannelPriorities(m_perAddressPriorities);
        }
        m_sender->startSending();
        setUniverseOptsEnabled(false);
        for(int i=0; i<sizeof(m_levels); i++)
            m_sender->setLevel(i, m_levels[i]);
    }

}

void transmitwindow::on_btnEditPerChan_pressed()
{
    ConfigurePerChanPrioDlg dlg;
    dlg.setData(m_perAddressPriorities);
    int result = dlg.exec();
    if(result==QDialog::Accepted)
    {
        memcpy(m_perAddressPriorities, dlg.data(), MAX_DMX_ADDRESS);
    }
}

void transmitwindow::on_cbPriorityMode_currentIndexChanged(int index)
{
    if(index==pmPER_ADDRESS_PRIORITY)
    {
        ui->sbPriority->setEnabled(false);
        ui->btnEditPerChan->setEnabled(true);
    }
    else
    {
        ui->sbPriority->setEnabled(true);
        ui->btnEditPerChan->setEnabled(false);
    }
}

void transmitwindow::on_btnCcNext_pressed()
{
    int value = ui->lcdNumber->value();

    value++;
    if(value>MAX_DMX_ADDRESS) return;

    ui->lcdNumber->display(value);
    if(m_sender)
    {
        m_sender->setLevel(value, ui->slChannelCheck->value());
        m_sender->setLevel(value-1, 0);
    }
}

void transmitwindow::on_btnCcPrev_pressed()
{
    int value = ui->lcdNumber->value();

    value--;
    if(value<1) return;
    value--;

    ui->lcdNumber->display(value+1);

    if(m_sender)
    {
        m_sender->setLevel(value, ui->slChannelCheck->value());
        m_sender->setLevel(value+1, 0);
    }
}

void transmitwindow::on_tabWidget_currentChanged(int index)
{
    if(index==tabChannelCheck)
    {
        if(m_sender)
        {
            int value = ui->lcdNumber->value();
            m_sender->setLevel(0, MAX_DMX_ADDRESS-1, 0);
            m_sender->setLevel(value, ui->slChannelCheck->value());
        }
    }

    if(index==tabSliders)
    {
        if(m_sender)
        {
            m_sender->setLevel(0, MAX_DMX_ADDRESS-1, 0);
        }
    }
}


void transmitwindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_PageDown:
        if(ui->tabWidget->currentIndex()==tabChannelCheck)
            on_btnCcPrev_pressed();
        event->accept();
        break;
    case Qt::Key_PageUp:
        if(ui->tabWidget->currentIndex()==tabChannelCheck)
            on_btnCcNext_pressed();
        event->accept();
        break;
    }
    QWidget::keyPressEvent(event);
}
