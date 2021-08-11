#ifndef QLOGBUTTON_HPP
#define QLOGBUTTON_HPP

#include <QPushButton>
#include <QTextEdit>
#include <QSocketNotifier>

class QLogButton : public QTextEdit
{
Q_OBJECT
public:
    explicit QLogButton (QWidget* parent = nullptr);

    ~QLogButton () override;

    void append (const QString& text, bool is_error);

    void scrollToEnd ();

protected:
    void mouseReleaseEvent (QMouseEvent* e) override;

signals:

    void released ();

private slots:

    void outRead ();

    void errRead ();

private:
    QSocketNotifier* outNotifier;
    QSocketNotifier* errNotifier;

    int outFd;
    int errFd;

    std::mutex outWriteMutex;
    std::mutex errWriteMutex;
};

#endif //QLOGBUTTON_HPP
