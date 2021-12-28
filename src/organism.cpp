#include "organism.h"

#include "memoryview.h"
#include "organismqueue.h"

Organism::Organism(Point topLeftPos, Point size) : _id(_nextID), _topLeftPos(topLeftPos), _size(size), _ip(topLeftPos)
{
    connect(this, &Organism::cellChanged, MemoryView::getInstance(), &MemoryView::paintCellIfVisible);
    connect(this, &Organism::colorsChanged, MemoryView::getInstance(), &MemoryView::paintArea);

    setInactiveColors();

    ++_nextID;
}

Organism::~Organism()
{
    Memory &mem = *Memory::getInstance();
    mem.freeArea(_topLeftPos, _size);
    if (_childSize != Point(0, 0)) // Why does { 0, 0 } cannot be used?
        mem.freeArea(_childTopLeftPos, _childSize);
}

void Organism::cycle()
{
    char inst = _memory->instAt(_ip.x, _ip.y);
    try {
        (this->*_instImpls.at(inst))();
    } catch (std::runtime_error &) { // Catch any type of error
        ++_errors;
    }
    _ip += _delta;
}

void Organism::setActiveColors()
{
    setColors(Qt::cyan, Qt::blue);
}

void Organism::setInactiveColors()
{
    setColors(Qt::gray, Qt::lightGray);
}

void Organism::nop() {}

void Organism::setDeltaUp()
{
    _delta = { -1, 0 };
    emit deltaChanged();
}

void Organism::setDeltaDown()
{
    _delta = { 1, 0 };
    emit deltaChanged();
}

void Organism::setDeltaRight()
{
    _delta = { 0, 1 };
    emit deltaChanged();
}

void Organism::setDeltaLeft()
{
    _delta = { 0, -1 };
    emit deltaChanged();
}

void Organism::findPattern()
{
    std::vector<char> pattern;
    int i = 2;
    int maxI = std::max(_size.x, _size.y);
    for (; i < maxI; ++i) {
        char inst = getInstAtOffset(i);
        if (inst == '.')
            pattern.push_back(':');
        else if (inst == ':')
            pattern.push_back('.');
        else
            break;
    }

    ++i;
    int ctr = 0;
    for (; i < maxI; ++i) {
        if (getInstAtOffset(i) == pattern[ctr]) {
            ++ctr;
        } else {
            ctr = 0;
        }
        if (ctr == pattern.size()) {
            char reg = getInstAtOffset(1);
            _regs.at(reg) = getIpAtOffset(i);
            emit regChanged(reg);
            break;
        }
    }
}

void Organism::ifZero()
{
    char inst1 = getInstAtOffset(1);
    if (inst1 == 'x') {
        char inst2 = getInstAtOffset(2);
        _ip = getIpAtOffset( _regs.at(inst2).x ? 3 : 2 );
    } else if (inst1 == 'y') {
        char inst2 = getInstAtOffset(2);
        _ip = getIpAtOffset( _regs.at(inst2).y ? 3 : 2 );
    } else {
        Point regConts = _regs.at(inst1);
        // TODO: Implement implicit conversion to bool for Point
        _ip = getIpAtOffset( (regConts.x || regConts.y) ? 2 : 1 );
    }
}

void Organism::setRegToZero()
{
    char reg = getInstAtOffset(1);
    _regs.at(reg) = { 0, 0 };
    emit regChanged(reg);
}

void Organism::setRegToOne()
{
    char reg = getInstAtOffset(1);
    _regs.at(reg) = { 1, 1 };
    emit regChanged(reg);
}

void Organism::decReg()
{
    char inst = getInstAtOffset(1);
    if (inst == 'x') {
        char reg = getInstAtOffset(2);
        --_regs.at(reg).x;
        emit regChanged(reg);
    } else if (inst == 'y') {
        char reg = getInstAtOffset(2);
        --_regs.at(reg).y;
        emit regChanged(reg);
    } else {
        --_regs.at(inst);
        emit regChanged(inst);
    }
}

void Organism::incReg()
{
    char inst = getInstAtOffset(1);
    if (inst == 'x') {
        char reg = getInstAtOffset(2);
        ++_regs.at(reg).x;
        emit regChanged(reg);
    } else if (inst == 'y') {
        char reg = getInstAtOffset(2);
        ++_regs.at(reg).y;
        emit regChanged(reg);
    } else {
        ++_regs.at(inst);
        emit regChanged(inst);
    }
}

void Organism::subRegs()
{
    char reg1 = getInstAtOffset(1);
    char reg2 = getInstAtOffset(2);
    char reg3 = getInstAtOffset(3);
    _regs.at(reg3) = _regs.at(reg1) - _regs.at(reg2);
    emit regChanged(reg3);
}

void Organism::loadInst()
{
    char reg1 = getInstAtOffset(1);
    Point reg1Conts = _regs.at(reg1);
    char inst = _memory->instAt(reg1Conts.x, reg1Conts.y);
    char reg2 = getInstAtOffset(2);
    _regs.at(reg2) = _opcodes.at(inst);
    emit regChanged(reg2);
}

