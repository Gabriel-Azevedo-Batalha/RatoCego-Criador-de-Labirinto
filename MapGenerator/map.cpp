#include "map.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QRgb>
#include <QDebug>
#include <QPainterPath>
#include <QCursor>
#include <QJsonArray>
#include <QMessageBox>

Map::Map(QWidget *parent) : QWidget(parent)
{
    this->rows = 30;
    this->columns = 30;
    this->visibleRows = 10;
    this->visibleCols = 10;
    this->startPos = QPoint(0, 0);
    this->endPos = QPoint(9, 9);
    changeCellTypeWall();

    for(int i=0; i<this->rows; i++)
    {
        for(int j=0; j<this->columns; j++)
        {
            Cell* cell = new Cell;
            if(i >= this->visibleRows || j >= this->visibleCols)
                cell->setVisible(false);

            this->cellGrid.addWidget(cell, i, j);
        }
    }

    Cell* startCell = getCell(this->startPos.y(), this->startPos.x());
    startCell->setCellType(cellType::Start);
    Cell* endCell = getCell(this->endPos.y(), this->endPos.x());
    endCell->setCellType(cellType::End);

    this->cellGrid.setSpacing(0);
    this->setLayout(&cellGrid);
}

Map::~Map()
{

}

void Map::changeCellTypeWall()
{
    this->selectedCellType = cellType::Wall;
}

void Map::changeCellTypeStart()
{
    this->selectedCellType = cellType::Start;
}

void Map::changeCellTypeEnd()
{
    this->selectedCellType = cellType::End;
}

void Map::changeCellTypeDecision()
{
    this->selectedCellType = cellType::Decision;
}

void Map::changeCellTypePath()
{
    this->selectedCellType = cellType::Path;
}

Cell* Map::getCell(int row, int col)
{
    return qobject_cast<Cell*>(this->cellGrid.itemAtPosition(row, col)->widget());
}

Cell* Map::getCell(QMouseEvent *event)
{
    QWidget * const widget = childAt(event->pos());
    if (widget) return qobject_cast<Cell*>(widget);
    else return NULL;
}

void Map::mousePressEvent(QMouseEvent *event)
{
    Cell* cell = getCell(event);
    if(event->buttons() & Qt::LeftButton) setGridCellType(cell); // Paint
    else if(event->buttons() & Qt::RightButton) this->selectedCellType = cell->getCellType(); // Pick Cell
}

void Map::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        Cell* cell = getCell(event);
        setGridCellType(cell);
    }
}

void Map::setGridCellType(Cell* cell)
{
    if (cell != NULL)
    {
        if(cell->getCellType() == this->selectedCellType) return;

        // Garante que existam apenas uma de cada celula do tipo Start e End
        if(this->selectedCellType == cellType::Start)
        {
            Cell* oldStartCell = getCell(this->startPos.y(), this->startPos.x());
            if(oldStartCell->getCellType()==cellType::Start) oldStartCell->setCellType(cellType::Path);

            int newCellRow = this->cellGrid.indexOf(cell)/this->rows;
            int newCellCol = this->cellGrid.indexOf(cell)%this->rows;

            this->startPos.setX(newCellCol);
            this->startPos.setY(newCellRow);
        }
        else if(selectedCellType == cellType::End)
        {
            Cell* oldEndCell = getCell(this->endPos.y(), this->endPos.x());
            if(oldEndCell->getCellType()==cellType::End) oldEndCell->setCellType(cellType::Path);

            int newCellRow = this->cellGrid.indexOf(cell)/this->rows;
            int newCellCol = this->cellGrid.indexOf(cell)%this->rows;

            this->endPos.setX(newCellCol);
            this->endPos.setY(newCellRow);
        }
        cell->setCellType(this->selectedCellType);
        repaint();
    }
}

void Map::increaseRows()
{
    if(this->visibleRows+1 > this->rows) return;
    for(int j=0; j<this->visibleCols; j++)
    {
        Cell* cell = getCell(this->visibleRows, j);
        cell->setVisible(true);
    }
    this->visibleRows++;

    repaint();
}

void Map::increaseCols()
{
    if(this->visibleCols+1 > this->columns) return;
    for(int i=0; i<this->visibleRows; i++)
    {
        Cell* cell = getCell(i, this->visibleCols);
        cell->setVisible(true);
    }
    this->visibleCols++;

    repaint();
}

void Map::decreaseRows()
{
    if(this->visibleRows-1 < 2) return;
    for(int j=0; j<this->visibleCols; j++)
    {
        Cell* cell = getCell(this->visibleRows-1, j);
        cell->setVisible(false);
    }
    this->visibleRows--;

    repaint();
}

