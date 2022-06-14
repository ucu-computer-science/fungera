#include "src/statistics.h"
#include "src/organism.h"
#include "src/point.h"
#include "organismqueue.h"
#include <cmath>
#include <map>

#include <iostream>
#include <vector>

double Statistics::entropy(OrganismQueue *oq) {
    // TODO: reconsider variable names
    /*
     * commands_freq contains map for all found charactern in the specified position across all organisms
     * TODO: change counters type in loops
     */

    size_t numOrgs = oq->getOrganismsNum();
    std::map<Point, std::map<char, int>> commands_freq; // TODO: rename this variable beacause it is not self explanatory
    for (int i = 0; i < numOrgs; i++)     // Iterate through all organisms
    {
        Organism *cu_org = oq->getOrganisms()[i];
        Point cu_org_size = cu_org->getSize();
        for (int j = 0; j < cu_org_size.x; j++)
            for (int k = 0; k < cu_org_size.y; k++)
            {
                /*
                 * For each point check if it is already inside commands_freq. If not initialize it to empty map
                 * The same for map which contains instruction - occurances
                 */
                Point cu_point = Point(j,k);
                char instr = cu_org->getInstAtOffsetAbs(Point(j,k));

                if ( commands_freq.find(cu_point) == commands_freq.end() )
                {
                    std::map<char, int> *cu_entry = new std::map<char, int>;
                    (*cu_entry)[instr] = 0;
                    commands_freq[cu_point] = *cu_entry;
                }

                if ( commands_freq[cu_point].find(instr) == commands_freq[cu_point].end() )
                    commands_freq[cu_point][instr] = 0;

                commands_freq[cu_point][instr]++;
            }
    }

    std::map<Point, double> per_site_entropies; // but it is not p_ks (p_k * log(p_k) for all )

    for (std::pair<Point, std::map<char, int>> curr_entry : commands_freq)
    {
        // TODO: have a look. max_instr_i contains instruction value (char). It is not used now but may be of use in future
        Point curr_pnt = curr_entry.first;
        int max_val = 0;       // n_m from diploma
        char max_instr_i = 0;
        for (std::pair<char, int> curr_subentry : curr_entry.second)
        {
            if (curr_subentry.second > max_val)
            {
                max_val = curr_subentry.second;
                max_instr_i = curr_subentry.first;
            }
        }
        double per_site_entropy = (double) max_val/numOrgs;
        per_site_entropies[curr_pnt] = per_site_entropy*std::log(per_site_entropy);
    }

    double entropy = 0;
    for (std::pair<Point, double> curr_pse : per_site_entropies)
    {
        entropy += curr_pse.second;
    }
    entropy = -entropy;

    return entropy;
}

std::map<Point, int> Statistics::sizeNOrgs() {
    std::map<Point, int> sizeNOrgsMap;
    for (Organism *curr_org : OrganismQueue::getInstance()->getOrganisms())
    {
        if (sizeNOrgsMap.find(curr_org->getSize()) == sizeNOrgsMap.end())
            sizeNOrgsMap[curr_org->getSize()] = 0;

        sizeNOrgsMap[curr_org->getSize()]++;
    }
    return sizeNOrgsMap;
}

void Statistics::addRelation(int child_id, int parent_id, size_t cycle_no) {
    std::pair<int, size_t> parent_n_cycle(parent_id, cycle_no);
    evo_tree[child_id] = parent_n_cycle;
    int i = 0;
    i++;
}

unsigned Statistics::hammingDistance(Organism *org, Organism *an_org) {
    return Statistics::locationsOfDifference(org, an_org).size();
}

/*
 * Returns vector points (absolute) of the first organism (org), which differs from this same relative points or another organism (an_org)
 */
std::vector<Point> Statistics::locationsOfDifference(Organism *org, Organism *an_org) {
    Memory *m = Memory::getInstance();
    std::vector<Point> locsOfDiff;
    for (int i = 0; i < org->getSize().x; i++) {
        for (int j = 0; j < org->getSize().y; j++) {
            char orgInst = m->instAt(org->getTopLeftPos().x+i, org->getTopLeftPos().y+j);
            char an_orgInst = m->instAt(an_org->getTopLeftPos().x+i, an_org->getTopLeftPos().y+j);
            if (orgInst != an_orgInst) {
                locsOfDiff.push_back(Point(org->getTopLeftPos().x+i, org->getTopLeftPos().y+j));
            }
        }
    }
    return locsOfDiff;
}

void Statistics::printAllStatistics() {
    std::cout << "Cycle " << OrganismQueue::getInstance()->cycle_no << ". Stats: " << std::endl;

    std::cout << "Num of organisms: " << OrganismQueue::getInstance()->getOrganismsNum() << std::endl;

    std::map<Point, int> sizeStat = this->sizeNOrgs();
    for (std::pair<Point,int> entry : sizeStat)
        std::cout << "  " << entry.first.x << ", " << entry.first.y << ": " << entry.second << std::endl;

    std::cout << "Entropy: " << this->entropy(OrganismQueue::getInstance()) << std::endl;

    std::cout << "Evolution tree:" << std::endl;
    for (std::pair<int, std::pair<int, size_t>> cu_rel: evo_tree)
    {
        Point parent_size(-1, -1);
        if (OrganismQueue::getInstance()->getOrganism(cu_rel.second.first) != NULL)
        {
            parent_size = OrganismQueue::getInstance()->getOrganism(cu_rel.second.first)->getSize();
        }
        Point child_size(-1, -1);
        if (OrganismQueue::getInstance()->getOrganism(cu_rel.second.first) != NULL)
        {
            child_size = OrganismQueue::getInstance()->getOrganism(cu_rel.first)->getSize();
        }
        std::cout << "  " << cu_rel.second.first << "(" << parent_size.x << ", " << parent_size.y << ")"
            << " -> " << cu_rel.first << "(" << child_size.x << ", " << child_size.y << ")"
            << " (cycle " << cu_rel.second.second << ")" << std::endl;
    }

    std::cout << std::endl;
}
