#include "mainwindow.h"
#include <QPluginLoader>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlRecord>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    window = new QWidget();
        controls = new QWidget();
            btnSelectFile = new QPushButton("Open *.sqlite file...");
            btnSelectFile->setFixedWidth(150);
            lblFileUrl = new QLabel("-= no opened file =-");
            lblFileUrl->setFixedWidth(200);
            lblFileUrl->setWordWrap(false);
            btnSaveFile = new QPushButton("Save");
            btnSaveFile->setEnabled(false);
            btnSaveAsFile = new QPushButton("Save as...");
            btnSaveAsFile->setEnabled(false);
            btnNewFile = new QPushButton("New database");
            btnNewFile->setEnabled(false);
        controlsLay = new QHBoxLayout();
        controlsLay->addWidget(btnSelectFile);
        controlsLay->addWidget(lblFileUrl);
        controlsLay->addWidget(btnSaveFile);
        controlsLay->addWidget(btnSaveAsFile);
        controlsLay->addWidget(btnNewFile);
        controls->setLayout(controlsLay);
        table = new QWidget();
            sell = new QWidget();
                tableManipulation = new QWidget();
                    btnAddRow = new QPushButton("add record");
                    btnAddRow->setEnabled(false);
                    btnRemoveRow = new QPushButton("remove record");
                    btnRemoveRow->setEnabled(false);
                    tableManipulationLay = new QHBoxLayout();
                    tableManipulationLay->addWidget(btnAddRow);
                    tableManipulationLay->addWidget(btnRemoveRow);
                    tableManipulation->setLayout(tableManipulationLay);
                sellNav = new QWidget();
                    navToStart = new QPushButton("<<");
                    navToPrev = new QPushButton("<-");
                    navToNext = new QPushButton("->");
                    navToEnd = new QPushButton(">>");
                    navToStart->setEnabled(false);
                    navToPrev->setEnabled(false);
                    navToNext->setEnabled(false);
                    navToEnd->setEnabled(false);
                    sellNavLay = new QHBoxLayout();
                    sellNavLay->addWidget(navToStart);
                    sellNavLay->addWidget(navToPrev);
                    sellNavLay->addWidget(navToNext);
                    sellNavLay->addWidget(navToEnd);
                sellNav->setLayout(sellNavLay);

                sellSet = new QWidget();
                    sellSetLay = new QFormLayout();
                sellSet->setLayout(sellSetLay);
                sellSet->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

                sellBtns = new QWidget();
                    btnSellSave = new QPushButton("Save sell");
                    btnSellCancel = new QPushButton("Cancel");
                    btnSellClear = new QPushButton("Clear");
                    settings = new QGroupBox("Settings");
                        chQuickWrite = new QCheckBox("quick edit mode");
                        chQuickWrite->setChecked(true);
                        chQuickWrite->setEnabled(false);
                        settingsLay = new QVBoxLayout();
                        settingsLay->addWidget(chQuickWrite);
                    settings->setLayout(settingsLay);
                    btnSellSave->setEnabled(false);
                    btnSellCancel->setEnabled(false);
                    btnSellClear->setEnabled(false);
                    sellBtnsLay = new QVBoxLayout();
                    sellBtnsLay->addWidget(btnSellSave);
                    sellBtnsLay->addWidget(btnSellCancel);
                    sellBtnsLay->addWidget(btnSellClear);
                    sellBtnsLay->addWidget(settings);
                sellBtns->setLayout(sellBtnsLay);
            sellLay = new QVBoxLayout();
            sellLay->addWidget(tableManipulation);
            sellLay->addWidget(sellNav);
            sellLay->addWidget(sellSet);
            sellLay->addWidget(sellBtns);
            sell->setLayout(sellLay);
            sell->setFixedWidth(350);
            tables = new QTabWidget();
            tables->setMinimumSize(500, 350);
        tableLay = new QHBoxLayout();
        tableLay->addWidget(sell);
        tableLay->addWidget(tables);
        table->setLayout(tableLay);
    layout = new QVBoxLayout();
    layout->addWidget(controls);
    layout->addWidget(table);
    window->setLayout(layout);
    setCentralWidget(window);

    db = QSqlDatabase::addDatabase("QSQLITE");

    QObject::connect(btnSelectFile, SIGNAL(clicked(bool)), this, SLOT(openFile()));
    QObject::connect(tables, SIGNAL(currentChanged(int)), this, SLOT(openTab(int)));

    QObject::connect(btnAddRow, SIGNAL(clicked(bool)), this, SLOT(addRow()));
    QObject::connect(btnRemoveRow, SIGNAL(clicked(bool)), this, SLOT(removeRow()));

    QObject::connect(navToEnd, SIGNAL(clicked(bool)), this, SLOT(toEnd()));
    QObject::connect(navToPrev, SIGNAL(clicked(bool)), this, SLOT(toPrev()));
    QObject::connect(navToNext, SIGNAL(clicked(bool)), this, SLOT(toNext()));
    QObject::connect(navToStart, SIGNAL(clicked(bool)), this, SLOT(toStart()));

    QObject::connect(btnSellSave, SIGNAL(clicked(bool)), this, SLOT(sellSave()));
    QObject::connect(btnSellCancel, SIGNAL(clicked(bool)), this, SLOT(selectSell()));
    QObject::connect(btnSellClear, SIGNAL(clicked(bool)), this, SLOT(sellClear()));

}

