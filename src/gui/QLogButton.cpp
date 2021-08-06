#include <QPainter>
#include <QPalette>
#include <unistd.h
#include <fcntl.h>

#include "QLogButton.hpp"

QLogButton::QLogButton (QWidget* parent) : QTextEdit (parent)
{
    // Capture stdout and stderr and redirect to log.
    this->setReadOnly (true);
    int outFds[2];
    pipe (outFds);
    dup2 (outFds[1], 1);
    fcntl (outFds[0], F_SETFL, O_NONBLOCK);
    outFd = outFds[0];

    int errFds[2];
    pipe (errFds);
    dup2 (errFds[1], 2);
    fcntl (errFds[0], F_SETFL, O_NONBLOCK);
    errFd = errFds[0];

    outNotifier = new QSocketNotifier (outFds[0], QSocketNotifier::Read, this);
    errNotifier = new QSocketNotifier (errFds[0], QSocketNotifier::Read, this);
    connect (outNotifier, &QSocketNotifier::activated, this, &QLogButton::outRead);
    connect (errNotifier, &QSocketNotifier::activated, this, &QLogButton::errRead);

    // Setup colors.
    QPalette pal = palette ();
    pal.setColor (QPalette::Base, QColor (33, 33, 41));
    pal.setColor (QPalette::Text, Qt::white);
    setPalette (pal);
    setFont (QFont ("Ubuntu Mono", 12));
}

QLogButton::~QLogButton ()
{
    delete outNotifier;
    delete errNotifier;
}

void QLogButton::append (const QString& text, bool is_error)
{
    if (is_error)
    {
        // A nicer red than Qt::red.
        setTextColor (QColor (247, 79, 79));
        QTextEdit::append (text);
    } else
    {
        setTextColor (Qt::white);
        QTextEdit::append (text);
    }
}

void QLogButton::mouseReleaseEvent (QMouseEvent* e)
{
    QTextEdit::mouseReleaseEvent (e);

    emit released ();
}

void QLogButton::outRead ()
{
    char buf[1024];
    ssize_t bytesRead;
    while ((bytesRead = read (outFd, buf, 1024)) > 0)
    {
        append (QString::fromStdString (std::string (buf, bytesRead)), false);
    }
}

void QLogButton::errRead ()
{
    char buf[1024];
    ssize_t bytesRead;
    while ((bytesRead = read (errFd, buf, 1024)) > 0)
    {
        append (QString::fromStdString (std::string (buf, bytesRead)), true);
    }
}
