/**
 * @file main.cpp
 * @brief Kanki iRODS client main() function
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
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
    QApplication app(argc, argv);

    // enable retina-type screen High DPI support
#ifdef HIGH_DPI
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    // we force the GUI style
    app.setStyle(QStyleFactory::create("Fusion"));

    // we create a custom color palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(0x53,0x53,0x53));
    darkPalette.setColor(QPalette::WindowText, QColor(0xd0, 0xd0, 0xd0));

    darkPalette.setColor(QPalette::Base, QColor(0x40,0x40,0x40));
    darkPalette.setColor(QPalette::AlternateBase, QColor(0x45,0x45,0x45));

    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);

    darkPalette.setColor(QPalette::Text, QColor(0xe0, 0xe0, 0xe0));
    darkPalette.setColor(QPalette::Button, QColor(0x40,0x40,0x40));
    darkPalette.setColor(QPalette::ButtonText, QColor(0xe0,0xe0,0xe0));
    darkPalette.setColor(QPalette::BrightText, Qt::white);

    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(0x6f, 0x6e, 0x6e));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);

    app.setPalette(darkPalette);
    app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    // instantiate a main window bring it up
    RodsMainWindow window;
    window.show();

    // invoke connect action
    window.doRodsConnect();

    // enter Qt event loop
    return (app.exec());
}
