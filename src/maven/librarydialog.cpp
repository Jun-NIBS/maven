#include "librarydialog.h"

LibraryMangerDialog::LibraryMangerDialog(QWidget *parent) : QDialog(parent) {
        setupUi(this);
        setModal(true);
        connect(loadButton,SIGNAL(clicked(bool)),SLOT(loadLibrary()));
        connect(deleteButton,SIGNAL(clicked(bool)),SLOT(deleteLibrary()));
        connect(importButton,SIGNAL(clicked()),SLOT(loadCompoundsFile()));
        connect(reloadAllButton,SIGNAL(clicked()),SLOT(reloadMethodsFolder()));
        connect(btnUnload, SIGNAL(clicked()), SLOT(unloadLibrary()));
        connect(btnUnloadAll, SIGNAL(clicked()), SLOT(unloadAllLibraries()));
}



void LibraryMangerDialog::show() {
    updateLibraryStats();
    this->treeWidget->resizeColumnToContents(0);
    QDialog::show();
}

void LibraryMangerDialog::updateLibraryStats() {

    QMap<QString,int> dbcounts;
    for(Compound* c: DB.compoundsDB)  dbcounts[c->db.c_str()]++;

    treeWidget->clear();
    for(QString dbname: DB.getDatabaseNames()) {
        QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
        item->setText(0,dbname);
        item->setText(1,QString::number(dbcounts[dbname]));
        treeWidget->addTopLevelItem(item);
    }

    if (mainwindow && mainwindow->setRumsDBDialog) {
        mainwindow->setRumsDBDialog->updateComboBox();
    }
}

void LibraryMangerDialog::loadLibrary() {
    foreach(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        QString libraryName = item->text(0);
        qDebug() << "Load Library" << libraryName;
        DB.loadCompoundsSQL(libraryName,DB.getLigandDB());

        QStringList dataLocations = QStandardPaths::standardLocations(QStandardPaths::DataLocation);

        for (QString dataLocation : dataLocations) {
            qDebug() << "Maven SQLite library ligand.db may be located at:" << dataLocation;
        }
    }
    updateLibraryStats();
}

void LibraryMangerDialog::deleteLibrary() {

    foreach(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        QString libraryName = item->text(0);
        qDebug() << "LibraryMangerDialog::deleteLibrary(): Delete Library" << libraryName;
        DB.deleteCompoundsSQL(libraryName,DB.getLigandDB());
    }
    updateLibraryStats();
}

void LibraryMangerDialog::unloadLibrary() {

    foreach(QTreeWidgetItem* item, treeWidget->selectedItems() ) {
        QString libraryName = item->text(0);
        qDebug() << "LibraryMangerDialog::unloadLibrary(): Unload Library" << libraryName;
        DB.unloadCompounds(libraryName);
    }
    updateLibraryStats();
}

void LibraryMangerDialog::unloadAllLibraries() {
    qDebug() << "LibraryMangerDialog::unloadAllLibraries()";
    DB.unloadAllCompounds();
    updateLibraryStats();
}

void LibraryMangerDialog::loadCompoundsFile(QString filename){
    string dbfilename = filename.toStdString();
    string dbname = mzUtils::cleanFilename(dbfilename);

    int compoundCount = DB.loadCompoundsFile(filename);
    DB.loadCompoundsSQL(dbname.c_str(),DB.getLigandDB());

    if (compoundCount > 0 && mainwindow->ligandWidget) {
        mainwindow->ligandWidget->updateDatabaseList();
        mainwindow->massCalcWidget->updateDatabaseList();

        if( mainwindow->ligandWidget->isVisible() )
            mainwindow->ligandWidget->setDatabase(QString(dbname.c_str()));
    }


    QSettings* settings = mainwindow->getSettings();
    settings->setValue("lastDatabaseFile",filename);
    updateLibraryStats();
}

void LibraryMangerDialog::loadCompoundsFile() {
    QSettings* settings = mainwindow->getSettings();
    QString methodsFolder =      settings->value("methodsFolder").value<QString>();

    QStringList filelist = QFileDialog::getOpenFileNames(this,
            "Select Compounds File To Load",
             methodsFolder,
            "All Known Formats(*.csv *.tab *.txt *.msp *.sptxt *.pepXML);;Tab Delimited(*.tab);;CSV File(*.csv);;NIST Library(*.msp);;SpectraST(*.sptxt) pepXML(*.pepXML)");

    if ( filelist.size() == 0 || filelist[0].isEmpty() ) return;
    for( QString filename: filelist) {
        loadCompoundsFile(filename);
    }
}

void LibraryMangerDialog::reloadMethodsFolder() {
    QSettings* settings = mainwindow->getSettings();
    QString methodsFolder =      settings->value("methodsFolder").value<QString>();
    methodsFolder = QFileDialog::getExistingDirectory(this,methodsFolder);

    if (not methodsFolder.isEmpty()) {
        DB.deleteAllCompoundsSQL(DB.getLigandDB());
        DB.loadMethodsFolder(methodsFolder);
        mainwindow->ligandWidget->updateDatabaseList();
        mainwindow->massCalcWidget->updateDatabaseList();
    }
    updateLibraryStats();
}
