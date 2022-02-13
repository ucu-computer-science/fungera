#ifndef ORGANISM_H
#define ORGANISM_H

#include "memory.h"
#include "memoryview.h"
#include "point.h"

#include <QtWidgets>

#include <stack>
#include <unordered_map>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>

// Forward declaration
class OrganismQueue;

class Organism : public QObject
{
    Q_OBJECT

public:
    Organism(Point topLeftSize, Point size);
    Organism();

    int id() {
        return m_id;
    }

    ~Organism();

    void cycle();

    void setActiveColors();
    void setInactiveColors();

    char getInstAtOffsetAbs(Point offset);

    Point getSize() {
        return _size;
    }
    Point getTopLeftPos() {
        return _topLeftPos;
    }

    size_t last_snap_cycle = 0;
    void self_serialize();

    Point getIP() { return _ip; }
    Point getDelta() { return _delta; }

    void setIP(Point newIP) { _ip = newIP; }
    void setDelta(Point newDelta) { _delta = newDelta; }

    size_t getErrors() const { return m_errors; }

signals:
    void cellChanged(int, int);
    void colorsChanged(Point, Point);

    // IP changes every cycle anyway
    // void ipChanged(Point);
    void deltaChanged();
    void regChanged(char);
    void errorsChanged();
    void pushedToStack();
    void popedFromStack(char);

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
    void jump();
    void random();
    void jumpInRange();
    void randomDelta();

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

    int m_id; // Name m_id_lbl can lead to conflict with MOC generated code.
              // TODO: rename all _XXX то m_XXX.

    size_t m_errors = 0;

    Point _topLeftPos;
    Point _size;

    Point _ip;
    Point _delta = { 0, 0 };

    std::unordered_map<char, Point> _regs{
        { 'a', {} },
        { 'b', {} },
        { 'c', {} },
        { 'd', {} }
    };

    std::vector<Point> _stack;

    Point _childTopLeftPos;
    Point _childSize;

    friend class StatusPanel;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int ) // version
    {
        ar & _topLeftPos;
        ar & _size;
        ar & _ip;
        ar &m_id;
        ar & _nextID;
        ar & _delta;
        ar &m_errors;
        ar & _childTopLeftPos;
        ar & _childSize;
        ar & _stack;
        ar & _regs;
    }
};

#endif // ORGANISM_H