void MainWindow::openFile(){

    // open file dialog
    QString fileUrl = QFileDialog::getOpenFileName(this, tr("Open DataBase"), "/home", tr("SQLITE data base files (*.sqlite)"));
    lblFileUrl->setText(fileUrl);
    db.setDatabaseName(fileUrl);
    if( !db.open() )
    {
      qFatal( "Failed to connect." );
    }
    qDebug( "Connected!" );

    // search and import all tables
    query = new QSqlQuery("SELECT name FROM sqlite_master WHERE type = 'table'");
    while (query->next()) { // all tables
        qDebug() << "------------\n" << "TABLE =" << query->value(0).toString();

        // create structure for db fields details
        QList<QStringList *> *tableDetails = new QList<QStringList *>();
        QStringList *fieldsNames = new QStringList();
        QStringList *fieldsType = new QStringList();
        QStringList *fieldsNull = new QStringList();
        QStringList *fieldsDefault = new QStringList();
        tableDetails->append(fieldsNames);
        tableDetails->append(fieldsType);
        tableDetails->append(fieldsNull);
        tableDetails->append(fieldsDefault);
        dbDetails.append(tableDetails);

        QTableWidget *newTable = new QTableWidget();
        QObject::connect(newTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(selectSell()));
        // get columns data (names and count in list 'fields')
        QSqlQuery queryInfo(QString("PRAGMA TABLE_INFO(%1)").arg(query->value(0).toString()));
        while(queryInfo.next()){
            fieldsNames->append(queryInfo.value(1).toString());
            qDebug() << "|- columnName =" << queryInfo.value(1).toString();
            fieldsType->append(queryInfo.value(2).toString());
            qDebug() << "   |- columnType =" << queryInfo.value(2).toString();
            fieldsNull->append(queryInfo.value(3).toString());
            qDebug() << "   |- columnNull =" << queryInfo.value(3).toString();
            fieldsDefault->append(queryInfo.value(4).toString());
            qDebug() << "   |- columnDefaultValue =" << queryInfo.value(4).toString();
        }
        qDebug() << "|- columnCount =" << fieldsNames->size();

        // fill column data
        newTable->setColumnCount(fieldsNames->size());
        for(int i = 0; i < fieldsNames->size(); i++){
            newTable->setHorizontalHeaderItem(i, new QTableWidgetItem(fieldsNames->value(i)));
            newTable->horizontalHeaderItem(i)->setToolTip(fieldsType->at(i));
        }

        // get index of primary key
        primaryKeyIndex << db.primaryIndex(query->value(0).toString()).value(0).toInt();
        qDebug() << "|- primary key index =" << QString::number(primaryKeyIndex.last());

        // get count of records
        QSqlQuery querySeq(QString("SELECT seq FROM sqlite_sequence WHERE name = '%1'").arg(query->value(0).toString()));
        int row = -1;
        while(querySeq.next())
            row = querySeq.value(0).toInt();
        //newTable->setRowCount(row);
        newTable->setRowCount(0);
        qDebug() << "|- rowCount from sqlite_sequence =" << row;

        // write data in table
        QSqlQuery queryData(QString("SELECT * FROM %1").arg(query->value(0).toString()));
        qDebug() << "|- queryData =" << queryData.lastQuery();
        int currentRow = 0;
        while(queryData.next()){
            newTable->setRowCount(newTable->rowCount()+1);
            qDebug() << "  |- row =" << currentRow;
            for(int i = 0; i < fieldsNames->size(); i++){
                qDebug() << "   |- value =" << queryData.value(i).toString();
                if(fieldsType->value(i) == "INTEGER"){
                    QSpinBox *item = new QSpinBox();
                    item->setValue(queryData.value(i).toInt());
                    if(primaryKeyIndex.last() == i)
                        item->setEnabled(false);
                    newTable->setCellWidget(currentRow, i, item);
                } else {
                    QTableWidgetItem *item = new QTableWidgetItem(queryData.value(i).toString());
                    if(primaryKeyIndex.last() == i)
                        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    newTable->setItem(currentRow, i, item);
                }
            }
            currentRow++;
        }

        // add new table as tab
        tables->addTab(newTable, query->value(0).toString());
    }
    currentTable = qobject_cast<QTableWidget *>(tables->currentWidget());
    db.close();


}

