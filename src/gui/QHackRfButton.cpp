
#include <QHackRfButton.hpp>
#include <iostream>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>

QHackRfButton::QHackRfButton (QWidget* parent) : QStatusButton (parent), gain (0)
{
    vboxLayout = new QVBoxLayout (this);
    setLayout (vboxLayout);

    // Set title.
    auto* title = new QLabel ("TX Start/Stop");
    title->setAlignment (Qt::AlignCenter | Qt::AlignBottom);
    title->setFont (QFont ("Ubuntu Mono", 18, QFont::Bold));
    vboxLayout->addWidget (title);

    // Init subtext.
    subtext = new QLabel ("");
    vboxLayout->addWidget (subtext);
}

QHackRfButton::~QHackRfButton ()
{
    delete subtext;
    delete vboxLayout;
}

uint8_t QHackRfButton::getDbGain () const
{
    return gain;
}

void QHackRfButton::setDbGain (uint8_t dbGain)
{
    gain = dbGain;
    update ();
}

void QHackRfButton::paintEvent (QPaintEvent* event)
{
    QStatusButton::paintEvent (event);
    QPainter painter (this);

    // Display gain.
    painter.setFont (QFont ("Ubuntu Mono", 12, QFont::Bold));
    painter.drawText (7, 15, QString::fromStdString ("← dB Gain: " + std::to_string (getDbGain ())));

    // Update subtext.
    vboxLayout->takeAt (1);
    vboxLayout->insertWidget (1, subtext);

    if (getStatus () != OK && getStatus () != OFF)
    {
        painter.drawText (event->rect (), Qt::AlignHCenter | Qt::AlignBottom, "(Tap to try again.)");
    }
}

void QHackRfButton::setSubtext (const std::string& text)
{
    delete subtext;
    subtext = new QLabel (QString::fromStdString (text));
    subtext->setAlignment (Qt::AlignCenter);
    subtext->setFont (QFont ("Ubuntu Mono", 14, QFont::Medium));
    subtext->setWordWrap (true);
    update ();
}

void QHackRfButton::setOff (const std::string& text)
{
    setStatus (OFF);
    setSubtext (text);
}

void QHackRfButton::setOk (const std::string& text)
{
    setStatus (OK);
    setSubtext (text);
}

void QHackRfButton::setWarning (const std::string& text)
{
    setStatus (WARNING);
    setSubtext (text);
}

void QHackRfButton::setError (const std::string& text)
{
    setStatus (ERROR);
    setSubtext (text);
}
