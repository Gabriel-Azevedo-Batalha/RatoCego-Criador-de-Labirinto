#ifndef CELL_H
#define CELL_H

#include <QWidget>
#include <QObject>
#include <QPixmap>

enum class cellType
{
    Wall,
    Path,
    Decision,
    End,
    Start
};

class Cell : public QWidget
{
    Q_OBJECT
public:
    explicit Cell();
    cellType getCellType();
    void setCellType(cellType newType);
    QPixmap getCellImage();
    QString getCellName();

private:
    cellType type;
    QString cellName;
    QPixmap img;
};

#endif // CELL_H
