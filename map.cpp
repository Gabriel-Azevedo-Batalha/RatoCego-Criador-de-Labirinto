#include "map.h"
#include "pixmapstorage.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QRgb>
#include <QDebug>
#include <QPainterPath>
#include <QJsonArray>
#include <QMessageBox>
#include <QQueue>

Map::Map(QWidget *parent) : QWidget(parent)
{
    this->connected = false;
    this->rows = 40;
    this->columns = 40;
    this->visibleRows = 10;
    this->visibleCols = 10;
    this->startPos = QPoint(0, 0);
    this->endPos = QPoint(9, 9);
    changeCellTypeWall();

    // Carrega os pixmaps das celulas
    PixmapStorage::getGlobalStorage()["wall"] = QPixmap(":/cell/wall");
    PixmapStorage::getGlobalStorage()["path"] = QPixmap(":/cell/path");
    PixmapStorage::getGlobalStorage()["decision"] = QPixmap(":/cell/decision");
    PixmapStorage::getGlobalStorage()["start"] = QPixmap(":/cell/start");
    PixmapStorage::getGlobalStorage()["end"] = QPixmap(":/cell/end");

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
    else if(event->buttons() & Qt::RightButton)
    {
        this->selectedCellType = cell->getCellType(); // Pick Cell
        emit cellCopy(cell->getCellName());
    }
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

void Map::clearPoints(){
    for(int i=0; i<this->visibleRows; i++)
    {
        for(int j=0; j<this->visibleCols; j++)
        {
            if(getCell(i, j)->getCellType() == cellType::Decision)
                setCellAtGrid(i, j, cellType::Path);
        }
    }
}

// BFS
bool Map::checkConectivity(Graph& graph, bool onlyStart)
{
    QPair<int, int> node = {this->startPos.y(), this->startPos.x()};
    QMap<QPair<int, int>, bool> visited;
    QQueue<QPair<int, int>> queue;
    int initialSize = graph.size();
    bool startEnd = false;

    queue.enqueue(node);
    while(!queue.empty())
    {
        node = queue.dequeue();
        if(node.first == this->endPos.y() and node.second == this->endPos.x()) startEnd = true;

        for (const QPair<int, int>& p : graph[node])
        {
            if(!visited.contains(node)) queue.enqueue(p);
        }

        visited.insert(node, true);
        graph.remove(node);
    }

    if(onlyStart) return startEnd;

    if(visited.size() == initialSize)
    {
        return true;
    }

    return false;
}

QString Map::findMoves(int row, int col, Graph& graph)
{
    QString moves = "";
    QList<QPair<int, int>> adj;

    for (int i = row - 1; i >= 0; --i)
    {
        Cell* cell = getCell(i, col);
        if (cell->getCellType() == cellType::Wall) break;
        if (cell->getCellType() == cellType::Decision || cell->getCellType() == cellType::End || cell->getCellType() == cellType::Start)
        {
            adj.append(QPair<int, int>(i, col));
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
            adj.append(QPair<int, int>(i, col));
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
            adj.append(QPair<int, int>(row, j));
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
            adj.append(QPair<int, int>(row, j));
            moves.append("W");
            break;
        }
    }

    graph.insert(QPair<int, int>(row, col), adj);

    return moves;
}

bool Map::isConnected()
{
    return this->connected;
}

QJsonObject Map::getJSON()
{
    QJsonObject json;

    json.insert("cellSize", 60);
    json.insert("height", this->visibleRows);
    json.insert("width", this->visibleCols);

    QJsonObject exit;
    exit.insert("row", this->endPos.y());
    exit.insert("col", this->endPos.x());
    json.insert("exit", exit);

    QJsonArray map;
    QJsonArray decisions;
    int dCount = 0;

    Graph graph;

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
                decision.insert("moves", findMoves(i, j, graph));
                dCount++;
                decisions.push_back(decision);
            }
        }
        map.append(line);
    }

    graph.insert(QPair<int, int>(this->endPos.y(), this->endPos.x()), QList<QPair<int, int>>());

    json.insert("map", map);
    json.insert("decisionCount", dCount);
    json.insert("decisions", decisions);

    this->connected = checkConectivity(graph, false);

    return json;
}

void Map::generatePoints()
{
    for(int i=0; i < this->visibleRows; i++)
    {
        for(int j=0; j < this->visibleCols; j++)
        {
            if(getCell(i, j)->getCellType() == cellType::Path)
            {
                int vertical = 0, horizontal = 0;

                if(i > 0 and getCell(i-1, j)->getCellType() != cellType::Wall)
                    vertical++;
                if(i+1 < this->visibleRows and getCell(i+1, j)->getCellType() != cellType::Wall)
                    vertical++;
                if(j > 0 and getCell(i, j-1)->getCellType() != cellType::Wall)
                    horizontal++;
                if(j+1 < this->visibleCols and getCell(i, j+1)->getCellType() != cellType::Wall)
                    horizontal++;

                if(horizontal+vertical == 1 or (horizontal > 0 and vertical > 0))
                    setCellAtGrid(i, j, cellType::Decision);
                else if(horizontal+vertical == 0)
                    setCellAtGrid(i, j, cellType::Wall);
            }
        }
    }
}

void Map::fillInacessible()
{
    generatePoints();
    QList<QPoint> decisions;
    Graph graph;

    // Achar pontos de decisão
    for(int i=0; i<this->visibleRows; i++)
    {
        for(int j=0; j<this->visibleCols; j++)
        {
            if(getCell(i, j)->getCellType() == cellType::Decision){
                decisions.push_back({i, j});
                findMoves(i, j, graph);
            }
        }
    }

    graph.insert(QPair<int, int>(this->endPos.y(), this->endPos.x()), QList<QPair<int, int>>());
    findMoves(this->startPos.y(), this->startPos.x(), graph);

    if(!checkConectivity(graph, true))
    {
        QMessageBox messageBox;
        messageBox.setFixedSize(500,200);
        messageBox.critical(0, "Não é possível preencher", "A entrada do Labirinto não está conectada com a saída");
        return;
    }

    while(graph.size() > 0)
    {
        QPair<int, int> node = graph.firstKey();
        int i, j;

        setCellAtGrid(node.first, node.second, cellType::Wall);

        // Cima
        for(i=node.first-1, j=node.second; i>= 0; i--)
        {
            if(getCell(i, j)->getCellType() != cellType::Path)
                break;
            else
                setCellAtGrid(i, j, cellType::Wall);
        }

        // Baixo
        for(i=node.first+1, j=node.second; i < this->visibleRows; i++)
        {
            if(getCell(i, j)->getCellType() != cellType::Path)
                break;
            else
                setCellAtGrid(i, j, cellType::Wall);
        }

        // Esquerda
        for(i=node.first, j=node.second-1; j >= 0; j--)
        {
            if(getCell(i, j)->getCellType() != cellType::Path)
                break;
            else
                setCellAtGrid(i, j, cellType::Wall);
        }

        // Direita
        for(i=node.first, j=node.second+1; j < this->visibleCols; j++)
        {
            if(getCell(i, j)->getCellType() != cellType::Path)
                break;
            else
                setCellAtGrid(i, j, cellType::Wall);
        }
        graph.remove(node);
    }
}


