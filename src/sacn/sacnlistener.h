// Copyright (c) 2015 Electronic Theatre Controls, http://www.etcconnect.com
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

#ifndef SACNLISTENER_H
#define SACNLISTENER_H

#include <QObject>
#include <QUdpSocket>
#include <vector>
#include <QTimer>
#include "streamingacn.h"


struct sACNMergedAddress
{
    sACNMergedAddress() {
        level = -1;
        winningSource = NULL;
    }
    int level;
    sACNSource *winningSource;
    QSet<sACNSource *> otherSources;
};

typedef QList<sACNMergedAddress> sACNMergedSourceList;

class sACNListener : public QObject
{
    Q_OBJECT
public:
    sACNListener(QObject *parent = 0);
    ~sACNListener();

    int universe();
    sACNMergedSourceList mergedLevels() { return m_merged_levels;}
public slots:
    void startReception(int universe);
signals:
    void sourceFound(sACNSource *source);
    void sourceLost(sACNSource *source);
    void sourceChanged(sACNSource *source);
    void levelsChanged();
private slots:
    void readPendingDatagrams();
    void performMerge();
    void checkSourceExpiration();
    void checkSampleExpiration();
private:
    QUdpSocket *m_socket;
    std::vector<sACNSource *> m_sources;
    int m_last_levels[512];
    sACNMergedSourceList m_merged_levels;
    int m_versionSpec;
    int m_universe;
    // The per-source hold last look time
    int m_ssHLL;
    // Are we in the initial sampling state
    bool m_isSampling;
    ttimer m_sampleTimer;
    QTimer *m_initalSampleTimer;
    QTimer *m_mergeTimer;
};


#endif // SACNLISTENER_H
