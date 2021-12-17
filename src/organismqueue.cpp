#include "organismqueue.h"

OrganismQueue *OrganismQueue::getInstance()
{
    static OrganismQueue oq;
    return &oq;
}

void OrganismQueue::add(Organism *organism)
{
    _organisms.push_back(organism);
}

void OrganismQueue::cycleAll()
{
    for (Organism *o : _organisms)
        o->cycle();
}

void OrganismQueue::selectNextOrg()
{
    _organisms[_activeOrgIdx]->setInactiveColors();
    _activeOrgIdx = (_activeOrgIdx + 1) % _organisms.size();
    _organisms[_activeOrgIdx]->setActiveColors();
    emit organismChanged(_organisms[_activeOrgIdx]);
}

void OrganismQueue::selectPrevOrg()
{
    _organisms[_activeOrgIdx]->setInactiveColors();
    _activeOrgIdx = (((_activeOrgIdx - 1) % _organisms.size()) + _organisms.size())
                    % _organisms.size();
    _organisms[_activeOrgIdx]->setActiveColors();
    emit organismChanged(_organisms[_activeOrgIdx]);
}
