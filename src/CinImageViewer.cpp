#include "CinImageViewer.h"

#include <string>
#include <iostream>

using namespace std;

bool CinImageView::loadImage(QString path, QPixmap *image )
{
    return image->load(path);
}

string CinImageView::constructQueryString(QStringList currentParamnames)
{
    string query;

    query = "SELECT * FROM " + mTableName.toStdString() + " WHERE ";
    for(int i=0;i<currentParamnames.size();i++)
    {
        if(i<currentParamnames.size()-1)
            query = query + currentParamnames[i].toStdString() + "=:" + currentParamnames[i].toStdString() + " AND ";
        else
            query = query + currentParamnames[i].toStdString() + "=:" + currentParamnames[i].toStdString();
    }
    return query;
}

void CinImageView::onLoadImage(QString &path)
{
    QPixmap pixmap;

    // qDebug() << "CINIMAGEVIEW: " << path;

    loadImage(path, &pixmap);
    sceneObj->addPixmap(pixmap);
}

void CinImageView::wheelEvent(QWheelEvent * event)
{
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);

    } else {
        // Zooming out
         scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void CinImageView :: mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        //cout<<"CINIMAGE: on left mouse press"<<endl;
        currentXloc = e->pos().rx();
        currentYloc = e->pos().ry();
    }
}

void CinImageView::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        //cout<<"CINIMAGE: n left mouse release"<<endl;
        lastXloc = e->pos().rx();
        lastYloc = e->pos().ry();
    }
}

