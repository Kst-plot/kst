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

#include <application.h>

int main(int argc, char *argv[]) {
  Kst::Application app(argc, argv);
  if (app.mainWindow()->initFromCommandLine()) {
    return app.exec();
  }
  return 0;
}
