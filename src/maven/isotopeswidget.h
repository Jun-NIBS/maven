#ifndef ISOTOPEWIDGETGUI_H
#define ISOTOPEWIDGETGUI_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "background_peaks_update.h"
#include "ui_isotopeswidget.h"
#include "numeric_treewidgetitem.h"


class QAction;
class QMenu;
class QTextEdit;
class MainWindow;
class BackgroundPeakUpdate;

extern Database DB;

using namespace std;

class IsotopeWidget: public QDockWidget, public Ui_isotopesWidget {
      Q_OBJECT

public:
      IsotopeWidget(MainWindow*);
      ~IsotopeWidget();

public slots: 
	  void setFormula(QString f);
      void updateAdduct();
	  void userChangedFormula(QString f);
	  void computeIsotopes(string f);
	  void setPeakGroup(PeakGroup* group);
	  void setCompound(Compound* compound);
	  void setPeak(Peak* peak);
	  void setClipboard();
	  void setClipboard(PeakGroup* group);
      void setClipboard(QList<PeakGroup*>& groups);
	  void pullIsotopes(PeakGroup* group);
	  Peak* getSamplePeak(PeakGroup* group, mzSample* sample);


private slots:
      void showInfo();
      void showTable();
	  QString groupTextEport(PeakGroup* group);
	  float getIsotopeIntensity(float mz);
   
private:
	  string _formula;

	  vector<mzLink> links;
      MainWindow* _mw;

      Scan* _scan;
	  PeakGroup* _group;
	  Compound*  _compound;
      Adduct* _adduct;

	  BackgroundPeakUpdate* workerThread;
	  MassCalculator mcalc;
	  Compound* tempCompound;

     QString groupIsotopeMatrixExport(PeakGroup* group, bool includeSampleHeader);
     Adduct* getCurrentAdduct();
      
};

#endif
