/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H


namespace Kst {

class ApplicationSettings
{
public:
  static ApplicationSettings *self();

  bool useOpenGL() const;
  void setUseOpenGL(bool use);

private:
  ApplicationSettings();
  ~ApplicationSettings();
  static void cleanup();
  bool _useOpenGL;
};

}

#endif

// vim: ts=2 sw=2 et
