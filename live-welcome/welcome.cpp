// SPDX-FileCopyrightText: 2021-2022 Filipe Coelho <falktx@falktx.com>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include <QtWidgets/QApplication>

#include "src/KioskWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_X11InitThreads);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationDisplayName("MOD Live-USB");
    app.setApplicationName("MOD Live-USB Welcome");

    QIcon::setThemeSearchPaths({"/usr/share/icons"});
    QIcon::setThemeName("breeze-dark");

    app.setWindowIcon(QIcon::fromTheme("settings-configure"));

    QFont font(app.font());
    font.setPointSize(12);
    app.setFont(font);

    QPalette palette;
    palette.setColor(QPalette::Disabled, QPalette::Window, QColor(14, 14, 14));
    palette.setColor(QPalette::Active,   QPalette::Window, QColor(17, 17, 17));
    palette.setColor(QPalette::Inactive, QPalette::Window, QColor(17, 17, 17));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(83, 83, 83));
    palette.setColor(QPalette::Active,   QPalette::WindowText, QColor(240, 240, 240));
    palette.setColor(QPalette::Inactive, QPalette::WindowText, QColor(240, 240, 240));
    palette.setColor(QPalette::Disabled, QPalette::Base, QColor(6, 6, 6));
    palette.setColor(QPalette::Active,   QPalette::Base, QColor(7, 7, 7));
    palette.setColor(QPalette::Inactive, QPalette::Base, QColor(7, 7, 7));
    palette.setColor(QPalette::Disabled, QPalette::AlternateBase, QColor(12, 12, 12));
    palette.setColor(QPalette::Active,   QPalette::AlternateBase, QColor(14, 14, 14));
    palette.setColor(QPalette::Inactive, QPalette::AlternateBase, QColor(14, 14, 14));
    palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, QColor(4, 4, 4));
    palette.setColor(QPalette::Active,   QPalette::ToolTipBase, QColor(4, 4, 4));
    palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, QColor(4, 4, 4));
    palette.setColor(QPalette::Disabled, QPalette::ToolTipText, QColor(230, 230, 230));
    palette.setColor(QPalette::Active,   QPalette::ToolTipText, QColor(230, 230, 230));
    palette.setColor(QPalette::Inactive, QPalette::ToolTipText, QColor(230, 230, 230));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(74, 74, 74));
    palette.setColor(QPalette::Active,   QPalette::Text, QColor(230, 230, 230));
    palette.setColor(QPalette::Inactive, QPalette::Text, QColor(230, 230, 230));
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor(24, 24, 24));
    palette.setColor(QPalette::Active,   QPalette::Button, QColor(28, 28, 28));
    palette.setColor(QPalette::Inactive, QPalette::Button, QColor(28, 28, 28));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(90, 90, 90));
    palette.setColor(QPalette::Active,   QPalette::ButtonText, QColor(240, 240, 240));
    palette.setColor(QPalette::Inactive, QPalette::ButtonText, QColor(240, 240, 240));
    palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Active,   QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Inactive, QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Disabled, QPalette::Light, QColor(191, 191, 191));
    palette.setColor(QPalette::Active,   QPalette::Light, QColor(191, 191, 191));
    palette.setColor(QPalette::Inactive, QPalette::Light, QColor(191, 191, 191));
    palette.setColor(QPalette::Disabled, QPalette::Midlight, QColor(155, 155, 155));
    palette.setColor(QPalette::Active,   QPalette::Midlight, QColor(155, 155, 155));
    palette.setColor(QPalette::Inactive, QPalette::Midlight, QColor(155, 155, 155));
    palette.setColor(QPalette::Disabled, QPalette::Dark, QColor(129, 129, 129));
    palette.setColor(QPalette::Active,   QPalette::Dark, QColor(129, 129, 129));
    palette.setColor(QPalette::Inactive, QPalette::Dark, QColor(129, 129, 129));
    palette.setColor(QPalette::Disabled, QPalette::Mid, QColor(94, 94, 94));
    palette.setColor(QPalette::Active,   QPalette::Mid, QColor(94, 94, 94));
    palette.setColor(QPalette::Inactive, QPalette::Mid, QColor(94, 94, 94));
    palette.setColor(QPalette::Disabled, QPalette::Shadow, QColor(155, 155, 155));
    palette.setColor(QPalette::Active,   QPalette::Shadow, QColor(155, 155, 155));
    palette.setColor(QPalette::Inactive, QPalette::Shadow, QColor(155, 155, 155));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(14, 14, 14));
    palette.setColor(QPalette::Active,   QPalette::Highlight, QColor(60, 60, 60));
    palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(34, 34, 34));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(83, 83, 83));
    palette.setColor(QPalette::Active,   QPalette::HighlightedText, QColor(255, 255, 255));
    palette.setColor(QPalette::Inactive, QPalette::HighlightedText, QColor(240, 240, 240));
    palette.setColor(QPalette::Disabled, QPalette::Link, QColor(34, 34, 74));
    palette.setColor(QPalette::Active,   QPalette::Link, QColor(100, 100, 230));
    palette.setColor(QPalette::Inactive, QPalette::Link, QColor(100, 100, 230));
    palette.setColor(QPalette::Disabled, QPalette::LinkVisited, QColor(74, 34, 74));
    palette.setColor(QPalette::Active,   QPalette::LinkVisited, QColor(230, 100, 230));
    palette.setColor(QPalette::Inactive, QPalette::LinkVisited, QColor(230, 100, 230));
    app.setPalette(palette);

    const bool desktopMode = getenv("USING_SYSTEMD") != nullptr;

    KioskWindow win(desktopMode);

    if (desktopMode)
    {
        win.showFullScreen();
    }
    else
    {
        win.resize(1200, 800);
        win.show();
    }

    if (getenv("TESTING") == nullptr)
        win.openSettings(false);

    return app.exec();
}
