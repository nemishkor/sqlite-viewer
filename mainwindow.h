#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QPointer>
#include <QSqlIndex>
#include <QSpinBox>
#include <QGroupBox>
#include <QCheckBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QWidget *window;
        QWidget *controls;
            QPushButton *btnSelectFile;
            QLabel *lblFileUrl;
            QPushButton *btnSaveFile;
            QPushButton *btnSaveAsFile;
            QPushButton *btnNewFile;
            QHBoxLayout *controlsLay;
        QWidget *table;
            QWidget *sell;
                QWidget *tableManipulation;
                    QPushButton *btnAddRow;
                    QPushButton *btnRemoveRow;
                    QHBoxLayout *tableManipulationLay;
                QWidget *sellNav;
                    QPushButton *navToStart;
                    QPushButton *navToPrev;
                    QPushButton *navToNext;
                    QPushButton *navToEnd;
                    QHBoxLayout *sellNavLay;
                QWidget *sellSet;
                    QFormLayout *sellSetLay;
                QWidget *sellBtns;
                    QPushButton *btnSellSave;
                    QPushButton *btnSellCancel;
                    QPushButton *btnSellClear;
                    QGroupBox *settings;
                        QCheckBox *chQuickWrite;
                        QVBoxLayout *settingsLay;
                    QVBoxLayout *sellBtnsLay;
                QVBoxLayout *sellLay;
            QTabWidget *tables;
            QHBoxLayout *tableLay;
        QVBoxLayout *layout;
    QFileDialog fileDialog;
    QSqlDatabase db;
    QSqlQuery *query;
    QList<QList<QStringList *> *> dbDetails;
    QList<int> primaryKeyIndex;
    QTableWidget *currentTable;

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFile();
    void openTab(int);
    void addRow();
    void removeRow();
    void selectSell();
    void sellSave();
    void sellClear();
    void toStart();
    void toPrev();
    void toNext();
    void toEnd();

};

#endif // MAINWINDOW_H
