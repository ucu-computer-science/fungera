#include "memoryview.h"

#include <algorithm>

// The hasing function for the color is needed only in this translation
// unit
std::size_t hash_value(const QColor &color)
{
    return std::hash<QRgb>{}(color.rgba());
}

MemoryView::MemoryView() : QAbstractScrollArea(nullptr)
{
    initCellSize();

    // Assuming that the resizeEvent() is called when the widget is
    // created
//    updateRowRange(verticalScrollBar()->value());
//    updateColRange(horizontalScrollBar()->value());

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &MemoryView::onHorBarValChanged);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &MemoryView::onVerBarValChanged);
}

MemoryView *MemoryView::getInstance()
{
    static MemoryView mw;
    return &mw;
}

void MemoryView::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(viewport());
    painter.drawPixmap(0, 0, *_buffer);
}

// TODO: Move the update of the last row and column to paint here
// TODO: Add enlarging of the buffer
void MemoryView::resizeEvent(QResizeEvent *event)
{
    updateScrollBars();
    updateRowRange(verticalScrollBar()->value());
    updateColRange(horizontalScrollBar()->value());
    paintVisibleCells();
}

void MemoryView::paintCellIfVisible(int row, int col)
{
    if (row > _firstRow && row < _lastRow && col > _firstCol && col < _lastCol) {
        int cellX = col*_cellWidth - _viewX;
        int cellY = row*_cellHeight - _viewY;
        const Cell &c = (*_memory)(row, col);
        const QPixmap &pm = getPixmapForPair({ c.inst, c.bgColor });
        QPainter painter(_buffer);
        painter.drawPixmap(cellX, cellY, _cellWidth, _cellHeight, pm);
        viewport()->repaint();
    }
}

void MemoryView::paintArea(Point topLeftPos, Point size)
{
    int lastRow = topLeftPos.x + size.x;
    int lastCol = topLeftPos.y + size.y;
    QPainter painter(_buffer);
    for (int row = topLeftPos.x; row < lastRow; ++row) {
        for (int col = topLeftPos.y; col < lastCol; ++col) {
            int cellX = col*_cellWidth - _viewX;
            int cellY = row*_cellHeight - _viewY;
            const Cell &c = (*_memory)(row, col);
            const QPixmap &pm = getPixmapForPair({ c.inst, c.bgColor });
            painter.drawPixmap(cellX, cellY, _cellWidth, _cellHeight, pm);
        }
    }
    viewport()->repaint();
}

void MemoryView::initCellSize()
{
    QFontMetrics fontMetrics(viewport()->font());
    _cellWidth = 2 * fontMetrics.averageCharWidth();
    _cellHeight = fontMetrics.height();
}

void MemoryView::onHorBarValChanged(int horBarVal)
{
    updateColRange(horBarVal);
    paintVisibleCells();
}

void MemoryView::onVerBarValChanged(int verBarVal)
{
    updateRowRange(verBarVal);
    paintVisibleCells();
}

void MemoryView::updateScrollBars()
{
    QScrollBar *horBar = horizontalScrollBar();
    int w = _memory->cols() * _cellWidth;
    QSize sz = viewport()->size();
    // Assuming that the minimum of the scrollbars is always 0 and size
    // of the memory is larger than the vieport's one
    horBar->setMaximum(w-sz.width());
    horBar->setPageStep(sz.width());

    QScrollBar *verBar = verticalScrollBar();
    int h = _memory->rows() * _cellHeight;
    verBar->setMaximum(h-sz.height());
    verBar->setPageStep(sz.height());
}

void MemoryView::updateRowRange(int verBarVal)
{
    _viewY = verBarVal;
    _firstRow = _viewY / _cellHeight;
    int viewH = viewport()->height();
    _lastRow = std::min(_memory->rows(), (_viewY+viewH) / _cellHeight + 1);
}

void MemoryView::updateColRange(int horBarVal)
{
    _viewX = horBarVal;
    _firstCol = _viewX / _cellWidth;
    int viewW = viewport()->width();
    _lastCol = std::min(_memory->cols(), (_viewX+viewW) / _cellWidth + 1);
}

void MemoryView::paintVisibleCells()
{
    QPainter painter(_buffer);
    for (int row = _firstRow; row < _lastRow; ++row) {
        int cellY = row*_cellHeight - _viewY;
        for (int col = _firstCol; col < _lastCol; ++col) {
            int cellX = col*_cellWidth - _viewX;
            // TODO: Maybe add an analog of instAt() for bgColor
            const Cell &c = (*_memory)(row, col);
            const QPixmap &pm = getPixmapForPair({ c.inst, c.bgColor });
            painter.drawPixmap(cellX, cellY, _cellWidth, _cellHeight, pm);
        }
    }
}

using Pair = std::pair<char, QColor>;
const QPixmap &MemoryView::getPixmapForPair(const Pair &p)
{
    auto it = _pairsCache.find(p);
    if (it != _pairsCache.end())
        return it->second;

    QPixmap &pm = _pairsCache[p] = QPixmap(_cellWidth, _cellHeight);
    pm.fill(p.second);
    QPainter painter(&pm);
    painter.drawText(0, 0, _cellWidth, _cellHeight, Qt::AlignCenter, QString(p.first));
    return pm;
}
