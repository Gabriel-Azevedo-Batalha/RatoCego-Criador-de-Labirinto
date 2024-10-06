#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "helpwindow.h"

#include <QColorDialog>
#include <QLayout>
#include <QFileDialog>
#include <QFrame>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLabel>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    this->setWindowTitle("Criador de Labirintos");

    // ToolBar
    QWidget* toolbarWidget = new QWidget();
    QGridLayout* toolbarLayout = new QGridLayout(toolbarWidget);

    toolbarLayout->addWidget(makeButton("Path", SLOT(changeCellTypePath()), QKeySequence(Qt::Key_P), true), 0, 0);
    toolbarLayout->addWidget(makeButton("Decision", SLOT(changeCellTypeDecision()), QKeySequence(Qt::Key_D), true), 0, 1);
    toolbarLayout->addWidget(makeButton("Start", SLOT(changeCellTypeStart()), QKeySequence(Qt::Key_S), true), 1, 0);
    toolbarLayout->addWidget(makeButton("End", SLOT(changeCellTypeEnd()), QKeySequence(Qt::Key_E), true), 1, 1);

    QPushButton* selectedButton = makeButton("Wall", SLOT(changeCellTypeWall()), QKeySequence(Qt::Key_W), true);
    QPalette pal = selectedButton->palette();
    pal.setColor(QPalette::Button, QColor(Qt::blue));
    selectedButton->setPalette(pal);
    toolbarLayout->addWidget(selectedButton, 2, 0);
    this->selectedButton = selectedButton;

    toolbarLayout->addItem(new QSpacerItem(0, 50), 3, 0);

    QFrame *line;
    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    toolbarLayout->addWidget(line, 3, 0, 1, 2);

    toolbarLayout->addWidget(makeButton("Expand-Vertical", SLOT(increaseRows()), QKeySequence(Qt::Key_Up), false), 4, 0);
    toolbarLayout->addWidget(makeButton("Collapse-Vertical", SLOT(decreaseRows()), QKeySequence(Qt::Key_Down), false), 4, 1);
    toolbarLayout->addWidget(makeButton("Expand-Horizontal", SLOT(increaseCols()), QKeySequence(Qt::Key_Right), false), 5, 0);
    toolbarLayout->addWidget(makeButton("Collapse-Horizontal", SLOT(decreaseCols()), QKeySequence(Qt::Key_Left), false), 5, 1);

    toolbarLayout->addItem(new QSpacerItem(0, 50), 6, 0);

    QFrame *line2;
    line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    toolbarLayout->addWidget(line2, 6, 0, 1, 2);


    QPushButton* generateButton = new QPushButton;
    generateButton->setText("Generate Decision Points");
    connect(generateButton, SIGNAL(clicked()), this, SLOT(generatePoints()));
    // Colocar no final da página?
    // toolbarLayout->addItem(new QSpacerItem(0, 200), 7, 0);
    toolbarLayout->addWidget(generateButton, 8, 0, 1, 2);

    QPushButton* clearDecisionsButton = new QPushButton;
    clearDecisionsButton->setText("Clear Decision Points");
    connect(clearDecisionsButton, SIGNAL(clicked()), this, SLOT(clearPoints()));
    toolbarLayout->addWidget(clearDecisionsButton, 9, 0, 1, 2);

    QPushButton* floodFillButton = new QPushButton;
    floodFillButton->setText("Fill inacessible areas");
    connect(floodFillButton, SIGNAL(clicked()), this, SLOT(floodFill()));
    toolbarLayout->addWidget(floodFillButton, 10, 0, 1, 2);


    toolbarWidget->setLayout(toolbarLayout);
    toolbarWidget->setObjectName("toolbar");
    m_ui->toolBar->addWidget(toolbarWidget);

    // Menu Help
    QAction *helpAction = new QAction("&Help");
    helpAction->setShortcut(QKeySequence(Qt::Key_H));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(helpWindow()));

    connect(m_ui->map, SIGNAL(cellCopy(QString)), this, SLOT(copyCell(QString)));

    m_ui->menubar->addAction(helpAction);

    this->mapName = "";
    //qDebug() << qgetenv("HOME");
}

