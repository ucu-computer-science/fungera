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
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/program_options.hpp>

bool isRunning = true;

void run(OrganismQueue *organismQueue, StatusPanel *statusPanel, unsigned snapCycle);
void cycle(OrganismQueue *, StatusPanel *, size_t);
void restoreMemoryMap();

using namespace boost::program_options;
int main(int argc, char *argv[])
{ 

    srand(1);

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
    bool isCheck = false;
    bool drawIP = false;
    bool isExtractAlone = false;
    bool isExtractInContext = false;
    std::string extractFn = "";
    string log_level = "release";
    int instructionSetIdx = 0;
    unsigned snapCycle = 0;
    unsigned orgSnap = 0;


    options_description desc{"Options"};
    variables_map vm;
    try {
        desc.add_options()
            ("help,h", "Show this help screen")
            ("restore,r", "Restore flag, means that simulation will be restored from snapshots. Ignore it to run simulation from the beggining with single organism with genome, specified with -f")
            ("extract-alone", value<string>(), "Extract organism from organism snapshot and memory map from snapshot to see it on the memory map and run it alone on empty map")
            ("extract-in-context", value<string>(), "Extract organism from organism snapshot and memory map from snapshot to see it on the memory map and run it in context next to organisms it were originally (but all other organisms are dead). If both extract-alone and extract-in-context are specified, extract-alone is prioritized")
            ("hamming-dist", value<std::vector<string>>(), "Determine hamming distance between snapshots of organisms")
            ("instr-set,i", value<int>()->default_value(0), "Instruction set index on which simulation will be ran")
            ("snap-cycle,s", value<int>()->default_value(0), "Snapshot cycle -- on which cycle snapshot will be done. 0 defaults to never")
            ("org-snap", value<int>()->default_value(0), "How often to snap each organism individually (each x cycle)")
            ("ip,p", "Whether to draw Instruction pointer or not")
            ("log-level", value<string>()->default_value("release"), "Possible options: \"debug\", \"release\"")
            ("filename,f", value<string>()->default_value(""), "Name of file that contains genome of an organism");

        positional_options_description hamming_files;
        hamming_files.add("hamming-dist", -1);
        store(command_line_parser(argc, argv).options(desc).positional(hamming_files).run(), vm);
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
            if (vm.count("extract-alone"))
            {
                isExtractAlone = true;
                extractFn = vm["extract-alone"].as<string>();
            }
            if (vm.count("extract-in-context"))
            {
                isExtractInContext = true;
                extractFn = vm["extract-in-context"].as<string>();
            }
            if (vm.count("hamming-dist"))
            {
                std::vector<string> orgs_files = vm["hamming-dist"].as<std::vector<string>>();

                if (orgs_files.size() < 2)
                {
                    std::cerr << "Specified not enough files to perform hamming distance (2 required)";
                    exit(1);
                }
                if (orgs_files.size() > 2)
                {
                    std::cerr << "Specified too many files to perform hamming distance (2 required). Other are ignored";
                }

                std::cout << "Restoring memory map before restoring organisms and calculating hamming distance..." << std::endl;
                restoreMemoryMap();
                std::cout << "  Done" << std::endl;

                static Organism org, an_org;

                std::ifstream org_stream(orgs_files[0]);
                boost::archive::text_iarchive ia_org(org_stream);
                ia_org >> org;

                std::ifstream an_org_stream(orgs_files[1]);
                boost::archive::text_iarchive ia_an_org(an_org_stream);
                ia_an_org >> an_org;

                std::cout << "Hamming distance: " << Statistics::hammingDistance(&org, &an_org) << std::endl;
                std::vector<Point> pointsOfDifference = Statistics::locationsOfDifference(&org, &an_org);
                for (Point pnt : pointsOfDifference)
                    std::cout << pnt.x << ", " << pnt.y << std::endl;

                exit(0);

            }
            if (vm.count("ip"))
                drawIP = true;
            if (vm.count("log-level"))
                if (vm["log-level"].as<string>() == "debug")
                    log_level = "debug";

            instructionSetIdx = vm["instr-set"].as<int>();
            snapCycle = vm["snap-cycle"].as<int>();
            orgSnap = vm["org-snap"].as<int>();
        }
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << std::endl;
        exit(1);
    }

    if (isExtractAlone) {
        std::cout << "Restoring memory map before extracting alone..." << std::endl;
        restoreMemoryMap();
        std::cout << "  Done" << std::endl;


        static Organism interim_pseudo_org;

        std::ifstream org_stream(extractFn);
        boost::archive::text_iarchive ia_org(org_stream);

        ia_org >> interim_pseudo_org;

        std::ofstream extracted_org;
        std::string extracted_fn = std::to_string(interim_pseudo_org.id()) + "_extracted.gen";
        extracted_org.open(extracted_fn);

        Point orgTopLeft = interim_pseudo_org.getTopLeftPos();
        Point orgSize = interim_pseudo_org.getSize();
        for (int i = 0; i < orgSize.x; i++) {
            std::string line = "";
            for (int j = 0; j < orgSize.y; j++) {
                line += m->instAt(orgTopLeft.x+i, orgTopLeft.y+j);
            }
            extracted_org << line << std::endl;
        }
        extracted_org.close();

        m->clear();
        sz = m->loadGenome(extracted_fn, tlp);
        static Organism org(tlp, sz);

        try {
            std::ofstream o_parent("organisms/" + std::to_string(org.id()) + "_" + std::to_string(0));
            {
                boost::archive::text_oarchive oa_parent(o_parent);
                oa_parent << org;
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
        }

        org.setActiveColors();
        OrganismQueue::getInstance()->add(&org);

        sp = new StatusPanel(&org);
    }
    else if (isExtractInContext) {
        std::cout << "Restoring memory map before extracting in context..." << std::endl;
        restoreMemoryMap();
        std::cout << "  Done" << std::endl;

        static Organism interim_pseudo_org;

        std::ifstream org_stream(extractFn);
        boost::archive::text_iarchive ia_org(org_stream);

        ia_org >> interim_pseudo_org;

        std::ofstream extracted_org;
        std::string extracted_fn = std::to_string(interim_pseudo_org.id()) + "_extracted.gen";
        extracted_org.open(extracted_fn);

        Point orgTopLeft = interim_pseudo_org.getTopLeftPos();
        Point orgSize = interim_pseudo_org.getSize();
        for (int i = 0; i < orgSize.x; i++) {
            std::string line = "";
            for (int j = 0; j < orgSize.y; j++) {
                line += m->instAt(orgTopLeft.x+i, orgTopLeft.y+j);
            }
            extracted_org << line << std::endl;
        }
        extracted_org.close();

        tlp = orgTopLeft;
        std::cout << "Organism tlp: " << tlp.x << ", " << tlp.y << std::endl;

        sz = m->loadGenome(extracted_fn, tlp);
        static Organism org(tlp, sz);

        try {
            std::ofstream o_parent("organisms/" + std::to_string(org.id()) + "_" + std::to_string(0));
            {
                boost::archive::text_oarchive oa_parent(o_parent);
                oa_parent << org;
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
        }

        org.setActiveColors();
        OrganismQueue::getInstance()->add(&org);

        sp = new StatusPanel(&org);
    }

    else if (isRestore) {
        // Restore from snapshot
        std::cout << "Restoring..." << std::endl;

        restoreMemoryMap();

        std::ifstream ifs_q("cache/queue");
        boost::archive::text_iarchive ia_q(ifs_q);

        ia_q >> *OrganismQueue::getInstance();

        std::cout << "  Done" << std::endl;


        static Organism *org = OrganismQueue::getInstance()->getOrganism();
        org->setActiveColors();
        // OrganismQueue::getInstance()->add(org);

        sp = new StatusPanel(org);
    }
    else {
        system("mkdir -p cache");
        system("mkdir -p organisms");

        sz = m->loadGenome(fn, tlp);

        static Organism org(tlp, sz);

        try {
            std::ofstream o_parent("organisms/" + std::to_string(org.id()) + "_" + std::to_string(0));
            {
                boost::archive::text_oarchive oa_parent(o_parent);
                oa_parent << org;
            }
        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
        }

        org.setActiveColors();
        OrganismQueue::getInstance()->add(&org);

        sp = new StatusPanel(&org);
    }

    OrganismQueue::getInstance()->setDrawIP(drawIP);
    OrganismQueue::getInstance()->setLogLevel(log_level);
    OrganismQueue::getInstance()->orgSnap = orgSnap;


    QPushButton *runBtn = new QPushButton("Run");
    runBtn->connect(runBtn, &QPushButton::clicked, [=](){ run(oq, sp, snapCycle); });

    QPushButton *startAndStopBtn = new QPushButton("Start/Stop");
    startAndStopBtn->connect(startAndStopBtn, &QPushButton::clicked, [](){ isRunning = !isRunning; });

    QPushButton *cycleBtn = new QPushButton("Cycle");
    cycleBtn->connect(cycleBtn, &QPushButton::clicked, [=](){ cycle(oq, sp, oq->cycle_no); });

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
    horBar->setValue(horBar->maximum()*((double)tlp.y/m->cols()));
    QScrollBar *verBar = mw->verticalScrollBar();
    verBar->setValue(verBar->maximum()*((double)tlp.x/m->rows()));

//    MainWindow w;
//    w.show();
    return a.exec();
}

void make_snapshot() {
    std::cout << "Serializing..." << std::endl;

    std::ofstream ofs_mem("cache/memory");
    {
        boost::archive::text_oarchive oa_mem(ofs_mem);
        oa_mem << *Memory::getInstance();
    }

    std::ofstream ofs_q("cache/queue");
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

        cycle(organismQueue, statusPanel, curr_cycle);

        if (curr_cycle % (10000-1) == 0)
            OrganismQueue::getInstance()->qStat.printAllStatistics();


        if (curr_cycle % 3 == 0)
            Memory::getInstance()->irradiate();

        if (curr_cycle % 10000 == 0 &&
                Memory::getInstance()->isTimeToKill())
            OrganismQueue::getInstance()->killOrganisms();

        if (curr_cycle == snapCycle)
            make_snapshot();
    }
}

// Have no fucking idea how does this work (doesn't interfere with already
// running run()
void cycle(OrganismQueue *organismQueue, StatusPanel *statusPanel, size_t curr_cycle)
{
    organismQueue->cycleAll();
    statusPanel->cycle(curr_cycle);
}

void restoreMemoryMap() {
    std::ifstream ifs_mem("cache/memory");
    boost::archive::text_iarchive ia_mem(ifs_mem);

    ia_mem >> *Memory::getInstance();
}
