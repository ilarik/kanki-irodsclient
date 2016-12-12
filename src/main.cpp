/**
 * @file main.cpp
 * @brief Kanki iRODS client main() function
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsMainWindow header
#include "rodsmainwindow.h"

int main(int argc, char *argv[])
{
    // initialize Qt application
    QApplication a(argc, argv);

#ifdef HIGH_DPI
    // enable retina-type screen High DPI support
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    // create an iRODS Grid Browser window
    RodsMainWindow w;

    w.show();
    w.doRodsConnect();

    // enter Qt main loop
    return a.exec();
}
