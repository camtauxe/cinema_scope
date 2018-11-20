#include "mainwindow.h"
#include <QObject>
#include <QSizePolicy>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QSplitter>
#include "CinDBReader.h"
#include "CinParamSliders.h"
#include "CinDatabase.h"
#include "CinParameter.h"
#include "CinParamSet.h"
#include "CinImageViewer.h"

MainWindow::~MainWindow()
{

}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        cout<<"On left mouse release"<<endl;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        cout<<"On left mouse press"<<endl;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint p = e->pos();
    cout<<"MAINWINDOW: moving mouse loc: "<< p.rx()<<" "<<p.ry()<<endl;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->buildApplication(parent);
}

void MainWindow::buildApplication(QWidget *parent)
{
    // create the database and cinema reaader
    this->mCDB = new CinDatabase();
    this->mDatabase = this->mCDB->TEMPGetDatabase();
    // this->mDatabase = QSqlDatabase::addDatabase("QSQLITE");
    // this->mDatabase.open();
    // this->mReader = new CinDBReader();

    // remember the table name
    this->mTableName = "cinema";

    // create the basic components
    QWidget     *mainWidget       = new QWidget(parent);
    QVBoxLayout *mainWidgetLayout = new QVBoxLayout;
    this->splitter         = new QSplitter(Qt::Horizontal, mainWidget);
    this->mImagePanel   = new QWidget();
    this->mImageLayout  = new QVBoxLayout;
    this->mSliderLayout = new QFormLayout;

    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainWidget->setLayout(mainWidgetLayout);
    mainWidgetLayout->addWidget(splitter);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->addWidget(this->mImagePanel);

    mImagePanel->setLayout(mImageLayout);
    mImagePanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // colors for testing
    // mainWidget->setStyleSheet("background-color:red");
    // splitter->setStyleSheet("background-color:blue");

    // set up initial state
    setCentralWidget(mainWidget);
    resize(1200,600);
    createActions();
    setUnifiedTitleAndToolBarOnMac(true);
    menuBar()->setNativeMenuBar(false);

    // image and scene
    mScene = new QGraphicsScene();
    mImageView = new CinImageView(mImagePanel);
    mImageView->sceneObj = mScene;
    mImageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mImageView->mTableName = mTableName;
}

void MainWindow::loadCinemaDatabase(const QString &database)
{
    // clean up all UI components
    this->flushUI();

    // remember this DB
    this->mCurDatabase = database;
    this->mImageView->dbPath = database;

    // Cin* class testing
    mCDB->loadDatabase(database, this->mTableName);
    CinParamSliders *dbSliders = new CinParamSliders();
    mParamSet = new CinParamSet();
    mParamSet->setDatabase(mCDB);
    mParamSet->print();
    dbSliders->connect(mCDB, mParamSet);
    this->splitter->addWidget(dbSliders);
    QObject::connect(dbSliders, SIGNAL(artifactSelected(QString &)), mImageView, SLOT(onLoadImage(QString &)));

    mImageView->paramSet = mParamSet; //pointing to parameter set

    // load database
    // mReader->readCinemaDatabase(this->mDatabase, database, this->mTableName);
    // mDatabase = mCinDatabase->TEMPGetDatabase();
    // mReader->readCinemaDatabase(this->mDatabase, database, this->mTableName);
    // mDatabase = mCinDatabase->TEMPGetDatabase();

    QSqlQuery qry;
    string queryText = "SELECT * FROM " + this->mTableName.toStdString();
    qry.exec(queryText.c_str());

    //Get number of sliders = number of columns-1
    this->numSliders = qry.record().count()-1;
    cout<<"Number of columns: "<<this->numSliders+1<<endl;

    //load and display an initial image
    queryText = "SELECT * FROM " + this->mTableName.toStdString();
    qry.exec(queryText.c_str());
    QString initFileID;
    qry.first();
    //get the value of last column which is the image path
    // TODO: query for FILE column
    initFileID = qry.value(this->numSliders).toString();

    QString imagePath = database;
    imagePath += "/" + initFileID; //loads the first image from first row in the db
    QPixmap image;
    if(!mImageView->loadImage(imagePath,&image))
    {
        //image.fill(Qt::transparent); // shows a blank screen
        cout<<"image loading failed"<<endl;
        imagePath = database;
        imagePath += "/empty_image/empty.png";
        mImageView->loadImage(imagePath,&image);
    }

    this->mScene->addPixmap(image);
    this->mImageView->setScene(this->mScene);
    this->mImageLayout->addWidget(this->mImageView);
}


void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    mOpenAction = fileMenu->addAction(tr("&Open ..."), this, &MainWindow::onOpenFile);
    mOpenAction->setShortcut(QKeySequence::Open);
    mOpenAction->setStatusTip(tr("Open a file"));

    fileMenu->addSeparator();

    mQuitAction = fileMenu->addAction(tr("E&xit"), this, &MainWindow::onQuit);
    mQuitAction->setShortcut(tr("Ctrl+Q"));
    mQuitAction->setStatusTip(tr("Quit application"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, &MainWindow::onAbout);
}

void MainWindow::onOpenFile()
{

    QString result = QFileDialog::getExistingDirectory(this,
                                                       tr("Open Cinema Database"), "/",
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    this->loadCinemaDatabase(result);
}

void MainWindow::onQuit()
{
    QApplication::quit();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("CinemaScope"),
                       tr("This is the about message"));
}

void MainWindow::flushUI()
{
    // flush the database
    QSqlQuery qry;
    qry.exec("DROP TABLE " + this->mTableName);
}
