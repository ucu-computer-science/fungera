//#include "mainwindow.h"
// falls on 12452 cycle with seg fault
#include "memory.h"
#include "memoryview.h"
#include "organism.h"
#include "point.h"
#include "statuspanel.h"
#include "organismqueue.h"

#include <QtWidgets>

#include <string>

#include <iostream>
#include <fstream>
#include <ostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


void run(OrganismQueue *organismQueue, StatusPanel *statusPanel, int snapCycle);

int main(int argc, char *argv[])
{ 
    QApplication a(argc, argv);
    QWidget wgt;

    Memory *m = Memory::getInstance();
    using std::string;
    string fn;
    Point tlp(2500, 2500);
    Point sz;
    StatusPanel *sp;

    MemoryView *mw = MemoryView::getInstance();
    OrganismQueue *oq = OrganismQueue::getInstance();

    /*
     * Command line arguments usage:
     * argv[1] -- int -- mode how to run program: 
     *     0 run from start
     *     1 continue from snapshot
     *
     * argv[2] -- string -- filename of organism gen
     *
     * No command line arguments defaults to loading in "normal" mode from begining ../initial.gen
     */

    bool isRestore = false;

    int snapCycle = 0;     // Num of cycle on which to do a snapshot. 0 means never

    if (argc == 3)
    {
        isRestore = atoi(argv[1]);
        fn = argv[2];
    }
    else if (argc == 2) {
        fn = argv[1];
    }
    else {
        fn = "../initial.gen";
    }


    if (isRestore) {
        // Restore from snapshot
        std::cout << "Restoring...";

        std::ifstream ifs_mem("memory_cache");
        boost::archive::text_iarchive ia_mem(ifs_mem);

        ia_mem >> *m;


        std::ifstream ifs_q("queue_cache");
        boost::archive::text_iarchive ia_q(ifs_q);

        ia_q >> *OrganismQueue::getInstance();

        std::cout << " Done" << std::endl;


        static Organism *org = OrganismQueue::getInstance()->getOrganism();
        org->setActiveColors();
        // OrganismQueue::getInstance()->add(org);

        sp = new StatusPanel(org);
    }
    else {
        sz = m->loadGenome(fn, tlp);

        static Organism org(tlp, sz);
        org.setActiveColors();
        OrganismQueue::getInstance()->add(&org);

        sp = new StatusPanel(&org);
    }


    QPushButton *runBtn = new QPushButton("Run");
    runBtn->connect(runBtn, &QPushButton::clicked, [&](){ run(oq, sp, snapCycle); });

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

void make_snapshot() {
    std::cout << "Serializing...";

    std::ofstream ofs_mem("memory_cache");
    {
        boost::archive::text_oarchive oa_mem(ofs_mem);
        oa_mem << *Memory::getInstance();
    }

    std::ofstream ofs_q("queue_cache");
    {
        boost::archive::text_oarchive oa_q(ofs_q);
        oa_q << *OrganismQueue::getInstance();
    }

    std::cout << " Done" << std::endl;
}

void run(OrganismQueue *organismQueue, StatusPanel *statusPanel, int snapCycle)
{
    unsigned int counter = 0;
    for (;;) {
        organismQueue->cycleAll();
        statusPanel->cycle();
        QCoreApplication::processEvents();

        if (counter == snapCycle && snapCycle != 0) {
            make_snapshot();
        }

        counter++;
    }
}
