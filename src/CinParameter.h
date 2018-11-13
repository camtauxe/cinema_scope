#ifndef CINPARAMETER_H
#define CINPARAMETER_H

#include <QString>

//! A class that manages a parameter 
/*!
 *  A set of parameters defines the input values for a Cinema Database
 *  This class manages communication about these parameters as they
 *  change in a Qt application.
*/
class CinParameter
{
    public:
        enum Type{UNDEFINED=0, STRING, FLOAT, INT, NUMTYPES};
        static const char *TypeNames[NUMTYPES];
        static const char *GetNameForType(CinParameter::Type type);

        CinParameter(const QString &name, CinParameter::Type type, float min, float max, float cur);

        const QString &getName() const {return mName;}
        CinParameter::Type getType() const {return mType;}
        float getMin() const {return mMin;}
        float getMax() const {return mMax;}
        float getValue() const {return mValue;}

        void  setName(const QString &name)     {mName  = name;}
        void  setType(CinParameter::Type type) {mType  = type;}
        void  setMin(float min)                {mMin   = min;}
        void  setMax(float max)                {mMax   = max;}
        void  setValue(float value)            {mValue = value;}

    private:
        QString mName;
        CinParameter::Type mType=CinParameter::UNDEFINED;
        float   mMin;
        float   mMax;
        float   mValue;
};

#endif // CINPARAMETER_H
