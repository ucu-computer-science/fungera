#include "organism.h"

#include "memoryview.h"
#include "organismqueue.h"
#include "src/statistics.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <cmath>
#include <utility>
#include <vector>

Organism::Organism(Point topLeftPos, Point size) : _id(_nextID), _topLeftPos(topLeftPos), _size(size), _ip(topLeftPos)
{
    connect(this, &Organism::cellChanged, MemoryView::getInstance(), &MemoryView::paintCellIfVisible);
    connect(this, &Organism::colorsChanged, MemoryView::getInstance(), &MemoryView::paintArea);

    setInactiveColors();
    Memory &mem = *Memory::getInstance();
    Cell &cell = mem(_ip.x, _ip.y);
    cell.bgColor = Qt::red;
//    emit cellChanged(_ip.x, _ip.y); // No effect warning

    ++_nextID;
}

Organism::Organism()
    : _id(_nextID), _topLeftPos({0, 0}), _size({0, 0}), _ip({0, 0}) { }

Organism::~Organism()
{
    Memory &mem = *Memory::getInstance();
    mem.freeArea(_topLeftPos, _size);
    if (_childSize != Point(0, 0)) // Why does { 0, 0 } cannot be used?
        mem.freeArea(_childTopLeftPos, _childSize);
}

void Organism::self_serialize() {
    // TODO: Fix this hardcoded name and before executing check if folder exists
    size_t curr_cycle = OrganismQueue::getInstance()->cycle_no;
    std::ofstream o_this("organisms/" + std::to_string(this->_id) + "_" + std::to_string(curr_cycle));
    {
        boost::archive::text_oarchive oa_child(o_this);
        oa_child << *this;
    }
    this->last_snap_cycle = curr_cycle;
}

void Organism::cycle()
{
    Memory &mem = *Memory::getInstance();

    try {
        char inst = _memory->instAt(_ip.x, _ip.y);
        (this->*_instImpls.at(inst))();
    } catch (...) { // Catch any type of error
        ++_errors;
    }
    mem(_ip.x, _ip.y).bgColor = mem(_ip.x, _ip.y).lastBgColor;
    emit cellChanged(_ip.x, _ip.y);
    _ip += _delta;
    // TODO: Encapsulate the change of the color FUCK
    mem(_ip.x, _ip.y).bgColor = Qt::red;
    emit cellChanged(_ip.x, _ip.y);


    // TODO: Extract snapshoting function for organism to separate method
    size_t curr_cycle = OrganismQueue::getInstance()->cycle_no;
    if (curr_cycle - this->last_snap_cycle > 4200)
    {
        //this->self_serialize();
    }
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
    // TODO: !!!!!! this function probably is not working correctly: it misses possible patterns if they are subpatterns of another incorrect pattern!!!!
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
    size_t ctr = 0;
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
    if (_childTopLeftPos.x + _childSize.x >= Memory::getInstance()->rows()
            || _childTopLeftPos.y + _childSize.y >= Memory::getInstance()->cols()) {
        throw "Part of child is out of memory map, child is not added";
    }
    Organism *org = new Organism(_childTopLeftPos, _childSize);
    _organismQueue->addInterim(org);
    _childSize = { 0, 0 };

    size_t curr_cycle = OrganismQueue::getInstance()->cycle_no;
    OrganismQueue::getInstance()->qStat.addRelation(org->_id, this->_id, curr_cycle);

    //this->self_serialize();
    //org->self_serialize();
}

void Organism::pushToStack()
{
    char reg = getInstAtOffset(1);
    Point regConts = _regs.at(reg);
    _stack.push_back(regConts);
    emit pushedToStack();
}

void Organism::popFromStack()
{
    char reg = getInstAtOffset(1);
    _regs.at(reg) = _stack.back();
    _stack.pop_back();
    emit popedFromStack(reg);
}

