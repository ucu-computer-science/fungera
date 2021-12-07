//#include "mainwindow.h"
#include "memory.h"
#include "memoryview.h"
#include "organism.h"
#include "point.h"
#include "statuspanel.h"
#include "organismqueue.h"

#include <QtWidgets>

#include <string>

void run(OrganismQueue *organismQueue, StatusPanel *statusPanel);

int main(int argc, char *argv[])
{ 
    QApplication a(argc, argv);
    QWidget wgt;

    Memory *m = Memory::getInstance();
    using std::string;
    string fn = "../initial.gen";
    Point tlp(2500, 2500);
    Point sz = m->loadGenome(fn, tlp);

    Organism org(tlp, sz);
    org.setActiveColors();
    OrganismQueue::getInstance()->add(&org);

    StatusPanel *sp = new StatusPanel(&org);

    MemoryView *mw = MemoryView::getInstance();

    OrganismQueue *oq = OrganismQueue::getInstance();

    QPushButton *runBtn = new QPushButton("Run");
    runBtn->connect(runBtn, &QPushButton::clicked, [&](){ run(oq, sp); });

    QPushButton *nextBtn = new QPushButton("Next");
    nextBtn->connect(nextBtn, &QPushButton::clicked, oq, &OrganismQueue::selectNextOrg);

    QPushButton *prevBtn = new QPushButton("Prev");
    prevBtn->connect(prevBtn, &QPushButton::clicked, oq, &OrganismQueue::selectPrevOrg);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(sp);
    hBoxLayout->addWidget(mw);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addWidget(runBtn);
    vBoxLayout->addWidget(nextBtn);
    vBoxLayout->addWidget(prevBtn);
    wgt.setLayout(vBoxLayout);

    wgt.show();

    QScrollBar *horBar = mw->horizontalScrollBar();
    horBar->setValue(horBar->maximum()/2);
    QScrollBar *verBar = mw->verticalScrollBar();
    verBar->setValue(verBar->maximum()/2);

//    MainWindow w;
//    w.show();
    return a.exec();
}

void run(OrganismQueue *organismQueue, StatusPanel *statusPanel)
{
    for (;;) {
        organismQueue->cycleAll();
        statusPanel->cycle();
        QCoreApplication::processEvents();
    }
}
