#ifndef ORGANISMQUEUE_H
#define ORGANISMQUEUE_H

#include "organism.h"
#include "src/statistics.h"

#include <QtWidgets>

#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class OrganismQueue : public QObject
{
    Q_OBJECT

public:
    size_t cycle_no = 1;
    OrganismQueue(OrganismQueue &) = delete;
    OrganismQueue &operator=(OrganismQueue &) = delete;

    static OrganismQueue *getInstance();

    void add(Organism *organism);
    void addInterim(Organism *organism);

    void cycleAll();

    Organism * getOrganism() {
        return _organisms[0];
    }
    Organism * getOrganism(int id) {
        for (Organism *org : _organisms)
            if (org->id() == id)
                return _organisms[id];
        return NULL;
    }

    size_t getOrganismsNum() {
        return _organisms.size();
    }

    std::vector<Organism*> getOrganisms() {
        return _organisms;
    }
    void killOrganisms();

    Statistics qStat = Statistics();

    // TODO: reconsider whether these method and field should be in OrganismQueue or in some other class or check this in some other way 

    bool getDrawIP();
    void setDrawIP(bool value);

    std::string getLogLevel() {
        return logLevel;
    }
    void setLogLevel(std::string log_level) {
        logLevel = log_level;
    }

    bool drawIP = false;
    std::string logLevel = "release";
    unsigned orgSnap = 0;

public slots:
    void selectNextOrg();
    void selectPrevOrg();

signals:
    void organismChanged(Organism *);

protected:
    OrganismQueue() = default;

private:
    friend class boost::serialization::access;

    std::vector<Organism *> _organisms;
    std::vector<Organism *> _organismsInterim;

    int _activeOrgIdx = 0;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & _organisms;
        ar & _activeOrgIdx;
        ar & cycle_no;
    }

};

#endif // ORGANISMQUEUE_H
