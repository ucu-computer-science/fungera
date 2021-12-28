#ifndef ORGANISM_H
#define ORGANISM_H

#include "memory.h"
#include "memoryview.h"
#include "point.h"

#include <QtWidgets>

#include <stack>
#include <unordered_map>

// Forward declaration
class OrganismQueue;

class Organism : public QObject
{
    Q_OBJECT

public:
    Organism(Point topLeftSize, Point size);

    ~Organism();

    void cycle();

    void setActiveColors();
    void setInactiveColors();

signals:
    void cellChanged(int, int);
    void colorsChanged(Point, Point);

    // IP changes every cycle anyway
//    void ipChanged(Point);
    void deltaChanged();
    void regChanged(char);
    void errorsChanged();
    void pushedToStack();
    void popedFromStack(char);

    friend bool operator>(const Organism &lhs, const Organism &rhs);

private:
    void nop();
    void setDeltaUp();
    void setDeltaDown();
    void setDeltaRight();
    void setDeltaLeft();
    void findPattern();
    void ifZero();
    void setRegToZero();
    void setRegToOne();
    void decReg();
    void incReg();
    void subRegs();
    void loadInst();
    void writeInst();
    void allocChild();
    void separateChild();
    void pushToStack();
    void popFromStack();

    // TODO: Rename Ip to IP
    Point getIpAtOffset(int offset);
    char getInstAtOffset(int offset);

    void setColors(Qt::GlobalColor frameColor, Qt::GlobalColor internalColor);

    static Memory *_memory;
    static OrganismQueue *_organismQueue;

    using InstImpl = void (Organism::*)();
    static const std::unordered_map<char, InstImpl> _instImpls;

    static const std::unordered_map<char, Point> _opcodes;

    static int _nextID;

    const int _id;

    int _errors = 0;

    const Point _topLeftPos;
    const Point _size;

    Point _ip;
    Point _delta = { 0, 0 };

    std::unordered_map<char, Point> _regs{
        { 'a', {} },
        { 'b', {} },
        { 'c', {} },
        { 'd', {} }
    };

    std::stack<Point> _stack;

    Point _childTopLeftPos;
    Point _childSize;

    friend class StatusPanel;
};

#endif // ORGANISM_H
