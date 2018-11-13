#ifndef CINDBSLIDERS_H
#define CINDBSLIDERS_H

#include <QWidget>
#include <QFormLayout>
#include <QSqlDatabase>
#include <QSlider>
#include <QLabel>
#include "CinDatabase.h"
#include "CinParamSet.h"

/*! \brief Manages a set of parameter sliders for a Cinema Database
 *
 * The class builds/rebuilds a set of sliders that select valid values
 * for a set of parameters in a Cinema Database. The set of paramaters
 * managed need not be the full set of parameters available in the database.
 * 
 * An instance of this class emits the following signals:
 *
 *    artifactSelected(QString &) 
*/
class CinParamSliders : public QWidget
{
    Q_OBJECT

public:
    CinParamSliders();
    void connect(CinDatabase *cdb, CinParamSet *params);

private:
    void     buildSliders();
    void     reset();
    void     constructQueryString();
    void     popSlidersToValidValue();
    QSlider *getSliderAt(int i);
    QLabel  *getLabelAt(int i);

    QFormLayout *mSliderLayout=NULL; /*!< The layout object for the sliders */
    QString      mSliderQuery;       /*!< Common query over all sliders */
    CinDatabase *mCurDatabase=NULL;  /*!< Common database object */ 
    CinParamSet *mParameters=NULL;
    
private slots:
    void onSliderValueChanged(int value);
    void onParameterValueChanged(QString &name, float value);

signals:
    void artifactSelected(QString &);
};


#endif // CINDBSLIDERS_H