void MainWindow::openTab(int currentTab){
    qDebug() << "Selected tab " << currentTab;
    sellSet->setEnabled(false);
    while ( sellSetLay->count() != 0)
    {
        QLayoutItem *forDeletion = sellSetLay->takeAt(0);
        delete forDeletion->widget();
        delete forDeletion;
    }
    currentTable = qobject_cast<QTableWidget *>(tables->currentWidget());
    for(int i = 0; i < currentTable->columnCount(); i++){
        if(dbDetails.at(tables->currentIndex())->at(1)->at(i) == "INTEGER")
            sellSetLay->addRow(currentTable->horizontalHeaderItem(i)->text(), new QSpinBox());
        else
            sellSetLay->addRow(currentTable->horizontalHeaderItem(i)->text(), new QLineEdit());
        sellSetLay->itemAt(sellSetLay->rowCount() - 1, QFormLayout::FieldRole)->widget()->setToolTip(dbDetails.at(tables->currentIndex())->at(1)->at(i));
    }
    if(currentTable->itemAt(0,0) || currentTable->cellWidget(0,0)){
        navToStart->setEnabled(true);
        navToPrev->setEnabled(true);
        navToNext->setEnabled(true);
        navToEnd->setEnabled(true);
        btnAddRow->setEnabled(true);
        btnRemoveRow->setEnabled(true);
    }
    else{
        navToStart->setEnabled(false);
        navToPrev->setEnabled(false);
        navToNext->setEnabled(false);
        navToEnd->setEnabled(false);
        btnRemoveRow->setEnabled(false);
    }
}

void MainWindow::selectSell(){
    qDebug() << "Selected sell (" <<  currentTable->currentColumn() << "," << currentTable->currentRow() << ")";
    currentTable->selectRow(currentTable->currentRow());
    sellSet->setEnabled(true);
    btnSellSave->setEnabled(true);
    btnSellCancel->setEnabled(true);
    btnSellClear->setEnabled(true);
    for(int i = 0; i < currentTable->columnCount(); i++){
        int j = i==0 ? 1 : 2*i+1;
        if(dbDetails.at(tables->currentIndex())->at(1)->value(i) == "INTEGER"){
            QSpinBox *param = qobject_cast<QSpinBox *>((sellSetLay->itemAt(j))->widget());
            QSpinBox *cell = qobject_cast<QSpinBox *>(currentTable->cellWidget(currentTable->currentRow(), i));
            param->setValue(cell->value());
            param->setEnabled(cell->isEnabled());
            qDebug() << "|-" << currentTable->horizontalHeaderItem(i)->text() << ":" << QString::number(param->value());
        } else {
            QLineEdit *param = qobject_cast<QLineEdit *>((sellSetLay->itemAt(j))->widget());
            param->setText(currentTable->item(currentTable->currentRow(), i)->text());
            qDebug() << "|-" << currentTable->horizontalHeaderItem(i)->text() << ":" << currentTable->item(currentTable->currentRow(), i)->text();
        }
    }
}

