// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "organismqueue.h"

#include <algorithm>
#include <iostream>
#include "statistics.h"

OrganismQueue *OrganismQueue::getInstance()
{
    static OrganismQueue oq;
    return &oq;
}

void OrganismQueue::add(Organism *organism)
{
    _organisms.push_back(organism);
}
void OrganismQueue::addInterim(Organism *organism)
{
    _organismsInterim.push_back(organism);
}

void OrganismQueue::cycleAll()
{
    for (Organism *o : _organisms)
    {
        //auto start_time = Statistics::get_current_time_fenced();
        o->cycle();
        //auto finish_time = Statistics::get_current_time_fenced();
        //std::cout << o->id() << ": " << Statistics::to_us(finish_time - start_time) << " ms" << std::endl;
    }

    while (!_organismsInterim.empty())
    {
        this->add(_organismsInterim.back());
        _organismsInterim.pop_back();
    }

    cycle_no++;
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
    std::cout << " ******** KILLING ******* " << std::endl;
    std::sort(_organisms.begin(), _organisms.end(), [](const Organism* lhs, const Organism* rhs){ return *lhs < *rhs; });
    constexpr double killOrganismsRatio = 0.5;
    int ratio = static_cast<int>(_organisms.size() * killOrganismsRatio);
    for (; ratio > 0; --ratio) {
        Organism *org = _organisms.back();
        _organisms.pop_back();
        delete org;
    }
}

void OrganismQueue::setDrawIP(bool value) {
    drawIP = value;
}

bool OrganismQueue::getDrawIP() {
    return drawIP;
}
