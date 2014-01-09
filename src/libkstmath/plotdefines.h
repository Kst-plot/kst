/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2004 by University of British Columbia
 *                   dscott@phas.ubc.ca                                    *
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


enum AxisInterpretationType {
  AXIS_INTERP_YEAR,
  AXIS_INTERP_CTIME,
  AXIS_INTERP_JD,
  AXIS_INTERP_MJD,
  AXIS_INTERP_RJD,
  AXIS_INTERP_AIT,
  AXIS_INTERP_EXCEL};

enum AxisDisplayType { 
  AXIS_DISPLAY_YEAR,
  AXIS_DISPLAY_YYMMDDHHMMSS_SS,
  AXIS_DISPLAY_DDMMYYHHMMSS_SS,
  AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS,
  AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS,
  AXIS_DISPLAY_JD,
  AXIS_DISPLAY_MJD,
  AXIS_DISPLAY_RJD,
  AXIS_DISPLAY_QTDATETIME_FORMAT};

struct AxisInterpretation {
  const char *label;
  AxisInterpretationType type;
};

struct AxisDisplay {
  const char *label;
  AxisDisplayType type;
};

const AxisInterpretation AxisInterpretations[] = {
  { QT_TR_NOOP("Julian Year"), AXIS_INTERP_YEAR },
  { QT_TR_NOOP("Standard C time"), AXIS_INTERP_CTIME },
  { QT_TRANSLATE_NOOP("Julian Date", "JD"), AXIS_INTERP_JD },
  { QT_TRANSLATE_NOOP("Modified Julian Date", "MJD"), AXIS_INTERP_MJD },
  { QT_TRANSLATE_NOOP("Reduced Julian Date", "RJD"), AXIS_INTERP_RJD },
  { QT_TRANSLATE_NOOP("Temps Atomique International", "TAI"), AXIS_INTERP_AIT },
  { QT_TR_NOOP("Excel time (Windows)"), AXIS_INTERP_EXCEL }
};

const AxisDisplay AxisDisplays[] = {
  { QT_TR_NOOP("Julian Year"), AXIS_DISPLAY_YEAR },
  { QT_TR_NOOP("YYYY/MM/DD HH:MM:SS.SS"), AXIS_DISPLAY_YYMMDDHHMMSS_SS },
  { QT_TR_NOOP("DD/MM/YYYY HH:MM:SS.SS"), AXIS_DISPLAY_DDMMYYHHMMSS_SS },
  { QT_TR_NOOP("<Qt Text Date> HH:MM:SS.SS"), AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS },
  { QT_TR_NOOP("<Qt Local Date> HH:MM:SS.SS"), AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS },
  { QT_TRANSLATE_NOOP("Julian Date", "JD"), AXIS_DISPLAY_JD },
  { QT_TRANSLATE_NOOP("Modified Julian Date", "MJD"), AXIS_DISPLAY_MJD },
  { QT_TRANSLATE_NOOP("Reduced Julian Date", "RJD"), AXIS_DISPLAY_RJD },
  { QT_TR_NOOP("Formatted String"), AXIS_DISPLAY_QTDATETIME_FORMAT }
};

const unsigned int numAxisInterpretations = sizeof( AxisInterpretations ) / sizeof( AxisInterpretation );
const unsigned int numAxisDisplays = sizeof( AxisDisplays ) / sizeof( AxisDisplay );

#endif
// vim: ts=2 sw=2 et
