// To create an empty database in command line type: sqlite3 testSqlite.db "create table aTable(field1 int); drop table aTable;"

#include "mainwindow.h"

// Path of the CDB folder upto images folder
string dataPath = "/home/soumya/Shared_Vbox/cinema_project_codes/cinema_scope/data/volume-render/";
string dbPath = "/home/soumya/Shared_Vbox/cinema_project_codes/cinema_scope/data/volume-render/testSqlite.db";

int main(int argc, char *argv[])
{
    /////////////////////////////////////////////////////////////////
    /// Read cinema database and create a sqlite database
    QString path = QString::fromStdString(dbPath);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    db.open();

    QSqlQuery query;

    if ( !db.tables().contains( QLatin1String("dummyCinemaDB")) )
    {
        qDebug()<<query.exec("CREATE TABLE dummyCinemaDB (theta integer, phi integer, image varchar(100))");
        query.prepare("INSERT INTO dummyCinemaDB(theta,phi,image) values(:theta,:phi,:image)");
        for(int i=1;i<=50;i++)
        {
            for(int j=1;j<=50;j++)
            {
                query.bindValue(":theta", i);
                query.bindValue(":phi", j);

                stringstream ss;
                ss<<rand() % 1000 + 1; //randomely assign image id for now
                string fname = "images/" + ss.str() + ".jpg";
                query.bindValue(":image", QString::fromStdString(fname));

                //execute the query
                query.exec();
            }
        }
    }

    //Show the contents of the table: for testing only
    /*query.exec("SELECT * FROM dummyCinemaDB");
    while (query.next())
    {
        cout<<query.value(0).toString().toStdString()<<" "<<
              query.value(1).toString().toStdString()<<" "<<
              query.value(2).toString().toStdString()<<endl;
    }
    cout<<query.record().count()<<" "<<query.size()<<endl;*/

    /////////////////////////////////////////////////////////////////////
    /// Qt application code starts here for cinemaViewer

    QApplication cinemaViewer(argc, argv);
    cinemaViewer.setStyleSheet("QSlider {height:20px; width:200}");

    MainWindow mainWindow(dataPath);
    mainWindow.setWindowTitle("Qt-based CinemaDB Viewer");
    mainWindow.show();

    return cinemaViewer.exec();
}
