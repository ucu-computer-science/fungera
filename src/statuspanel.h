#ifndef STATUSPANEL_H
#define STATUSPANEL_H

#include "organism.h"

#include <QtWidgets>

class StatusPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StatusPanel(Organism *organism, QWidget *parent = nullptr);

    void cycle(int curr_cycle);

public slots:
//    void onPurgeHappened(int purged);
//    void onOrganismCreated();

    void updateOrganism(Organism *organism);

//    void updateErrors();
    void updateDelta();
    void updateReg(char reg);
    void onPushedToStack();
    void onPoppedFromStack(char reg);

private:
    void updateIP();

    Organism *_organism = nullptr;

    QLabel *_cycleNoLbl = new QLabel;

    QLabel *_alive = new QLabel;

    QLabel *_purges = new QLabel;

    QLabel *_id = new QLabel;

    QLabel *_errors = new QLabel;

    QLabel *_ip = new QLabel;
    QLabel *_delta = new QLabel;

    QLabel *_ra = new QLabel;
    QLabel *_rb = new QLabel;
    QLabel *_rc = new QLabel;
    QLabel *_rd = new QLabel;
    const std::unordered_map<char, QLabel *> _regs{
        { 'a', _ra },
        { 'b', _rb },
        { 'c', _rc },
        { 'd', _rd }
    };

    QLabel *_stack0 = new QLabel;
    QLabel *_stack1 = new QLabel;
    QLabel *_stack2 = new QLabel;
    QLabel *_stack3 = new QLabel;
    QLabel *_stack4 = new QLabel;
    QLabel *_stack5 = new QLabel;
    QLabel *_stack6 = new QLabel;
    QLabel *_stack7 = new QLabel;
    // TODO: Look into this and possibly rename the variables
    QLabel * const _stackLbls[8] = { _stack0, _stack1, _stack2, _stack3, _stack4, _stack5, _stack6, _stack7 };
    int _nextAfterTopIdx = 0;
};

#endif // STATUSPANEL_H
