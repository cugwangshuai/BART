#ifndef SCHEDULER_DLG_H
#define SCHEDULER_DLG_H

#include <QtGui/QToolBar>

#include "ui_SchedulerDlg.h"


class SchedulerDlg : public QDialog, private Ui::SchedulerDlg
{
    Q_OBJECT

    // types

    // static data members

    static const auto smSCHEDULER_TIMER_INTERVAL = 30000;
    static const auto smCHECK_CONFIGFILE_TIMER_INTERVAL = 5000;

    // instance data members

    bool mIsDialog = false;

public:
    explicit SchedulerDlg(QWidget *parent = 0);

    void createMenuBar();

    static void functionWithCodeToMoveToApplicationClassConstructor();

public slots:
    void action_ViewConfig_slot();
    void action_UserManual_slot();
    void action_About_slot();
};

#endif // SCHEDULER_DLG_H