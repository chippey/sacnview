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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QObject>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QColor>
#include "deftypes.h"
#include "CID.h"





enum DisplayFormats
    {
        DECIMAL = 0,
        PERCENT = 1,
        HEXADECIMAL = 2,
        TOTAL_NUM_OF_FORMATS = 3
    };

void initializePreferences();

class Preferences
{
public:
    static Preferences *getInstance();
   // Q_PROPERTY(QNetworkInterface networkInterface READ networkInterface WRITE setNetworkInterface NOTIFY networkInterfaceChanged);

    QNetworkInterface networkInterface() const;

    QColor colorForCID(const CID &cid);

    // Preferences access functions here:
    void SetDisplayFormat(unsigned int nDisplayFormat);
    void SetBlindVisualizer (bool bBlindVisualizer);
    void SetNumSecondsOfSacn (int nNumSecondsOfSacn);

    unsigned int GetDisplayFormat();
    bool GetBlindVisualizer();
    unsigned int GetNumSecondsOfSacn();

    QString GetFormattedValue(unsigned int nLevelInDecimal);


public slots:
    void setNetworkInterface(const QNetworkInterface &value);
private:
    Preferences();
    static Preferences *m_instance;
    QNetworkInterface m_interface;
    QHash<CID, QColor> m_cidToColor;

    unsigned int m_nDisplayFormat;
    bool m_bBlindVisualizer;
    unsigned int m_nNumSecondsOfSacn;


};

#endif // PREFERENCES_H
