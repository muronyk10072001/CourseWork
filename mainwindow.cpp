#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <math.h>
#include <QMessageBox>

#include "documentation.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    file_name = nullptr;
    unsaved_work = false;
    path = false;
    setWindowTitle("[Untitled.txt] - Chess program");

    connect(ui->widget, SIGNAL(check_fights(int)), this, SLOT(on_check_clicked(int)));
    connect(ui->widget, SIGNAL(update_list()), this, SLOT(update_ListWidget()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::write_file()
{
    setWindowTitle("[" + file_name + "] - Chess program");
    QVector<figure> *data = ui->widget->GetData();
    // qDebug() << data->size() << " " << file_name;

    QFile Output(file_name);
    if (!Output.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error: can not open file...";
        return;
    }
    QTextStream out(&Output);
    for (int i = 0; i < data->size(); i++)
    {
        out << id_to_string((*data)[i].type) << " " << (*data)[i].x << " " << (*data)[i].y << "\n";
        // qDebug() << id_to_string((*data)[i].type) << " " << (*data)[i].x << " " << (*data)[i].y << "\n";
    }
    Output.flush();
    Output.close();
}

void MainWindow::read_file()
{
    int t;
    figure tmp;
    QString buff;
    QVector<figure> *data = ui->widget->GetData();

    QFile Input(file_name);
    if (!Input.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "Error: can not open file...";
    QTextStream in(&Input);

    data->clear();

    in >> buff;
    tmp.type = string_to_id(buff);;
    in >> tmp.x;
    in >> tmp.y;
    while (!in.atEnd())
    {
        if (data->size() == 64)
        {
            QMessageBox::warning(this, "Warning:", "Chess board is full...");
            Input.flush();
            Input.close();
            return;
        }
        // qDebug() << tmp.type << " " << tmp.x << " " << tmp.y << "\n";
        if (tmp.type != -1 && (tmp.x >= 1 && tmp.x <= 8) && (tmp.y >= 1 && tmp.y <= 8))
            data->push_back(tmp);
        else
            QMessageBox::warning(this, "Error:", "Wrong input data...");
        in >> buff;
        tmp.type = string_to_id(buff);
        in >> tmp.x;
        in >> tmp.y;
    }
    Input.flush();
    Input.close();
}

int MainWindow::string_to_id(QString name)
{
    if (name == "king")
        return 0;
    if (name == "queen")
        return 1;
    if (name == "rook")
        return 2;
    if (name == "bishop")
        return 3;
    if (name == "knight")
        return 4;
    return -1;
}

QString MainWindow::id_to_string(int id)
{
    switch (id)
    {
    case 0:
        return "king";
    case 1:
        return "queen";
    case 2:
        return "rook";
    case 3:
        return "bishop";
    case 4:
        return "knight";
    default:
        return nullptr;
    }
    return "";
}

void MainWindow::save_work()
{
    if (unsaved_work)
    {
        // connect
        unsaved_work = false;
        connect(ui->widget, SIGNAL(unsave()), this, SLOT(save_flag()));
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Save?", "You have unsaved work. Save it?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            if (file_name == nullptr)
                file_name = QFileDialog::getSaveFileName(this, tr("Save file"), "/home/", "All files (*.*);;Text file (*.txt);;CSV-file (*.csv)");
            write_file();
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (file_name == nullptr)
        file_name = QFileDialog::getSaveFileName(this, tr("Save file"), "/home/", "All files (*.*);;Text file (*.txt);;CSV-file (*.csv)");
    if (file_name != nullptr)
        write_file();
}

void MainWindow::on_actionQuit_triggered()
{
    save_work();
    QCoreApplication::quit();
}

void MainWindow::on_actionOpen_triggered()
{
    save_work();
    file_name = QFileDialog::getOpenFileName(this, tr("Open file"), "/home/", "All files (*.*);;Text file (*.txt)");
    if (file_name != nullptr)
    {
        // QMessageBox::information(this, tr("file name"), file_name);
        setWindowTitle("[*" + file_name + "] - Chess program");
    }
    else
    {
        QMessageBox::information(this, tr("file name"), "Error: file not open...");
        return;
    }
    unsaved_work = true;
    read_file();
}

void MainWindow::on_actionNew_triggered()
{
    save_work();
    setWindowTitle("[*Untitled.txt] - Chess program");
    unsaved_work = true;
    file_name = nullptr;
}

void MainWindow::on_actionSave_as_triggered()
{
    file_name = QFileDialog::getSaveFileName(this, tr("Save file as"), "/home/", "All files (*.*);;Text file (*.txt)");
    write_file();
//    unsaved_work = false;
}

void MainWindow::on_pushButton_4_clicked()
{
    save_flag();
    int s = ui->widget->selectedFigure;
    if (s != -1)
    {
        QVector<figure> *data = ui->widget->GetData();
        QVector<point> *battle = ui->widget->GetBattle();
        data->erase(data->begin() + s);
//        battle->erase(battle->begin() + s);
        battle->clear();
        ui->widget->selectedFigure = -1;
        ui->listWidget->clear();
    }
}

void MainWindow::on_clear_scene_clicked()
{
    QVector<figure> *data = ui->widget->GetData();
    data->clear();
    QVector<point> *battle = ui->widget->GetBattle();
    battle->clear();
    ui->widget->selectedFigure = -1;
}

void MainWindow::check_rook(int selected)
{
    QString t;
    QVector<figure> *data = ui->widget->GetData();
    QVector<point> *battle = ui->widget->GetBattle();
    point tmp;
    float d1, d2;
    bool flag = false;

    for (int i = 0; i < data->size(); i++)
    {
        if (i == selected)
            continue;
        flag = true;
        if ((*data)[i].y == (*data)[selected].y)
        {
            d1 = (*data)[i].x - (*data)[selected].x;
            for (int j = 0; j < battle->size(); j++)
            {
                d2 = (*battle)[j].x - (*data)[selected].x;
                if (d1 * d2 > 0)
                {
                    if (abs(d1) < abs(d2))
                    {
                        list_battles.erase(list_battles.begin() + j);
                        battle->erase(battle->begin() + j);
                    }
                    else
                    {
                        flag = false;
                        break;
                    }
                }
            }
            if (flag)
            {
                tmp.x = (*data)[i].x;
                tmp.y = (*data)[i].y;
                battle->push_back(tmp);
                t = id_to_string((*data)[selected].type) + "(" + QString::number((*data)[selected].x) + ", " + QString::number((*data)[selected].y) + ") beats a " + id_to_string((*data)[i].type) + " (" + QString::number((*data)[i].x) + ", " + QString::number((*data)[i].y) + ")";
                list_battles.push_back(t);
            }
        }
        if ((*data)[i].x == (*data)[selected].x)
        {
            d1 = (*data)[i].y - (*data)[selected].y;
            for (int j = 0; j < battle->size(); j++)
            {
                d2 = (*battle)[j].y - (*data)[selected].y;
                if (d1 * d2 > 0)
                {
                    if (abs(d1) < abs(d2))
                    {
                        list_battles.erase(list_battles.begin() + j);
                        battle->erase(battle->begin() + j);
                    }
                    else
                    {
                        flag = false;
                        break;
                    }
                }
            }
            if (flag)
            {
                tmp.x = (*data)[i].x;
                tmp.y = (*data)[i].y;
                battle->push_back(tmp);
                t = id_to_string((*data)[selected].type) + "(" + QString::number((*data)[selected].x) + ", " + QString::number((*data)[selected].y) + ") beats a " + id_to_string((*data)[i].type) + " (" + QString::number((*data)[i].x) + ", " + QString::number((*data)[i].y) + ")";
                list_battles.push_back(t);
            }
        }
    }
}

void MainWindow::check_bishop(int selected)
{
    QString t;
    QVector<figure> *data = ui->widget->GetData();
    QVector<point> *battle = ui->widget->GetBattle();
    point tmp;
    float d1, d2;
    float deltaX, deltaY;
    bool flag = false;

    for (int i = 0; i < data->size(); i++)
    {
        if (i == selected)
            continue;
        deltaX = (*data)[i].x - (*data)[selected].x;
        deltaY = (*data)[i].y - (*data)[selected].y;
        if (abs(deltaX) == abs(deltaY))
        {
            flag = true;
            for (int j = 0; j < battle->size(); j++)
            {
                d1 = (*battle)[j].x - (*data)[selected].x;
                d2 = (*battle)[j].y - (*data)[selected].y;
                if (d1 * deltaX > 0 && d2 * deltaY > 0)
                {
                    if (deltaX*deltaX + deltaY * deltaY < d1 * d1 + d2 * d2)
                    {
                        list_battles.erase(list_battles.begin() + j);
                        battle->erase(battle->begin() + j);
                    }
                    else
                    {
                        flag = false;
                        break;
                    }
                }
            }
            if (flag)
            {
                tmp.x = (*data)[i].x;
                tmp.y = (*data)[i].y;
                battle->push_back(tmp);
                t = id_to_string((*data)[selected].type) + "(" + QString::number((*data)[selected].x) + ", " + QString::number((*data)[selected].y) + ") beats a " + id_to_string((*data)[i].type) + " (" + QString::number((*data)[i].x) + ", " + QString::number((*data)[i].y) + ")";
                list_battles.push_back(t);
            }
        }
    }
}

void MainWindow::check_knight(int selected)
{
    QString t;

    QVector<figure> *data = ui->widget->GetData();
    QVector<point> *battle = ui->widget->GetBattle();
    point tmp;
    float deltaX, deltaY;

    for (int i = 0; i < data->size(); i++)
    {
        deltaX = (*data)[i].x - (*data)[selected].x;
        deltaY = (*data)[i].y - (*data)[selected].y;
        if ((abs(deltaX) == 1 && abs(deltaY) == 2) || (abs(deltaX) == 2 && abs(deltaY) == 1))
        {
            tmp.x = (*data)[i].x;
            tmp.y = (*data)[i].y;
            battle->push_back(tmp);
            t = id_to_string((*data)[selected].type) + "(" + QString::number((*data)[selected].x) + ", " + QString::number((*data)[selected].y) + ") beats a " + id_to_string((*data)[i].type) + " (" + QString::number((*data)[i].x) + ", " + QString::number((*data)[i].y) + ")";
            list_battles.push_back(t);
        }
    }
}

void MainWindow::check_king(int selected)
{
    QString t;

    QVector<figure> *data = ui->widget->GetData();
    QVector<point> *battle = ui->widget->GetBattle();
    float deltaX, deltaY;
    point tmp;

    for (int i = 0; i < data->size(); i++)
    {
        if (i == selected)
            continue;
        deltaX = (*data)[i].x - (*data)[selected].x;
        deltaY = (*data)[i].y - (*data)[selected].y;
        if (abs(deltaX) <= 1 && abs(deltaY) <= 1)
        {
            tmp.x = (*data)[i].x;
            tmp.y = (*data)[i].y;
            battle->push_back(tmp);
            t = id_to_string((*data)[selected].type) + "(" + QString::number((*data)[selected].x) + ", " + QString::number((*data)[selected].y) + ") beats a " + id_to_string((*data)[i].type) + " (" + QString::number((*data)[i].x) + ", " + QString::number((*data)[i].y) + ")";
            list_battles.push_back(t);
        }
    }
}

void MainWindow::on_check_clicked(int selected)
{
    QVector<QString> res;
    QVector<figure> *data = ui->widget->GetData();
    QVector<point> *battle = ui->widget->GetBattle();
    battle->clear();
    ui->listWidget->clear();
    list_battles.clear();

    switch ((*data)[selected].type)
    {
    case 0:
        check_king(selected);
        break;
    case 1:
        check_rook(selected);
        check_bishop(selected);
        break;
    case 2:
        check_rook(selected);
        break;
    case 3:
        check_bishop(selected);
        break;
    case 4:
        check_knight(selected);
        break;
    default:
        break;
    }
}

void MainWindow::update_ListWidget()
{
    ui->listWidget->clear();
    for (int i = 0; i < list_battles.size(); i++)
    {
        ui->listWidget->addItem(list_battles[i]);
    }
}

void MainWindow::save_flag()
{
    unsaved_work = true;
    setWindowTitle("[*" + file_name + "] - CHess program");
}

void MainWindow::on_change_type_activated(int index)
{
    save_flag();
    QVector<figure> *data = ui->widget->GetData();
    (*data)[ui->widget->selectedFigure].type = index;

    on_check_clicked(ui->widget->selectedFigure);
    update_ListWidget();
}

void MainWindow::on_comboBox_activated(int index)
{
    ui->widget->type_of_new_figure = index;
}

void MainWindow::on_save_current_kils_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save file"), "/home/", "All files (*.*);;Text file (*.txt)");
    if (file == nullptr)
    {
        qDebug() << "file's name is empty...";
        return;
    }
    int lim = list_battles.size();

    QFile Output(file);
    if (!Output.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error: can not open file...";
        return;
    }
    QTextStream out(&Output);

    for (int i = 0; i < lim ; i++)
    {
        out << list_battles[i] << "\n";
    }
    Output.flush();
    Output.close();
}

void MainWindow::on_save_all_kils_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Save file"), "/home/", "All files (*.*);;Text file (*.txt)");
    if (file == nullptr)
    {
        qDebug() << "file's name is empty...";
        return;
    }
    ui->widget->SetShow_path(false);
    int size = ui->widget->GetData()->size();

    QFile Output(file);
    if (!Output.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error: can not open file...";
        return;
    }
    QTextStream out(&Output);

    for (int i = 0; i < size; i++)
    {
        on_check_clicked(i);
        for (int j = 0; j < list_battles.size(); j++)
           out << list_battles[j] << "\n";
    }
    Output.flush();
    Output.close();
    ui->widget->SetShow_path(true);
}

void MainWindow::on_actionDocumentation_triggered()
{
    Documentation *newWindow = new Documentation;
    newWindow->show();

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About"), "Chess program\nAuthor: PAVLO\ne-mail: pspos.developqkation@gmail.com\nAll right reserved");
}
