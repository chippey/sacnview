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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>



namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

private slots:

    void on_DecimalDisplayFormat_toggled(bool checked);
    void on_PercentDisplayFormat_toggled(bool checked);
    void on_HexDisplayFormat_toggled(bool checked);

    void on_checkBox_toggled(bool checked);

    void on_NumOfSecOfSacn_valueChanged(int arg1);
    void on_NumOfMinOfSacn_valueChanged(int arg1);
    void on_NumOfHoursOfSacn_valueChanged(int arg1);

    void on_buttonBox_accepted();

private:
    Ui::PreferencesDialog *ui;

    void SetFieldsToCurrentState();
    void ConvertHourMinSecToSec();
    void refreshTransmitTimeFields();

    void SetMemberVariablesFromPreferences();
    void SaveMemberVariablesToPreferences();

    int m_nHour;
    int m_nMin;
    int m_nSec;

    int m_PrefDialog_nTotalNumOfSec;
    unsigned int m_PrefDialog_nDisplayFormat;
    bool m_PrefDialog_bBlindVisualizer;



};

#endif // PREFERENCESDIALOG_H
