#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H

#include "memory.h"

#include <QtWidgets>
#include <boost/functional/hash.hpp>

#include <unordered_map>
#include <utility>

class MemoryView : public QAbstractScrollArea
{
    Q_OBJECT

public:
    MemoryView(const MemoryView &) = delete;
    MemoryView &operator=(const MemoryView &) = delete;

    virtual ~MemoryView() = default;

    static MemoryView *getInstance();

protected:
    MemoryView();

    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

public slots:
    void paintCellIfVisible(int row, int col);
    // TODO: Change this function so it paint the visible part of the area
    void paintArea(Point topLeftPos, Point size);

private slots:
    void onHorBarValChanged(int horBarVal);
    void onVerBarValChanged(int verBarVal);

private:
    void initCellSize();

    void updateScrollBars();

    void updateRowRange(int verBarVal);
    void updateColRange(int horBarVal);

    void paintVisibleCells();

    using Pair = std::pair<char, Qt::GlobalColor>;
    const QPixmap &getPixmapForPair(const Pair &p);

    Memory *_memory = Memory::getInstance();

    static const int BUF_WIDTH = 800;
    static const int BUF_HEIGHT = 600;
    QPixmap *_buffer = new QPixmap(BUF_WIDTH, BUF_HEIGHT);

    using PairsCache = std::unordered_map<Pair, QPixmap, boost::hash<Pair> >;
    PairsCache _pairsCache;

    int _cellWidth = -1; // -1 -- safe initial value
    int _cellHeight = -1;

    int _viewX = -1;
    int _firstRow = -1;
    int _lastRow = -1;
    int _viewY = -1;
    int _firstCol = -1;
    int _lastCol = -1;
};

#endif // MEMORYVIEW_H