void Organism::writeInst()
{
    Point regConts = _regs.at( getInstAtOffset(2) );
    for (const auto &[inst, opcode] : _opcodes) {
        if (opcode == regConts) {
            Point coords = _regs.at(getInstAtOffset(1));
            _memory->instAt(coords.x, coords.y) = inst;
            emit cellChanged(coords.x, coords.y);
            break;
        }
    }
}

void Organism::allocChild()
{
    char reg1 = getInstAtOffset(1);
    _childSize = _regs.at(reg1);
    // ?
    if (_childSize.x == 0 && _childSize.y == 0)
        return;

    int maxI = std::max(_memory->rows(), _memory->cols());
    for (int i = 3; i < maxI; ++i) {
        _childTopLeftPos = getIpAtOffset(i);
        if (_memory->isAreaFree(_childTopLeftPos, _childSize)) {
            char reg2 = getInstAtOffset(2);
            _regs.at(reg2) = _childTopLeftPos;
            emit regChanged(reg2);
            _memory->allocArea(_childTopLeftPos, _childSize);
            return;
        }
    }
}

void Organism::separateChild()
{
    Organism *org = new Organism(_childTopLeftPos, _childSize);
    _organismQueue->add(org);
    _childSize = { 0, 0 };
}

void Organism::pushToStack()
{
    char reg = getInstAtOffset(1);
    Point regConts = _regs.at(reg);
    _stack.push(regConts);
    emit pushedToStack();
}

void Organism::popFromStack()
{
    char reg = getInstAtOffset(1);
    _regs.at(reg) = _stack.top();
    _stack.pop();
    emit popedFromStack(reg);
}

Point Organism::getIpAtOffset(int offset)
{
    return _ip + offset*_delta;
}

char Organism::getInstAtOffset(int offset)
{
    Point ipAtOffset = getIpAtOffset(offset);
    return _memory->instAt(ipAtOffset.x, ipAtOffset.y);
}

void Organism::setColors(Qt::GlobalColor frameColor, Qt::GlobalColor internalColor)
{
    int row = _topLeftPos.x;
    int lastCol = _topLeftPos.y + _size.y;
    for (int col = _topLeftPos.y; col < lastCol; ++col)
        (*_memory)(row, col).bgColor = frameColor;

    ++row;
    int lastRow = _topLeftPos.x + _size.x - 1;
    for (; row < lastRow; ++row) {
        int col = _topLeftPos.y;
        (*_memory)(row, col).bgColor = frameColor;
        ++col;
        for (; col < lastCol-1; ++col)
            (*_memory)(row, col).bgColor = internalColor;
        (*_memory)(row, col).bgColor = frameColor;
    }

    for (int col = _topLeftPos.y; col < lastCol; ++col)
        (*_memory)(row, col).bgColor = frameColor;

    emit colorsChanged(_topLeftPos, _size);
}

Memory *Organism::_memory = Memory::getInstance();
OrganismQueue *Organism::_organismQueue = OrganismQueue::getInstance();

using std::unordered_map;
using InstImpl = void (Organism::*)();
const unordered_map<char, InstImpl> Organism::_instImpls{
    { '.', &Organism::nop },
    { ':', &Organism::nop },
    { 'a', &Organism::nop },
    { 'b', &Organism::nop },
    { 'c', &Organism::nop },
    { 'd', &Organism::nop },
    { 'x', &Organism::nop },
    { 'y', &Organism::nop },
    { '^', &Organism::setDeltaUp },
    { 'v', &Organism::setDeltaDown },
    { '>', &Organism::setDeltaRight },
    { '<', &Organism::setDeltaLeft },
    { '&', &Organism::findPattern },
    { '?', &Organism::ifZero },
    { '1', &Organism::setRegToOne },
    { '0', &Organism::setRegToZero },
    { '-', &Organism::decReg },
    { '+', &Organism::incReg },
    { '~', &Organism::subRegs },
    { 'L', &Organism::loadInst },
    { 'W', &Organism::writeInst },
    { '@', &Organism::allocChild },
    { '$', &Organism::separateChild },
    { 'S', &Organism::pushToStack },
    { 'P', &Organism::popFromStack }
};

const unordered_map<char, Point> Organism::_opcodes{
    { '.', { 0, 0 } },
    { ':', { 0, 1 } },
    { 'a', { 1, 0 } },
    { 'b', { 1, 1 } },
    { 'c', { 1, 2 } },
    { 'd', { 1, 3 } },
    { 'x', { 2, 0 } },
    { 'y', { 2, 1 } },
    { '^', { 3, 0 } },
    { 'v', { 3, 1 } },
    { '>', { 3, 2 } },
    { '<', { 3, 3 } },
    { '&', { 4, 0 } },
    { '?', { 5, 0 } },
    { '1', { 6, 0 } },
    { '0', { 6, 1 } },
    { '-', { 6, 2 } },
    { '+', { 6, 3 } },
    { '~', { 6, 4 } },
    { 'L', { 7, 0 } },
    { 'W', { 7, 1 } },
    { '@', { 7, 2 } },
    { '$', { 7, 3 } },
    { 'S', { 8, 0 } },
    { 'P', { 8, 1 } }
};

int Organism::_nextID = 0;

bool operator>(const Organism &lhs, const Organism &rhs)
{
    return lhs._errors > rhs._errors;
}
