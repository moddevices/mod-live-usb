// SPDX-FileCopyrightText: 2021-2022 Filipe Coelho <falktx@falktx.com>
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "KioskWindow.hpp"

#include "KioskTabs.hpp"
#include "KioskSettingsPopup.hpp"
#include "PeakMeterThread.hpp"

#include "../widgets/digitalpeakmeter.hpp"

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpSocket>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabBar>

KioskWindow::KioskWindow(const bool desktop)
  : QMainWindow(nullptr, (desktop ? Qt::Window|Qt::WindowStaysOnBottomHint|Qt::FramelessWindowHint : Qt::Window)),
    settingsPopup(new KioskSettingsPopup()),
    tabWidget(new KioskTabs(this)),
    clockFont(font()),
    clockRect(),
    clockTimer(-1),
    gainSlider(new QSlider(Qt::Horizontal, this)),
    peakMeterIn(new DigitalPeakMeter(this)),
    peakMeterOut(new DigitalPeakMeter(this)),
    settingsButton(new QPushButton(this)),
    powerButton(new QPushButton(this)),
    program(findStartScript()),
    audioContainerProcess(),
    peakMeterThread(new PeakMeterThread(this, peakMeterIn, peakMeterOut))
{
    setCentralWidget(tabWidget);
    setWindowTitle("MOD Live USB");

    audioContainerProcess.setProcessChannelMode(QProcess::ForwardedChannels);

    const int height = tabWidget->tabBar()->height();

    clockFont.setFamily("Monospace");
    clockFont.setPixelSize(20);

    gainSlider->setFixedSize(height*4, height);
    gainSlider->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    gainSlider->setMinimum(-30);
    gainSlider->setMaximum(30);
    gainSlider->setTickPosition(QSlider::TicksBothSides);

    peakMeterIn->setChannelCount(2);
    peakMeterIn->setMeterColor(DigitalPeakMeter::COLOR_BLUE);
    peakMeterIn->setMeterLinesEnabled(false);
    peakMeterIn->setMeterOrientation(DigitalPeakMeter::HORIZONTAL);
    peakMeterIn->setMeterStyle(DigitalPeakMeter::STYLE_RNCBC);
    peakMeterIn->setSmoothMultiplier(0);
    peakMeterIn->setFixedSize(150, height);

    peakMeterOut->setChannelCount(2);
    peakMeterOut->setMeterColor(DigitalPeakMeter::COLOR_GREEN);
    peakMeterOut->setMeterLinesEnabled(false);
    peakMeterOut->setMeterOrientation(DigitalPeakMeter::HORIZONTAL);
    peakMeterOut->setMeterStyle(DigitalPeakMeter::STYLE_OPENAV);
    peakMeterOut->setSmoothMultiplier(0);
    peakMeterOut->setFixedSize(150, height);

    settingsButton->setFixedSize(height, height);
    settingsButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    settingsButton->setIcon(QIcon::fromTheme("configure"));

    powerButton->setFixedSize(height, height);
    powerButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    powerButton->setIcon(QIcon::fromTheme("system-shutdown"));

    repositionTabBarWidgets();

    clockTimer = startTimer(1000);

    connect(powerButton, SIGNAL(clicked()), this, SLOT(openPower()));
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(openSettings()));
    connect(gainSlider, SIGNAL(valueChanged(int)), this, SLOT(setGain(int)));

    if (program.isEmpty())
        settingsButton->hide();
    else
        peakMeterThread->init();

    if (getenv("TESTING") != nullptr)
        tryConnectingToWebServer();
}

KioskWindow::~KioskWindow()
{
    peakMeterThread->requestInterruption();
    stopAudioContainer();
    peakMeterThread->stop();
    // delete children?
}

void KioskWindow::stopAudioContainer()
{
    if (getenv("USING_SYSTEMD") != nullptr)
    {
        QProcess sctl;
        sctl.setProcessChannelMode(QProcess::ForwardedChannels);
        sctl.start("systemctl", {"stop", "mod-live-audio"}, QIODevice::ReadWrite);
        sctl.waitForFinished();
    }

    if (getenv("USING_SYSTEMD") != nullptr || getuid() == 0)
    {
        QProcess mctl;
        mctl.setProcessChannelMode(QProcess::ForwardedChannels);
        mctl.start("machinectl", {"stop", "mod-live-usb"}, QIODevice::ReadWrite);
        mctl.waitForFinished();
    }

    if (audioContainerProcess.state() != QProcess::NotRunning)
    {
        audioContainerProcess.terminate();
        audioContainerProcess.waitForFinished();
    }
}