MainWindow::~MainWindow()
{

}

QPushButton* MainWindow::makeButton(QString name, const char* slot, QKeySequence key, bool selectable)
{
    QPushButton* button = new QPushButton;

    button->setObjectName(name);

    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, QColor(Qt::white));
    button->setPalette(pal);
    button->setIcon(QIcon(":/button/" + name.toLower()));
    button->setShortcut(key);
    button->setToolTip(name + " (" + button->shortcut().toString() + ")");

    button->setIconSize(QSize(52, 52));
    button->setFixedSize(64, 64);
    connect(button, SIGNAL(clicked()), m_ui->map, slot);

    if(selectable)
    {
        connect(button, SIGNAL(clicked()), this, SLOT(changeCell()));
    }
    return button;
}

void MainWindow::changeCell()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

    QPalette oldPal = this->selectedButton->palette();
    oldPal.setColor(QPalette::Button, QColor(Qt::white));
    this->selectedButton->setPalette(oldPal);

    QPalette pal = clickedButton->palette();
    pal.setColor(QPalette::Button, QColor(Qt::blue));
    clickedButton->setPalette(pal);

    this->selectedButton = clickedButton;
    repaint();
}

void MainWindow::copyCell(QString name)
{
    QPushButton *clickedButton = m_ui->toolBar->findChild<QWidget *>("toolbar")->findChild<QPushButton *>(name);
    QPalette oldPal = this->selectedButton->palette();
    oldPal.setColor(QPalette::Button, QColor(Qt::white));
    this->selectedButton->setPalette(oldPal);

    QPalette pal = clickedButton->palette();
    pal.setColor(QPalette::Button, QColor(Qt::blue));
    clickedButton->setPalette(pal);

    this->selectedButton = clickedButton;
    repaint();
}

void MainWindow::generatePoints()
{
    m_ui->map->generatePoints();
    repaint();
}

void MainWindow::clearPoints()
{
    m_ui->map->clearPoints();
    repaint();
}

void MainWindow::floodFill(){
    m_ui->map->fillInacessible();
    repaint();
}

void MainWindow::helpWindow()
{
    HelpWindow *helpWindow = new HelpWindow();
    helpWindow->exec();
}

void MainWindow::writeMapFile()
{
    QByteArray json = QJsonDocument(m_ui->map->getJSON()).toJson();

    QFile file(this->mapName);
    // TODO: Tratamento de erro
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "File open error";
        return;
    }

    file.write(json);
    file.close();
}

void MainWindow::on_actionSaveAs_triggered()
{
    if(!this->canSave()) return;
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/RatoCego";
    if (!QDir(path).exists()) QDir().mkdir(path);
    QFileDialog saveDialog = QFileDialog (this, "Save File as", path, "JSON Files (*.json)");
    saveDialog.setDefaultSuffix(".json");
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.exec();

    QStringList result = saveDialog.selectedFiles();

    if(result.isEmpty()) return;

    QString name = result.front();

    this->mapName = name;
    this->writeMapFile();

    name.replace(path + '/', "");
    this->setWindowTitle(name);
}

