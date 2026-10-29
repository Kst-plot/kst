#ifndef TESTNEXTCOLOR_H
#define TESTNEXTCOLOR_H

#include "nextcolor.h"

// Datasource construction expects the application-owned color provider to exist.
class TestNextColor : public Kst::NextColor {
public:
  QColor next() override { return Qt::black; }
  QColor current() override { return Qt::black; }
};

#endif