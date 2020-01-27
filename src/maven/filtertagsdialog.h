#ifndef FILTERTAGSDIALOG_H
#define FILTERTAGSDIALOG_H

#include "ui_filtertagsdialog.h"
#include "stable.h"
#include "database.h"
#include "mzSample.h"

extern Database DB;

class FilterTagsDialog : public QDialog, public Ui_filterTagsDialog {

    Q_OBJECT

    public:
        FilterTagsDialog(QWidget *parent);

    private:
        map<QTableWidgetItem*, PeakGroupTag*> checkBoxTag = {};

        QTableWidgetItem *untaggedPeakGroup = nullptr;
        QTableWidgetItem *goodTag = nullptr;
        QTableWidgetItem *badTag = nullptr;
};

#endif // FILTERTAGSDIALOG_H