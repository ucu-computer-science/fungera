#include "src/statistics.h"
#include "src/organism.h"
#include "src/point.h"
#include "organismqueue.h"
#include <cmath>
#include <map>

#include <iostream>

double Statistics::entropy(OrganismQueue *oq) {
    // TODO: reconsider variable names
    /*
     * commands_freq contains map for all found charactern in the specified position across all organisms
     * TODO: change counters type in loops
     */

    size_t numOrgs = oq->getOrganismsNum();
    std::map<Point, std::map<char, int>> commands_freq; // TODO: rename this shit beacause it is not self explanatory
    for (int i = 0; i < numOrgs; i++)     // Iterate through all organisms
    {
        for (int j = 0; j < oq->getOrganisms()[i]->getSize().x; j ++)
        {
            for (int k = 0; k < oq->getOrganisms()[i]->getSize().y; k++)
            {
                /*
                 * For each point check if it is already inside commands_freq. If not initialize it to empty shit map
                 * The same for map which contains instruction - occurances
                 */
                char instr = oq->getOrganisms()[i]->getInstAtOffsetAbs(Point(j,k));

                if ( commands_freq.find(Point(j,k)) == commands_freq.end() )
                {
                    std::map<char, int> *shit = new std::map<char, int>;
                    commands_freq[Point(j,k)] = *shit;
                }

                if ( commands_freq[Point(j,k)].find(instr) == commands_freq[Point(j,k)].end() )
                {
                    commands_freq[Point(j,k)][instr] = 0;
                }

                commands_freq[Point(j,k)][instr]++;
            }
        }
    }

    std::map<Point, double> per_site_entropies; // but it is not p_ks (p_k * log(p_k) for all )

    for (std::pair<Point, std::map<char, int>> curr_shit : commands_freq)
    {
        // TODO: have a look. max_instr_shit contains instruction value (char). It is not used now but may be of use in future
        Point curr_pnt = curr_shit.first;
        int max_val_shit = 0;       // n_m from diploma
        char max_instr_shit = 0;
        for (std::pair<char, int> curr_shit2 : curr_shit.second)
        {
            if (curr_shit2.second > max_val_shit)
            {
                max_val_shit = curr_shit2.second;
                max_instr_shit = curr_shit2.first;
            }
        }
        double per_site_entropy = (double) max_val_shit/numOrgs;
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

void Statistics::printAllStatistics() {
    std::cout << "Cycle " << OrganismQueue::getInstance()->cycle_no << ". Stats: " << std::endl;

    std::cout << "Num of organisms: " << OrganismQueue::getInstance()->getOrganismsNum() << std::endl;

    std::map<Point, int> sizeStat = this->sizeNOrgs();
    for (std::pair<Point,int> entry : sizeStat)
        std::cout << "  " << entry.first.x << ", " << entry.first.y << ": " << entry.second << std::endl;

    std::cout << "Entropy: " << this->entropy(OrganismQueue::getInstance()) << std::endl;

    std::cout << "Evolution tree:" << std::endl;
    for (std::pair<int, std::pair<int, size_t>> cu_rel: evo_tree)
        std::cout << "  " << cu_rel.second.first << " -> " << cu_rel.first
                  << " (cycle " << cu_rel.second.second << ")" << std::endl;

    std::cout << std::endl;

}
