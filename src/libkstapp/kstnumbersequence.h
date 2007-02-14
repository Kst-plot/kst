/***************************************************************************
                      kstnumbersequence.h  -  Part of KST
                             -------------------
    begin                : Mon Jul 07 2003
    copyright            : (C) 2005 The University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTNUMBERSEQUENCE_H
#define KSTNUMBERSEQUENCE_H

#include <kstaticdeleter.h>

#include "kst_export.h"

class KST_EXPORT KstNumberSequence {

  public:
    KstNumberSequence();
    KstNumberSequence(int min, int max);
    ~KstNumberSequence();
    
    int next(); //next number
    void reset();
    
    int current(); // current number - does not advance sequence
    void setRange(int min, int max);

    // numberSequence will be advanced when this sequence rolls over
    // set to NULL to have no next number sequence
    void hookToNextSequence(KstNumberSequence* numberSequence);
    
  private:
    int _curr;    // current number
    int _min;
    int _max;
    KstNumberSequence* _nextSequence;
};

#endif

// vim: ts=2 sw=2 et
