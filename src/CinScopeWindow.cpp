#include "CinScopeWindow.h"
#include <QObject>
#include <QSizePolicy>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QSplitter>
#include <QSettings>
#include <QList>
#include "CinCore.h"
#include "CinDBReader.h"
#include "CinParamSliders.h"
#include "CinDatabase.h"
#include "CinParameter.h"
#include "CinParamSet.h"
#include "CinImageView.h"
#include "CinParameterMapDialog.h"

CinScopeWindow::~CinScopeWindow()
{
}

void CinScopeWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        cout<<"On left mouse release"<<endl;
    }
}

void CinScopeWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        cout<<"On left mouse press"<<endl;
    }
}

void CinScopeWindow::mouseMoveEvent(QMouseEvent *e)
{
    QPoint p = e->pos();
    cout<<"MAINWINDOW: moving mouse loc: "<< p.rx()<<" "<<p.ry()<<endl;
}

CinScopeWindow::CinScopeWindow(QWidget *parent) : QMainWindow(parent)
{
    // settings
    mSettingsFile = QDir::homePath() + "/.cinema/scope/settings.ini";
    // QSettings settings(mSettingsFile, QSettings::IniFormat);

    buildApplication(parent);
}

void CinScopeWindow::buildApplication(QWidget *parent)
{
    // build the DB and all its object
    mDBV = new CinDBView();
    CinDatabase *db = new CinDatabase();
    mDBV->setDatabase(db);

    // create the UI components
    QWidget     *mainWidget       = new QWidget(parent);
    QVBoxLayout *mainWidgetLayout = new QVBoxLayout;
    mSplitter  = new QSplitter(Qt::Horizontal, mainWidget);
    mSliders   = new CinParamSliders();
    mImageView = new CinImageView(mSplitter);
    mImageView->setParameters(mDBV->getParameters());

    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainWidget->setLayout(mainWidgetLayout);
    mainWidgetLayout->addWidget(mSplitter);

    mSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mSliders->connect(mDBV->getDatabase(), mDBV->getParameters());

    // set up initial state
    setCentralWidget(mainWidget);
    resize(900,600);
    createActions();
    setUnifiedTitleAndToolBarOnMac(true);
    menuBar()->setNativeMenuBar(false);

    // image and scene
    mScene = new QGraphicsScene();
    mImageView->setScene(mScene);
    mImageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mSplitter->addWidget(mImageView);
    mSplitter->addWidget(mSliders);
    mSplitter->setSizes(QList<int>({INT_MAX, INT_MAX}));

    // connect slots
    QObject::connect(mDBV,       SIGNAL(artifactChanged(const QString &, const QString &)), 
                     mImageView, SLOT(onLoadImage(const QString &, const QString &)));

}

void CinScopeWindow::loadCinemaDatabase(const QString &database)
{
    // remember this DB
    mCurDatabase = database;

    // clear the view
    mImageView->clear();

    // determine if there are phi, theta and FILE parameters, or map them
    mDBV->reset();
    mDBV->getDatabase()->load(database);
    mDBV->initializeAttributes();

    bool pBool = mDBV->parameterExists("phi");
    bool tBool = mDBV->parameterExists("theta");
    bool fBool = mDBV->artifactExists("FILE");
    if (pBool && tBool && fBool)
    {
        // we are fine 
        qDebug() << "DATABASE PASSES";
    } else {
        CinParameterMapDialog map(this);
        map.connect(mDBV, mImageView);
        map.setWindowTitle("Map Parameters Dialog");
        map.exec();
    }

    // clean up all UI components
    flushUI();

    //
    mSliders->buildSliders();
    // mImageView->setParameters(mDBV->getParameters());
    mDBV->updateArtifacts();
}


void CinScopeWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    mOpenAction = fileMenu->addAction(tr("&Open ..."), this, &CinScopeWindow::onOpenFile);
    mOpenAction->setShortcut(QKeySequence::Open);
    mOpenAction->setStatusTip(tr("Open a file"));

    fileMenu->addSeparator();

    mQuitAction = fileMenu->addAction(tr("E&xit"), this, &CinScopeWindow::onQuit);
    mQuitAction->setShortcut(tr("Ctrl+Q"));
    mQuitAction->setStatusTip(tr("Quit application"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, &CinScopeWindow::onAbout);
}

void CinScopeWindow::onOpenFile()
{
    QSettings settings(mSettingsFile, QSettings::IniFormat);
    QString dataDir = settings.value("cinemascope/data").toString(); 
    // qDebug() << "SETTINGS: " << dataDir;
    QString result = QFileDialog::getExistingDirectory(this,
                         tr("Open Cinema Database"), dataDir, 
                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    loadCinemaDatabase(result);
}

void CinScopeWindow::onQuit()
{
    QApplication::quit();
}

void CinScopeWindow::onAbout()
{
    QMessageBox::about(this, tr("CinemaScope"),
                       tr("Cinema Scope v0.1"));
}

void CinScopeWindow::flushUI()
{
    mSliders->deleteSliders();
}
