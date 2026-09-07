#ifndef TEXTINSTRUMENT_H
#define TEXTINSTRUMENT_H

#include "abstractselection.h"

class TextInstrument : public AbstractSelection
{
    Q_OBJECT
public:
    explicit TextInstrument(QObject *parent = nullptr);
};

#endif // TEXTINSTRUMENT_H
