#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include "map.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void on_actionSaveAs_triggered();
    void on_actionOpen_triggered();
    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void helpWindow();
    void changeCell();
    void copyCell(QString);
    void generatePoints();
    void clearPoints();
    void floodFill();

private:
    QString mapName;
    QPushButton* makeButton(QString name, const char* slot, QKeySequence key, bool selectable);
    bool canSave();
    void writeMapFile();
    Map *m_map;
    Ui::MainWindow *m_ui;
    QPushButton* selectedButton;
};

#endif // MAINWINDOW_H