void MainWindow::addRow(){
    // DB
    db.open();
    if( !db.open() )
    {
      qFatal( "Failed to connect." );
    }
    QString str = "INSERT INTO ";
    str.append(tables->tabBar()->tabText(tables->currentIndex()));
    str.append(" (");
    QStringList headers;
    for(int i = 0; i < currentTable->columnCount(); i++){
        if(primaryKeyIndex.value(tables->currentIndex()) != i){
            headers << currentTable->horizontalHeaderItem(i)->text();
        }
    }
    for(int i = 0; i < headers.count(); i++){
        str.append(headers.value(i));
        if(i != (headers.count() - 1))
            str.append(", ");
    }
    str.append(") VALUES (");
    for(int i = 0; i < headers.count(); i++){
        if(dbDetails.at(tables->currentIndex())->at(3)->at((dbDetails.at(tables->currentIndex())->at(0)->indexOf(headers.value(i)))) == "")
            str.append(dbDetails.at(tables->currentIndex())->at(3)->at((dbDetails.at(tables->currentIndex())->at(0)->indexOf(headers.value(i)))));
        else{
            if(dbDetails.at(tables->currentIndex())->at(1)->at(dbDetails.at(tables->currentIndex())->at(0)->indexOf(headers.value(i))) == "INTEGER")
                str.append("0");
            else
                str.append("-");
        }
        if(i != (headers.count() - 1))
            str.append(", ");
    }
    str.append(");");
    qDebug() << "query: " << str;
    QSqlQuery *query = new QSqlQuery(str);
    db.close();
    delete query;

    // QTableWidget
    currentTable->setRowCount(currentTable->rowCount() + 1);
    for(int i = 0; i < currentTable->columnCount(); i++){
        if(dbDetails.at(tables->currentIndex())->at(2)->at(i) == "INTEGER"){
            QSpinBox *spin = new QSpinBox();
            spin->setValue(dbDetails.at(tables->currentIndex())->at(3)->at(i).toInt());
            if(primaryKeyIndex.value(tables->currentIndex()) == i)
                spin->setEnabled(false);
            currentTable->setCellWidget(currentTable->rowCount() - 1, i, spin);
        } else {
            currentTable->setItem(currentTable->rowCount() - 1, i, new QTableWidgetItem());
        }
    }
    currentTable->setCurrentCell(currentTable->rowCount() - 1, 1);
    emit selectSell();
}

void MainWindow::removeRow(){
    // DB
    db.open();
    if( !db.open() )
    {
      qFatal( "Failed to connect." );
    }
    QString str = "DELETE FROM ";
    str.append(tables->tabBar()->tabText(tables->currentIndex()));
    str.append(" WHERE ");
    str.append(dbDetails.at(tables->currentIndex())->at(0)->at(primaryKeyIndex.value(tables->currentIndex())));
    str.append("=");
    if(dbDetails.at(tables->currentIndex())->at(1)->at(primaryKeyIndex.value(tables->currentIndex())) == "INTEGER"){
        QSpinBox *spin = qobject_cast<QSpinBox *>(currentTable->cellWidget(currentTable->currentRow(), primaryKeyIndex.value(tables->currentIndex())));
        str.append(QString::number(spin->value()));
    } else {
        str.append(currentTable->item(currentTable->currentRow(), primaryKeyIndex.value(tables->currentIndex()))->text());
    }
    str.append(";");
    qDebug() << "query: " << str;
    QSqlQuery *query = new QSqlQuery(str);
    db.close();
    delete query;

    // QTableWidget
    int currentRow = currentTable->currentRow();
    currentTable->hideRow(currentRow);
    if(currentRow != 0){
        currentTable->setCurrentCell(currentRow - 1, 1);
        emit selectSell();
    }
    else{
        navToStart->setEnabled(false);
        navToPrev->setEnabled(false);
        navToNext->setEnabled(false);
        navToEnd->setEnabled(false);
        btnRemoveRow->setEnabled(false);
    }
}