void KioskWindow::openPower()
{
    if (QMessageBox::question(this,
                              "Power Off",
                              "Power off the system now?",
                              QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No)) == QMessageBox::Yes)
    {
        system("poweroff");
    }
}

void KioskWindow::openSettings(const bool cancellable)
{
    if (program.isEmpty())
        return;

    settingsPopup->setCancellable(cancellable);

    if (settingsPopup->exec() == 0 && cancellable)
        return;

    QString device;
    unsigned rate;
    unsigned bufsize;

    if (! settingsPopup->getSelected(device, rate, bufsize))
    {
        if (cancellable)
            return;

        return openSettings(cancellable);
    }
    printf("Selected %s %u %u\n", device.toUtf8().constData(), rate, bufsize);

    stopAudioContainer();

    const QStringList arguments = { device, QString::number(rate), QString::number(bufsize) };

    audioContainerProcess.start(program, arguments, QIODevice::ReadWrite | QIODevice::Unbuffered);

    if (getenv("USING_SYSTEMD") != nullptr)
        audioContainerProcess.waitForFinished();

    QTimer::singleShot(0, this, SLOT(tryConnectingToWebServer()));
}

void KioskWindow::tryConnectingToWebServer()
{
    QTcpSocket socket;
    socket.connectToHost("localhost", 8888);

    if (! socket.waitForConnected(500))
    {
        QTimer::singleShot(500, this, SLOT(tryConnectingToWebServer()));
        return;
    }

    tabWidget->reloadPage();
}

void KioskWindow::setGain(const int gain)
{
    peakMeterThread->send(sys_serial_event_type_pedalboard_gain, gain);
}

void KioskWindow::keyPressEvent(QKeyEvent* const event)
{
    QMainWindow::keyPressEvent(event);

    const Qt::KeyboardModifiers modifiers = event->modifiers();

    if ((modifiers & Qt::Modifier::ALT) == 0x0)
        return;
    if ((modifiers & Qt::Modifier::CTRL) == 0x0)
        return;

    if (event->key() == Qt::Key::Key_R)
        tabWidget->reloadPage();
}

void KioskWindow::paintEvent(QPaintEvent* const event)
{
    QMainWindow::paintEvent(event);

    if (clockRect.x() == 0)
        return;

    QPainter painter(this);
    painter.setFont(clockFont);
    painter.drawText(clockRect, QTime::currentTime().toString("hh:mm:ss"));
}

void KioskWindow::closeEvent(QCloseEvent* const event)
{
    tabWidget->closeForeignWidgets();
    QMainWindow::closeEvent(event);
}

void KioskWindow::resizeEvent(QResizeEvent* const event)
{
    QMainWindow::resizeEvent(event);
    repositionTabBarWidgets();
}

void KioskWindow::timerEvent(QTimerEvent* const event)
{
    QMainWindow::timerEvent(event);

    if (event->timerId() != clockTimer)
        return;
    if (clockRect.x() == 0)
        return;

    update(clockRect);
}

void KioskWindow::repositionTabBarWidgets()
{
    const int width = this->width();
    const int height = tabWidget->tabBar()->height();

    // clock at center
    QFontMetrics clockmetrics(clockFont);
    const int clockwidth = clockmetrics.horizontalAdvance("00:00:00");
    const int clockheight = clockmetrics.height();
    clockRect = QRect(width/2 - clockwidth/2, height/2 - clockheight/2, clockwidth, clockheight);

    // going from the corner..
    const int padding = 4;
    int x = width;

    x -= powerButton->width() + padding;
    powerButton->move(x, 0);

    x -= settingsButton->width() + padding;
    settingsButton->move(x, 0);

    x -= peakMeterOut->width() + padding;
    peakMeterOut->move(x, 0);

    x -= peakMeterIn->width() + padding;
    peakMeterIn->move(x, 0);

    x -= gainSlider->width() + padding;
    gainSlider->move(x, 0);
}