void Map::decreaseCols()
{
    if(visibleCols-1 < 2) return;
    for(int i=0; i<this->visibleRows; i++)
    {
        Cell* cell = getCell(i, this->visibleCols-1);
        cell->setVisible(false);
    }
    this->visibleCols--;

    repaint();
}

void Map::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);

    paintGrid(&painter);
}

void Map::paintGrid(QPainter* painter)
{
    for(int i=0; i<this->visibleRows; i++)
    {
        for(int j=0; j<this->visibleCols; j++)
        {
            Cell* cell = getCell(i, j);
            painter->drawPixmap(cell->pos(), cell->getCellImage());
        }
    }
}

void Map::setVisibleCols(int w)
{
    while(w > this->visibleCols) increaseCols();
    while(w < this->visibleCols) decreaseCols();
}

void Map::setVisibleRows(int h)
{
    while(h > this->visibleRows) increaseRows();
    while(h < this->visibleRows) decreaseRows();
}

int Map::getVisibleCols()
{
    return this->visibleCols;
}

int Map::getVisibleRows()
{
    return this->visibleRows;
}

QPoint Map::getStartPos()
{
    return this->startPos;
}

QPoint Map::getEndPos()
{
    return this->endPos;
}

void Map::setStartPos(int x, int y)
{
    this->startPos = QPoint(x, y);
}

void Map::setEndPos(int x, int y)
{
    this->endPos = QPoint(x, y);
}

void Map::setCellAtGrid(int i, int j, cellType type)
{
    Cell* cell = qobject_cast<Cell*>(this->cellGrid.itemAtPosition(i, j)->widget());

    if(type == cellType::Start)
    {
        this->startPos.setX(i);
        this->startPos.setY(j);
    }
    else if(type == cellType::End)
    {
        this->endPos.setX(i);
        this->endPos.setY(j);
    }
    cell->setCellType(type);
}

void Map::clearGrid(){
    for(int i=0; i<this->rows; i++)
    {
        for(int j=0; j<this->columns; j++)
        {
            setCellAtGrid(i, j, cellType::Path);
        }
    }
}

QString Map::findMoves(int row, int col)
{
    QString moves = "";

    for (int i = row - 1; i >= 0; --i)
    {
        Cell* cell = getCell(i, col);
        if (cell->getCellType() == cellType::Wall) break;
        if (cell->getCellType() == cellType::Decision || cell->getCellType() == cellType::End || cell->getCellType() == cellType::Start)
        {
            moves.append("N");
            break;
        }
    }

    for (int i = row + 1; i < this->visibleRows; ++i)
    {
        Cell* cell = getCell(i, col);
        if (cell->getCellType() == cellType::Wall) break;
        if (cell->getCellType() == cellType::Decision || cell->getCellType() == cellType::End || cell->getCellType() == cellType::Start)
        {
            moves.append("S");
            break;
        }
    }

    for (int j = col + 1; j < this->visibleCols; ++j)
    {
        Cell* cell = getCell(row, j);
        if (cell->getCellType() == cellType::Wall) break;
        if (cell->getCellType() == cellType::Decision || cell->getCellType() == cellType::End || cell->getCellType() == cellType::Start)
        {
            moves.append("E");
            break;
        }
    }

    for (int j = col - 1; j >= 0; --j)
    {
        Cell* cell = getCell(row, j);
        if (cell->getCellType() == cellType::Wall) break;
        if (cell->getCellType() == cellType::Decision || cell->getCellType() == cellType::End || cell->getCellType() == cellType::Start)
        {
            moves.append("W");
            break;
        }
    }

    return moves;
}

QJsonObject Map::getJSON()
{
    QJsonObject json;

    json.insert("cellSize", 60);
    json.insert("height", this->visibleRows);
    json.insert("width", this->visibleCols);

    QJsonObject exit;
    exit.insert("row", this->endPos.x());
    exit.insert("col", this->endPos.y());
    json.insert("exit", exit);

    QJsonArray map;
    QJsonArray decisions;
    int dCount = 0;
    for(int i=0; i < this->visibleRows; i++)
    {
        QJsonArray line;
        for(int j=0; j < this->visibleCols; j++)
        {
            Cell* cell = getCell(i, j);
            line.push_back(static_cast<int>(cell->getCellType()));
            if(cell->getCellType() == cellType::Decision or cell->getCellType() == cellType::Start)
            {
                QJsonObject decision;
                decision.insert("row", i);
                decision.insert("col", j);
                decision.insert("moves", findMoves(i, j));
                dCount++;
                decisions.push_back(decision);
            }
        }
        map.append(line);
    }
    json.insert("map", map);
    json.insert("decisionCount", dCount);
    json.insert("decisions", decisions);

    return json;
}
