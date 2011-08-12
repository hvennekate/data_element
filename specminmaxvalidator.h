#ifndef SPECMINMAXVALIDATOR_H
#define SPECMINMAXVALIDATOR_H

#include <QDoubleValidator>
#include <QString>

class specMinMaxValidator : public QDoubleValidator
{
Q_OBJECT
public:
    specMinMaxValidator(QObject *parent);

    ~specMinMaxValidator();

public slots :
	void setMax(const QString&) ;
	void setMin(const QString&) ;
};

#endif