void MainWindow::sellSave(){
    // QTableWidget
    int row = currentTable->currentRow();
    qDebug() << "---saving---\n" << "current row :" << row;
    for(int i = 0; i < currentTable->columnCount(); i++){
        int j = i==0 ? 1 : 2*i+1;
        QLineEdit *param = qobject_cast<QLineEdit *>(sellSetLay->itemAt(j)->widget());
        currentTable->item(row, i)->setText(param->text());
        qDebug() << currentTable->horizontalHeaderItem(i)->text() << ":" << param->text();
    }

    // DB
    db.open();
    if( !db.open() )
    {
      qFatal( "Failed to connect." );
    }
    QString str = "UPDATE ";
    str.append(tables->tabBar()->tabText(primaryKeyIndex.value(tables->currentIndex())));
    str.append(" SET ");
    QStringList *headers = new QStringList();
    for(int i = 0; i < currentTable->columnCount(); i++){
        if(primaryKeyIndex.at(tables->currentIndex()) != i){
            headers->append(*dbDetails.at(tables->currentIndex())->at(i));
        }
    }
    for(int i = 0; i < headers->count(); i++){
        str.append(headers->at(i));
        str.append("=");
        if(dbDetails.at(tables->currentIndex())->at(1)->at(dbDetails.at(tables->currentIndex())->at(0)->indexOf(headers->at(i))) == "INTEGER"){
            QSpinBox *spin = qobject_cast<QSpinBox *>(currentTable->cellWidget(currentTable->currentRow(), dbDetails.at(tables->currentIndex())->at(0)->indexOf(headers->at(i))));
            str.append(QString::number(spin->value()));
        } else {
            str.append(currentTable->item(currentTable->currentRow(), dbDetails.at(tables->currentIndex())->at(0)->indexOf(headers->at(i)))->text());
        }
        if(i != headers->count() - 1)
            str.append(", ");
    }
    str.append(" WHERE ");
    str.append(dbDetails.at(tables->currentIndex())->at(0)->at(primaryKeyIndex.value(tables->currentIndex())));
    str.append("=");
    if(dbDetails.at(tables->currentIndex())->at(1)->at(primaryKeyIndex.value(tables->currentIndex())) == "INTEGER"){
        QSpinBox *spin = qobject_cast<QSpinBox *>(currentTable->cellWidget(currentTable->currentRow(), primaryKeyIndex.value(tables->currentIndex())));
        str.append(QString::number(spin->value()));
    } else {
        str.append(currentTable->item(currentTable->currentRow(), primaryKeyIndex.value(tables->currentIndex()))->text());
    }
    str.append(";");
    qDebug() << "query: " << str;
    //QSqlQuery *query = new QSqlQuery(str);
    db.close();
    delete query;
}

void MainWindow::sellClear(){
    for(int i = 0; i < currentTable->columnCount(); i++){
        int j = i==0 ? 1 : 2*i+1;
        QLineEdit *param = qobject_cast<QLineEdit *>(sellSetLay->itemAt(j)->widget());
        if(param->isEnabled())
            param->clear();
    }
}

void MainWindow::toStart(){
    currentTable->setCurrentCell(0, currentTable->currentColumn());
    emit selectSell();
}

void MainWindow::toPrev(){
    if(currentTable->currentRow() != 0)
        currentTable->setCurrentCell(currentTable->currentRow() - 1, currentTable->currentColumn());
    emit selectSell();
}

void MainWindow::toNext(){
    if(currentTable->currentRow() != currentTable->rowCount() - 1)
        currentTable->setCurrentCell(currentTable->currentRow() + 1, currentTable->currentColumn());
    emit selectSell();
}

void MainWindow::toEnd(){
    currentTable->setCurrentCell(currentTable->rowCount() - 1, currentTable->currentColumn());
    emit selectSell();
}

MainWindow::~MainWindow()
{
    db.close();
}
