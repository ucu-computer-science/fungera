//#include "mainwindow.h"
#include "memory.h"
#include "memoryview.h"
#include "organism.h"
#include "point.h"
#include "src/statistics.h"
#include "statuspanel.h"
#include "organismqueue.h"

#include <QtWidgets>

#include <string>

#include <iostream>
#include <fstream>
#include <ostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/program_options.hpp>

bool isRunning = true;

void run(OrganismQueue *organismQueue, StatusPanel *statusPanel, unsigned snapCycle);
void cycle(OrganismQueue *, StatusPanel *);

using namespace boost::program_options;

int main(int argc, char *argv[])
{ 
    QApplication a(argc, argv);
    QWidget wgt;

    Memory *m = Memory::getInstance();
    using std::string;
    string fn = "";
    Point tlp(2500, 2500);
    Point sz;
    StatusPanel *sp;

    MemoryView *mw = MemoryView::getInstance();
    OrganismQueue *oq = OrganismQueue::getInstance();

    /*
     * Command line arguments usage:
     *     Run this program with -h flag to see usgae
     */


    bool isRestore = false;
    int instructionSetIdx = 0;
    unsigned snapCycle = 0;


    options_description desc{"Options"};
    variables_map vm;
    try {
        desc.add_options()
            ("help,h", "Show this help screen")
            ("restore,r", "Restore flag, means that simulation will be restored from snapshots. Ignore it to run simulation from the beggining with single organism with genome, specified with -f")
            ("instr-set,i", value<int>()->default_value(0), "Instruction set index on which simulation will be ran")
            ("snap-cycle,s", value<int>()->default_value(0), "Snapshot cycle -- on which cycle snapshot will be done. 0 defaults to never")
            ("filename,f", value<string>()->default_value(""), "Name of file that contains genome of an organism");

        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            exit(0);
        }
        else {
            if (vm.count("filename"))
                fn = vm["filename"].as<string>();
            if (vm.count("restore"))
                isRestore = true;

            instructionSetIdx = vm["instr-set"].as<int>();
            snapCycle = vm["snap-cycle"].as<int>();
        }
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(1);
    }


    if (isRestore) {
        // Restore from snapshot
        std::cout << "Restoring..." << std::endl;

        std::ifstream ifs_mem("memory_cache");
        boost::archive::text_iarchive ia_mem(ifs_mem);

        ia_mem >> *m;


        std::ifstream ifs_q("queue_cache");
        boost::archive::text_iarchive ia_q(ifs_q);

        ia_q >> *OrganismQueue::getInstance();

        std::cout << "  Done" << std::endl;


        static Organism *org = OrganismQueue::getInstance()->getOrganism();
        org->setActiveColors();
        // OrganismQueue::getInstance()->add(org);

        sp = new StatusPanel(org);
    }
    else {
        sz = m->loadGenome("../gens/initial_simple_improved.gen", tlp);

        // std::cout << sz.x << " " << sz.y << std::endl;

        static Organism org(tlp, sz);
        org.setActiveColors();
        OrganismQueue::getInstance()->add(&org);

        sp = new StatusPanel(&org);
    }


    QPushButton *runBtn = new QPushButton("Run");
    runBtn->connect(runBtn, &QPushButton::clicked, [=](){ run(oq, sp, snapCycle); });

    QPushButton *startAndStopBtn = new QPushButton("Start/Stop");
    startAndStopBtn->connect(startAndStopBtn, &QPushButton::clicked, [](){ isRunning = !isRunning; });

    QPushButton *cycleBtn = new QPushButton("Cycle");
    cycleBtn->connect(cycleBtn, &QPushButton::clicked, [=](){ cycle(oq, sp); });

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
    vBoxLayout->addWidget(startAndStopBtn);
    vBoxLayout->addWidget(cycleBtn);
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
    std::cout << "Serializing..." << std::endl;

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

    std::cout << "  Done" << std::endl;
}

void run(OrganismQueue *organismQueue, StatusPanel *statusPanel, unsigned snapCycle)
{
    for (;;) {
        size_t curr_cycle = OrganismQueue::getInstance()->cycle_no;
        QCoreApplication::processEvents();

        if (!isRunning)
            continue;

        cycle(organismQueue, statusPanel);

        if (curr_cycle % 1000 == 0)
            OrganismQueue::getInstance()->qStat.printAllStatistics();


        if (curr_cycle % 1 == 5)
            for (int j = 0; j < 1; j++)
                Memory::getInstance()->irradiate();

        if (curr_cycle % 10000 == 0 && Memory::getInstance()->isTimeToKill())
            OrganismQueue::getInstance()->killOrganisms();

        if (curr_cycle == snapCycle)
            make_snapshot();
    }
}

// Have no fucking idea how does this work (doesn't interfere with already
// running run()
void cycle(OrganismQueue *organismQueue, StatusPanel *statusPanel)
{
    organismQueue->cycleAll();
    statusPanel->cycle();
}
