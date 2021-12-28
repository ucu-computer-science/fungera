#include "organismqueue.h"

#include <algorithm>

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

void OrganismQueue::killOrganisms()
{
    std::sort(_organisms.begin(), _organisms.end(), [](const Organism* lhs, const Organism* rhs){ return *lhs > *rhs; });
    constexpr double killOrganismsRatio = 0.5;
    int ratio = static_cast<int>(_organisms.size() * killOrganismsRatio);
    for (; ratio > 0; --ratio) {
        Organism *org = _organisms.back();
        _organisms.pop_back();
        delete org;
    }
}
