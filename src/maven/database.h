#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QDebug>


#include "mzSample.h"
#include "mzUtils.h"

class Database {

    public:
        Database() {
            //empty constructor
        }

        Database(QString filename) {
            connect(filename);
            reloadAll();
        }
        //~Database() { closeAll(); }

		QSqlDatabase& getLigandDB() { return ligandDB; }
        void reloadAll();  //loads all tables
        void closeAll();
        bool connect(QString filename);
        bool disconnect();
    
        vector<Adduct*> loadAdducts(string filename);
        vector<Adduct*> defaultAdducts();

        void loadMethodsFolder(QString methodsFolder);
        int  loadCompoundsFile(QString filename);
        vector<Compound*> loadCompoundCSVFile(QString fileName);
        vector<Compound*> loadNISTLibrary(QString fileName);

        void loadCompoundsSQL(QString databaseName,QSqlDatabase& dbConnection);
        void saveCompoundsSQL(vector<Compound*> &compoundSet, QSqlDatabase& dbConnection, bool isUpdateOldDBVersion);
        void deleteCompoundsSQL(QString dbName, QSqlDatabase& dbConnection);
        void deleteAllCompoundsSQL(QSqlDatabase& dbConnection);

		multimap<string,Compound*> keywordSearch(string needle);
		vector<string>   getCompoundReactions(string compound_id);

		void addCompound(Compound*c);
        void loadReactions(string modelName);

        vector<Compound*> getCopoundsSubset(string database);
        vector<Compound*> getKnowns();


        QStringList getDatabaseNames();
        QStringList getLoadedDatabaseNames() { return loadedDatabase.keys(); }
        map<string,int>   getChromotographyMethods();

        Compound* findSpeciesById(string id,string db);
		Compound* findSpeciesByPrecursor(float precursorMz, float productMz,int polarity,double amuQ1, double amuQ3);
        set<Compound*> findSpeciesByMass(float mz, float ppm);
        vector<Compound*> findSpeciesByName(string name, string dbname);
        Compound* findSpeciesByNameAndAdduct(string name, string dbname, string adduct);

        vector<MassCalculator::Match> findMatchingCompounds(float mz, float ppm, float charge);
        vector<MassCalculator::Match> findMathchingCompoundsSLOW(float mz, float ppm, float charge);

        Adduct* findAdductByName(string id);

		void loadRetentionTimes(QString method);
		void saveRetentionTime(Compound* c, float rt, QString method);

        void saveValidation(Peak* p);

        vector<Adduct*> adductsDB;
        vector<Adduct*> fragmentsDB;
        vector<Compound*> compoundsDB;


      private:
		QSqlDatabase ligandDB;
        QMap<string,Compound*> compoundIdMap;
        QMap<QString,int> loadedDatabase;

};


#endif