void CinImageView::mouseMoveEvent(QMouseEvent *e)
{
    QPoint p = e->pos();

    string path = dbPath.toStdString();
    string imagePath;
    QPixmap image;

    int numSliders = paramSet->getNumParameters();
    QStringList paramNames = paramSet->getParameterNames();

    vector<float> currentParamVals;
    for(int i=0;i<paramNames.size();i++)
    {
        float val;
        paramSet->getValue(paramNames[i],&val);
        currentParamVals.push_back(val);
    }

    //get the current values of phi and theta
    for(int i=0;i<paramNames.size();i++)
    {
        if(paramNames[i]=="phi")
            paramSet->getValue(paramNames[i],&currentPhi);

        if(paramNames[i]=="theta")
            paramSet->getValue(paramNames[i],&currentTheta);
    }

    QSqlQuery qry;
    string queryText;
    float min,max;

    if(fabs(p.rx() - currentXloc) > slidePixel && fabs(p.ry() - currentYloc) < slidePixel) //only phi
    {
        if((p.rx() - currentXloc) > 0 && lastXloc < p.rx()) //slide right
        {
            queryText = "SELECT MIN(phi) FROM " + mTableName.toStdString() + " WHERE phi > :phi";
            qry.prepare(QString::fromStdString(queryText));
            qry.bindValue(":phi", currentPhi);
            qry.exec();
            while (qry.next())
            {
                currentPhi = qry.value(0).toFloat();
            }

            //now get the correct image
            queryText = constructQueryString(paramNames);
            qry.prepare(QString::fromStdString(queryText));
            for(int i=0;i<numSliders;i++)
            {
                if(paramNames[i].toStdString()=="phi")
                    qry.bindValue(":phi", currentPhi);
                else
                {
                    string s;
                    s = ":"+paramNames[i].toStdString();
                    qry.bindValue(QString::fromStdString(s), currentParamVals[i]);
                }
            }
            qry.exec();
            while (qry.next())
            {
                //cout<<qry.value(numSliders).toString().toStdString()<<endl;
                imagePath = path + "/" + qry.value(numSliders).toString().toStdString();
            }

            // Modify the slider value at the end as a result of drag
            paramSet->changeParameter("phi", currentPhi);
        }

        else if((p.rx() - currentXloc) < 0  && lastXloc > p.rx()) //slide left
        {
            //If current phi is equal to minimum phi then set current phi to maximum phi
            paramSet->getMinMax("phi",&min,&max);
            if( (currentPhi-min) < 0.0000001)
                currentPhi=max;
            else
            {
                queryText = "SELECT MAX(phi) FROM " + mTableName.toStdString() + " WHERE phi < :phi";
                qry.prepare(QString::fromStdString(queryText));
                qry.bindValue(":phi", currentPhi);
                qry.exec();
                while (qry.next())
                {
                    currentPhi = qry.value(0).toFloat();
                }
            }

            //now get the correct image
            queryText = constructQueryString(paramNames);
            qry.prepare(QString::fromStdString(queryText));
            for(int i=0;i<numSliders;i++)
            {
                if(paramNames[i].toStdString()=="phi")
                    qry.bindValue(":phi", currentPhi);
                else
                {
                    string s;
                    s = ":"+paramNames[i].toStdString();
                    qry.bindValue(QString::fromStdString(s), currentParamVals[i]);
                }
            }
            qry.exec();
            while (qry.next())
            {
                cout<<qry.value(numSliders).toString().toStdString()<<endl;
                imagePath = path + "/" + qry.value(numSliders).toString().toStdString();
            }

            //Modify the slider value at the end as a result of drag
            paramSet->changeParameter("phi", currentPhi);
        }
    }
    else if(fabs(p.ry() - currentYloc) > slidePixel && fabs(p.rx() - currentXloc) < slidePixel) //only theta
    {
        if((p.ry() - currentYloc) < 0 && lastYloc > p.ry()) //slide up
        {
            queryText = "SELECT MIN(theta) FROM " + mTableName.toStdString() + " WHERE theta > :theta";
            qry.prepare(QString::fromStdString(queryText));
            qry.bindValue(":theta", currentTheta);
            qry.exec();
            while (qry.next())
            {
                currentTheta = qry.value(0).toFloat();
            }

            //now get the correct image
            queryText = constructQueryString(paramNames);
            qry.prepare(QString::fromStdString(queryText));
            for(int i=0;i<numSliders;i++)
            {
                if(paramNames[i].toStdString()=="theta")
                    qry.bindValue(":theta", currentTheta);
                else
                {
                    string s;
                    s = ":"+paramNames[i].toStdString();
                    qry.bindValue(QString::fromStdString(s), currentParamVals[i]);
                }
            }
            qry.exec();
            while (qry.next())
            {
                //cout<<qry.value(numSliders).toString().toStdString()<<endl;
                imagePath = path + "/" + qry.value(numSliders).toString().toStdString();
            }

            // Modify the slider value at the end as a result of drag
            paramSet->changeParameter("theta", currentTheta);
        }

        else if((p.ry() - currentYloc) > 0 && lastYloc < p.ry()) //slide down
        {
            //If current theta is equal to minimum theta then set current theta to maximum theta
            paramSet->getMinMax("theta",&min,&max);
            if( (currentTheta-min) < 0.0000001)
                currentTheta=max;
            else
            {
                queryText = "SELECT MAX(theta) FROM " + mTableName.toStdString() + " WHERE theta < :theta";
                qry.prepare(QString::fromStdString(queryText));
                qry.bindValue(":theta", currentTheta);
                qry.exec();

                while (qry.next())
                {
                    currentTheta = qry.value(0).toFloat();
                }
            }

            //now get the correct image
            queryText = constructQueryString(paramNames);
            qry.prepare(QString::fromStdString(queryText));
            for(int i=0;i<numSliders;i++)
            {
                if(paramNames[i].toStdString()=="theta")
                    qry.bindValue(":theta", currentTheta);
                else
                {
                    string s;
                    s = ":"+paramNames[i].toStdString();
                    qry.bindValue(QString::fromStdString(s), currentParamVals[i]);
                }
            }
            qry.exec();
            while (qry.next())
            {
                imagePath = path + "/" + qry.value(numSliders).toString().toStdString();
            }

            // Modify the slider value at the end as a result of drag
            paramSet->changeParameter("theta", currentTheta);
        }
    }
    else if(fabs(p.rx() - currentXloc) > slidePixel && fabs(p.ry() - currentYloc) > slidePixel) //both theta and phi
    {
        cout<<"both"<<endl;
    }

    //Load the image finally
    if(!loadImage(QString::fromStdString(imagePath),&image))
    {
        cout<<"image loading failed over mouse move!!"<<endl;
    }
    else
    {
        sceneObj->addPixmap(image);
    }

    lastXloc = p.rx();
    lastYloc = p.ry();
}