void Organism::jump()
{
    // TODO: !!!!!! Jump probably is not working correctly: it misses possible patterns if they are subpatterns of another incorrect pattern!!!!
    std::vector<char> pattern;
    int i = 1;
    // RANGE FOR JUMP : TODO: Reconsider
    int range_j = 100;
    for (; i < range_j; ++i) {
        char inst = getInstAtOffset(i);
        if (inst == '.')
            pattern.push_back(':');
        else if (inst == ':')
            pattern.push_back('.');
        else
            break;
    }
    ++i;
    size_t ctr = 0;
    for (; i < range_j; ++i) {
        if (getInstAtOffset(i) == pattern[ctr]) {
            ++ctr;
        } else {
            ctr = 0;
        }
        if (ctr == pattern.size()) {
            _ip = getIpAtOffset(i);
            break;
        }
    }
}

static Point get_alt_delta(Point delta) {
    Point alt_delta(delta.x ? 0 : 1, delta.y ? 0 : 1);
    return alt_delta;
}

static Point get_reverse_delta(Point delta) {
    if (delta.x == 0) return Point(delta.x, delta.y * -1);
    if (delta.y == 0) return Point(delta.x * -1, delta.y);
    return delta;
}

void Organism::jumpInRange() {
    std::vector<char> pattern;
    int i = 1;

    // RANGE FOR JUMP : TODO: Reconsider
    int range_o = 100;

    for (; i < range_o; ++i) {
        char inst = getInstAtOffset(i);
        if (inst == '.')
            pattern.push_back(':');
        else if (inst == ':')
            pattern.push_back('.');
        else
            break;
    }

    int min_x = fmax(0, _ip.x-range_o);
    int max_x = fmin(Memory::getInstance()->cols()-1, _ip.x+range_o);

    int min_y = fmax(0, _ip.y-range_o);
    int max_y = fmin(Memory::getInstance()->rows()-1, _ip.y+range_o);

    std::vector<std::pair<int, int>> possible_res;
    int init_ip_x = _ip.x,
        init_ip_y = _ip.y;

    if (_delta.x == -1) {
        for (int y = min_y; y <= max_y; y++) {
            size_t ctr = 0;
            for (int x = max_x; x >= min_x; x--) {
                if (_memory->instAt(x, y) == pattern[ctr]) {
                    ++ctr;
                } else {
                    x = x + ctr;
                    ctr = 0;
                }
                if (ctr == pattern.size()) {
                    std::pair<int, int> cu_pair(x, y);
                    possible_res.push_back(cu_pair);
                    break;
                }
            }
        }
    }
    else if (_delta.x == 1) {
        for (int y = min_y; y <= max_y; y++) {
            size_t ctr = 0;
            for (int x = min_x; x <= max_x; x++) {
                if (_memory->instAt(x, y) == pattern[ctr]) {
                    ++ctr;
                } else {
                    x = x - ctr;
                    ctr = 0;
                }
                if (ctr == pattern.size()) {
                    std::pair<int, int> cu_pair(x, y);
                    possible_res.push_back(cu_pair);
                    break;
                }
            }
        }
    }
    else if (_delta.y == 1) {
        for (int x = min_x; x <= max_x; x++) {
            size_t ctr = 0;
            for (int y = min_y; y <= max_y; y++) {
                if (_memory->instAt(x, y) == pattern[ctr]) {
                    ++ctr;
                } else {
                    y = y - ctr;
                    ctr = 0;
                }
                if (ctr == pattern.size()) {
                    std::pair<int, int> cu_pair(x, y);
                    possible_res.push_back(cu_pair);
                    break;
                }
            }
        }
    }
    else if (_delta.y == -1) {
        for (int x = min_x; x <= max_x; x++) {
            size_t ctr = 0;
            for (int y = max_y; y >= min_y; y--) {
                if (_memory->instAt(x, y) == pattern[ctr]) {
                    ++ctr;
                } else {
                    y = y + ctr;
                    ctr = 0;
                }
                if (ctr == pattern.size()) {
                    std::pair<int, int> cu_pair(x, y);
                    possible_res.push_back(cu_pair);
                    break;
                }
            }
        }
    }

    if (possible_res.size())
    {
        std::pair<int, int> min_point = possible_res[0];
        double min_len = std::sqrt(pow(min_point.first-_ip.x, 2)+pow(min_point.second-_ip.y, 2));

        // First occurance is prioritized
        for (size_t i = 1; i < possible_res.size(); i++) {
            double current_len = std::sqrt(pow(possible_res[i].first-_ip.x, 2)
                    + pow(possible_res[i].second-_ip.y, 2));
            if (current_len < min_len) {
                min_len = current_len;
                min_point.first = possible_res[i].first;
                min_point.second = possible_res[i].second;
            }
        }
        _ip.x = min_point.first;
        _ip.y = min_point.second;
    }
    else {
        _ip.x = init_ip_x;
        _ip.y = init_ip_y;
    }
        
}

