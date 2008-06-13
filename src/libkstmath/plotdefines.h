/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2004 by University of British Columbia
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLOTDEFINES_H
#define PLOTDEFINES_H

#define MARKER_LABEL_PRECISION        15

#include "kst_i18n.h"

enum AxisInterpretationType {
  AXIS_INTERP_YEAR,
  AXIS_INTERP_CTIME,
  AXIS_INTERP_JD,
  AXIS_INTERP_MJD,
  AXIS_INTERP_RJD,
  AXIS_INTERP_AIT };

enum AxisDisplayType { 
  AXIS_DISPLAY_YEAR,
  AXIS_DISPLAY_YYMMDDHHMMSS_SS,
  AXIS_DISPLAY_DDMMYYHHMMSS_SS,
  AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS,
  AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS,
  AXIS_DISPLAY_JD,
  AXIS_DISPLAY_MJD,
  AXIS_DISPLAY_RJD };

struct AxisInterpretation {
  const char *label;
  AxisInterpretationType type;
};

struct AxisDisplay {
  const char *label;
  AxisDisplayType type;
};

const AxisInterpretation AxisInterpretations[] = {
  { I18N_NOOP("Julian Year"), AXIS_INTERP_YEAR },
  { I18N_NOOP("Standard C time"), AXIS_INTERP_CTIME },
  { I18N_NOOP2("Julian Date", "JD"), AXIS_INTERP_JD },
  { I18N_NOOP2("Modified Julian Date", "MJD"), AXIS_INTERP_MJD },
  { I18N_NOOP2("Reduced Julian Date", "RJD"), AXIS_INTERP_RJD },
  { I18N_NOOP2("Temps Atomique International", "TAI"), AXIS_INTERP_AIT }
};

const AxisDisplay AxisDisplays[] = {
  { I18N_NOOP("Julian Year"), AXIS_DISPLAY_YEAR },
  { I18N_NOOP("YYYY/MM/DD HH:MM:SS.SS"), AXIS_DISPLAY_YYMMDDHHMMSS_SS },
  { I18N_NOOP("DD/MM/YYYY HH:MM:SS.SS"), AXIS_DISPLAY_DDMMYYHHMMSS_SS },
  { I18N_NOOP("<Qt Text Date> HH:MM:SS.SS"), AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS },
  { I18N_NOOP("<Qt Local Date> HH:MM:SS.SS"), AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS },
  { I18N_NOOP2("Julian Date", "JD"), AXIS_DISPLAY_JD },
  { I18N_NOOP2("Modified Julian Date", "MJD"), AXIS_DISPLAY_MJD },
  { I18N_NOOP2("Reduced Julian Date", "RJD"), AXIS_DISPLAY_RJD }
};

const unsigned int numAxisInterpretations = sizeof( AxisInterpretations ) / sizeof( AxisInterpretation );
const unsigned int numAxisDisplays = sizeof( AxisDisplays ) / sizeof( AxisDisplay );

#endif
// vim: ts=2 sw=2 et