void MainWindow::on_actionOpen_triggered()
{
    QFile file;
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/RatoCego";
    if (!QDir(path).exists()) QDir().mkdir(path);
    QString name = QFileDialog::getOpenFileName(this, "Open File", path, "JSON Files (*.json)");
    if(name.isEmpty() or name.isNull()) return;

    file.setFileName(name);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    // TODO: Tratar caso de arquvios estranhos (Por exemplo, se não existir mapHash)
    // Extraindo o Json sem a linha "mapHash"
    QString jsonText;
    QString foundHash;
    QTextStream t(&file);
    while(!t.atEnd())
    {
        QString line = t.readLine();
        if(!line.contains("mapHash"))
        {
            jsonText.append(line + "\n");
        }
        else
        {
            int last = line.toStdString().find_last_of("\"");
            foundHash = line.mid(last-32,32);
        }
    }

    // Calcula o hash do json
    QCryptographicHash hash = QCryptographicHash(QCryptographicHash::Md5);
    hash.addData(jsonText.toUtf8());

    // qDebug() << "hash calculado: " << hash.result().toHex();

    // qDebug() << "hash encontrado: " << foundHash;

    // Check Sum
    if(foundHash != hash.result().toHex())
    {
        QMessageBox messageBox;
        messageBox.setFixedSize(500,200);
        messageBox.critical(0, "Arquivo Corrompido", "Não é possivel carregar o labirinto. O arquivo está corrompido");
        return;
    }

    this->mapName = name;
    name.replace(path + '/', "");
    this->setWindowTitle(name);

    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8());
    QJsonObject obj = doc.object();

    int w = obj.value(QString("width")).toInt();
    int h = obj.value(QString("height")).toInt();

    QJsonArray map = obj.value(QString("map")).toArray();

    m_ui->map->setVisibleCols(w); // Maze Width
    m_ui->map->setVisibleRows(h); // Maze Height

    m_ui->map->clearGrid();

    for(int row = 0; row < h; row++)
    {
        for (int col = 0; col < w; col++)
        {
            m_ui->map->setCellAtGrid(row, col, cellType(map[row][col].toInt()));

            if( cellType( map[row][col].toInt() ) == cellType::Start )
                m_ui->map->setStartPos(col, row);

            if( cellType( map[row][col].toInt() ) == cellType::End )
                m_ui->map->setEndPos(col, row);
        }
    }

    repaint();
}

void MainWindow::on_actionSave_triggered()
{

    if(this->mapName.size() > 0)
    {
        if(!this->canSave()) return;
        this->writeMapFile();
    }
    else
    {
        this->on_actionSaveAs_triggered();
    }
}

bool MainWindow::canSave()
{
    bool canSave = true;
    QPoint start = m_ui->map->getStartPos();
    QPoint end = m_ui->map->getEndPos();
    int visibleCols = m_ui->map->getVisibleCols();
    int visibleRows = m_ui->map->getVisibleRows();

    QMessageBox messageBox;
    messageBox.setFixedSize(500,200);

    // qDebug() << "##### CHECK SAVE DEBUG #####";
    // qDebug() << "START CELL TYPE: " << (int)m_ui->map->getCell(start.y(), start.x())->getCellType();
    // qDebug() << "START POS: " << start.x() << start.y();
    // qDebug() << "END CELL TYPE: " << (int)m_ui->map->getCell(end.y(), end.x())->getCellType();
    // qDebug() << "END POS: " << end.x() << end.y();
    // qDebug() << "COLS + ROWS: " << visibleCols << visibleRows;

    if(m_ui->map->getCell(start.y(), start.x())->getCellType() != cellType::Start)
    {
        messageBox.critical(0, "Não é possível salvar este labirinto", "O Labirinto não possui célula de entrada (1)");
        canSave = false;
    }
    else if(start.x() >= visibleCols || start.y() >= visibleRows)
    {
        messageBox.critical(0, "Não é possível salvar este labirinto", "O Labirinto não possui célula de entrada (2)");
        canSave = false;
    }
    else if(m_ui->map->getCell(end.y(), end.x())->getCellType() != cellType::End)
    {
        messageBox.critical(0, "Não é possível salvar este labirinto", "O Labirinto não possui célula de saída (3)");
        canSave = false;
    }
    else if(end.x() >= visibleCols || end.y() >= visibleRows)
    {
        messageBox.critical(0, "Não é possível salvar este labirinto", "O Labirinto não possui célula de saída (4)");
        canSave = false;
    }
    else if(!m_ui->map->isConnected())
    {
        messageBox.critical(0, "Não é possível salvar este labirinto", "O Labirinto não possui todos os pontos de decisão conectados");
        canSave = false;
    }

    return canSave;
}

void MainWindow::on_actionNew_triggered()
{
    m_ui->map->setVisibleCols(10);
    m_ui->map->setVisibleRows(10);
    m_ui->map->clearGrid();
    m_ui->map->setCellAtGrid(0, 0, cellType::Start);
    m_ui->map->setCellAtGrid(9, 9, cellType::End);
    this->setWindowTitle("Criador de Labirintos");
    this->mapName = "";
    repaint();
}