void Organism::random() {
    char reg = getInstAtOffset(1);
    int rand_x = rand() % 2;
    int rand_y = rand() % 2;
    _regs.at(reg) = { rand_x, rand_y };
    emit regChanged(reg);
}

void Organism::randomDelta() {

    std::vector<Point> deltas =
            {std::vector{Point{-1, 0},  // up
                         Point{1, 0},   // down
                         Point{0, 1},   // right
                         Point{0, -1}}  // left
            };

    deltas.erase(std::remove(deltas.begin(), deltas.end(), get_reverse_delta(_delta)), deltas.end());
    int rand_i = rand() % deltas.size();
    _delta.x = deltas[rand_i].x;
    _delta.y = deltas[rand_i].y;
}

Point Organism::getIpAtOffset(int offset)
{
    return _ip + offset*_delta;
}

char Organism::getInstAtOffsetAbs(Point offset)
{
    Point ipAtOffset = _topLeftPos + offset;
    return _memory->instAt(ipAtOffset.x, ipAtOffset.y);
}

char Organism::getInstAtOffset(int offset)
{
    Point ipAtOffset = getIpAtOffset(offset);
    return _memory->instAt(ipAtOffset.x, ipAtOffset.y);
}

void Organism::setColors(Qt::GlobalColor frameColor, Qt::GlobalColor internalColor)
{
    Memory &mem = *Memory::getInstance();

    int row = _topLeftPos.x;
    int lastCol = _topLeftPos.y + _size.y;
    for (int col = _topLeftPos.y; col < lastCol; ++col) {
        Cell &cell = mem(row, col);
        cell.bgColor = cell.lastBgColor = frameColor;
    }

    ++row;
    int lastRow = _topLeftPos.x + _size.x - 1;
    for (; row < lastRow; ++row) {
        int col = _topLeftPos.y;
        mem(row, col).bgColor = mem(row, col).lastBgColor = frameColor;
        ++col;
        for (; col < lastCol-1; ++col)
            mem(row, col).bgColor = mem(row, col).lastBgColor = internalColor;
        mem(row, col).bgColor = mem(row, col).lastBgColor = frameColor;
    }

    for (int col = _topLeftPos.y; col < lastCol; ++col)
        mem(row, col).bgColor = mem(row, col).lastBgColor = frameColor;

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
    { 'P', &Organism::popFromStack },
    { 'J', &Organism::jump},
    { 'R', &Organism::random},
    { 'U', &Organism::jumpInRange},
    { '#', &Organism::randomDelta}
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
    { 'P', { 8, 1 } },
    { 'J', { 9, 0 } },
    { 'R', { 9, 1 } },
    { 'U', { 9, 2 } },
    { '#', { 9, 3 } }
};

int Organism::_nextID = 0;

bool operator>(const Organism &lhs, const Organism &rhs)
{
    return lhs._errors > rhs._errors;
}
