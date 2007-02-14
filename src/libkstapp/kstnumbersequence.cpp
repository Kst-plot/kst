/***************************************************************************
                      kstnumbersequence.cpp  -  Part of KST
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

// application specific includes
#include "kstnumbersequence.h"

KstNumberSequence::KstNumberSequence() {
  setRange(0, 1);
  _curr = 0;
  _nextSequence = 0L;
}


KstNumberSequence::KstNumberSequence(int min, int max) {
  setRange(min, max);
  _curr = min;
  _nextSequence = 0L;
}


KstNumberSequence::~KstNumberSequence() {
  
}

int KstNumberSequence::next() {
  _curr++;
  if (_curr > _max) {
    _curr = _min;
    if (_nextSequence) {
      _nextSequence->next();
    }
  }
  
  return _curr;
}

void KstNumberSequence::reset() {
  _curr = _min;
  if (_nextSequence) {
    _nextSequence->reset();
  }
}

int KstNumberSequence::current() {
  return _curr;
}

void KstNumberSequence::setRange(int min, int max) {
  _min = min;
  _max = max;
  if (_min > _max) {
    _min = _max;
  }
}

void KstNumberSequence::hookToNextSequence(KstNumberSequence* numberSequence) {
  _nextSequence = numberSequence;
}


// vim: ts=2 sw=2 et
