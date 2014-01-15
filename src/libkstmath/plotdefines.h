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

//QT_TRANSLATE_NOOP3 ( context, sourceText, comment )

const AxisInterpretation AxisInterpretations[] = {
  { QT_TRANSLATE_NOOP("AxisTab", "Julian Year"), AXIS_INTERP_YEAR },
  { QT_TRANSLATE_NOOP("AxisTab", "Standard C time"), AXIS_INTERP_CTIME },
  //{ QT_TRANSLATE_NOOP3("AxisTab", "JD", "Julian Date"), AXIS_INTERP_JD },
  //{ QT_TRANSLATE_NOOP3("AxisTab", "MJD", "Modified Julian Date"), AXIS_INTERP_MJD },
  //{ QT_TRANSLATE_NOOP3("AxisTab", "RJD", "Reduced Julian Date"), AXIS_INTERP_RJD },
  //{ QT_TRANSLATE_NOOP3("AxisTab", "TAI", "Temps Atomique International"), AXIS_INTERP_AIT },
  { QT_TRANSLATE_NOOP("AxisTab", "JD"), AXIS_INTERP_JD },
  { QT_TRANSLATE_NOOP("AxisTab", "MJD"), AXIS_INTERP_MJD },
  { QT_TRANSLATE_NOOP("AxisTab", "RJD"), AXIS_INTERP_RJD },
  { QT_TRANSLATE_NOOP("AxisTab", "TAI"), AXIS_INTERP_AIT },
  { QT_TRANSLATE_NOOP("AxisTab", "Excel time (Windows)"), AXIS_INTERP_EXCEL }
};

const AxisDisplay AxisDisplays[] = {
  { QT_TRANSLATE_NOOP("AxisTab", "Julian Year"), AXIS_DISPLAY_YEAR },
  { QT_TRANSLATE_NOOP("AxisTab", "YYYY/MM/DD HH:MM:SS.SS"), AXIS_DISPLAY_YYMMDDHHMMSS_SS },
  { QT_TRANSLATE_NOOP("AxisTab", "DD/MM/YYYY HH:MM:SS.SS"), AXIS_DISPLAY_DDMMYYHHMMSS_SS },
  { QT_TRANSLATE_NOOP("AxisTab", "<Qt Text Date> HH:MM:SS.SS"), AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS },
  { QT_TRANSLATE_NOOP("AxisTab", "<Qt Local Date> HH:MM:SS.SS"), AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS },
  //{ QT_TRANSLATE_NOOP3("AxisTab", "JD", "Julian Date"), AXIS_DISPLAY_JD },
  //{ QT_TRANSLATE_NOOP3("AxisTab", "MJD", "Modified Julian Date"), AXIS_DISPLAY_MJD },
  //{ QT_TRANSLATE_NOOP3("AxisTab", "RJD", "Reduced Julian Date"), AXIS_DISPLAY_RJD },
  { QT_TRANSLATE_NOOP("AxisTab", "JD"), AXIS_DISPLAY_JD },
  { QT_TRANSLATE_NOOP("AxisTab", "MJD"), AXIS_DISPLAY_MJD },
  { QT_TRANSLATE_NOOP("AxisTab", "RJD"), AXIS_DISPLAY_RJD },
  { QT_TRANSLATE_NOOP("AxisTab", "Formatted String"), AXIS_DISPLAY_QTDATETIME_FORMAT }
};

const unsigned int numAxisInterpretations = sizeof( AxisInterpretations ) / sizeof( AxisInterpretation );
const unsigned int numAxisDisplays = sizeof( AxisDisplays ) / sizeof( AxisDisplay );

#endif
// vim: ts=2 sw=2 et
