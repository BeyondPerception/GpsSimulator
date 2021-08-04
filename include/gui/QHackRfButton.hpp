#ifndef HACKRFBUTTON_HPP
#define HACKRFBUTTON_HPP

#include <QVBoxLayout>
#include <QLabel>
#include "QStatusButton.hpp"

class QHackRfButton : public QStatusButton
{
Q_OBJECT

public:
    explicit QHackRfButton (QWidget* parent = nullptr);

    ~QHackRfButton () override;

    uint8_t getDbGain () const;

    void setOff (const std::string& text);

    void setOk (const std::string& text);

    void setWarning (const std::string& text);

    void setError (const std::string& text);

    void setSubtext (const std::string& text);

public slots:

    void setDbGain (uint8_t dbGain);

protected:
    void paintEvent (QPaintEvent* event) override;

private:
    QVBoxLayout* vboxLayout;

    uint8_t gain;

    QLabel* subtext;
};

#endif //HACKRFBUTTON_HPP
