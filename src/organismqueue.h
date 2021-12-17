#ifndef ORGANISMQUEUE_H
#define ORGANISMQUEUE_H

#include "organism.h"

#include <QtWidgets>

#include <vector>

class OrganismQueue : public QObject
{
    Q_OBJECT

public:
    OrganismQueue(OrganismQueue &) = delete;
    OrganismQueue &operator=(OrganismQueue &) = delete;

    static OrganismQueue *getInstance();

    void add(Organism *organism);

    void cycleAll();

public slots:
    void selectNextOrg();
    void selectPrevOrg();

signals:
    void organismChanged(Organism *);

protected:
    OrganismQueue() = default;

private:
    std::vector<Organism *> _organisms;

    int _activeOrgIdx = 0;
};

#endif // ORGANISMQUEUE_H
