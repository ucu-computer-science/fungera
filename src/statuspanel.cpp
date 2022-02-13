// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "statuspanel.h"

#include "organismqueue.h"

#include <sstream>
#include <string_view>
#include <unordered_map>

using std::string_view;
StatusPanel::StatusPanel(Organism *organism, QWidget *parent) : QWidget(parent), _organism(organism)
{
    connect(OrganismQueue::getInstance(), &OrganismQueue::organismChanged, this, &StatusPanel::updateOrganism);

    connect(_organism, &Organism::deltaChanged, this, &StatusPanel::updateDelta);
    connect(_organism, &Organism::regChanged, this, &StatusPanel::updateReg);
    connect(_organism, &Organism::pushedToStack, this, &StatusPanel::onPushedToStack);
    connect(_organism, &Organism::popedFromStack, this, &StatusPanel::onPoppedFromStack);

    QGridLayout *gridLayout = new QGridLayout;
    constexpr int PROPERTIES = 19;
    QString labelsTexts[PROPERTIES] = { "Cycle",   "Alive",   "Purges",  "Organism_id", "Errors",
                                        "IP",      "Delta",   "RA",      "RB",          "RC",
                                        "RD",      "Stack_0", "Stack_1", "Stack_2",     "Stack_3",
                                        "Stack_4", "Stack_5", "Stack_6", "Stack_7" };
    QLabel *propertiesValuesLabels[PROPERTIES] = { _cycleNoLbl, _alive,  _purges, m_id_lbl,     _errors,
                                                   _ip,         _delta,  _ra,     _rb,     _rc,
                                                   _rd,         _stack0, _stack1, _stack2, _stack3,
                                                   _stack4,     _stack5, _stack6, _stack7 };
    for (int row = 0; row < PROPERTIES; ++row)
    {
        gridLayout->addWidget(new QLabel(labelsTexts[row]), row, 0);
        gridLayout->addWidget(propertiesValuesLabels[row], row, 1);
    }
    setLayout(gridLayout);
}

void StatusPanel::cycle(size_t curr_cycle)
{
    _cycleNoLbl->setNum(static_cast<int>(curr_cycle)); // Let the warning be just here and not through all the code
    updateIP();
}

void StatusPanel::updateOrganism(Organism *organism)
{
    disconnect(_organism, &Organism::deltaChanged, this, &StatusPanel::updateDelta);
    disconnect(_organism, &Organism::regChanged, this, &StatusPanel::updateReg);
    disconnect(_organism, &Organism::pushedToStack, this, &StatusPanel::onPushedToStack);
    disconnect(_organism, &Organism::popedFromStack, this, &StatusPanel::onPoppedFromStack);

    _organism = organism;

    connect(_organism, &Organism::deltaChanged, this, &StatusPanel::updateDelta);
    connect(_organism, &Organism::regChanged, this, &StatusPanel::updateReg);
    connect(_organism, &Organism::pushedToStack, this, &StatusPanel::onPushedToStack);
    connect(_organism, &Organism::popedFromStack, this, &StatusPanel::onPoppedFromStack);
}

void StatusPanel::updateDelta(Point delta)
{
    QString qstr = _organism->_delta.qstr();
    _delta->setText( delta.qstr() );
}

void StatusPanel::updateReg(char reg)
{
    QLabel *regLbl = _regs.at(reg);
    Point regConts = _organism->_regs.at(reg);
    QString qstr = regConts.qstr();
    regLbl->setText(qstr);
}

void StatusPanel::onPushedToStack()
{
    QString qstr = _organism->_stack.back().qstr();
    _stackLbls[_nextAfterTopIdx]->setText(qstr);
    ++_nextAfterTopIdx;
}

void StatusPanel::onPoppedFromStack(char reg)
{
    --_nextAfterTopIdx;
    _stackLbls[_nextAfterTopIdx]->clear();

    updateReg(reg);
}

void StatusPanel::updateIP()
{
    QString qstr = _organism->_ip.qstr();
    _ip->setText(qstr);
}
