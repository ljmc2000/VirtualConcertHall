#include "userview.h"
#include "ui_userview.h"

#include <QtConcurrent/QtConcurrent>

UserView::UserView(InstrumentView *v, QString username, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::UserView)
{
    ui->setupUi(this);
    this->instrumentView=v;
    v->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    ui->instrumentView->addWidget(v);

    connect(ui->volumeSlider, &QSlider::valueChanged,
            [=](){this->volume=ui->volumeSlider->value()/127.0f;});

    ui->usernameLabel->setText(username);
    ui->pingLabel->setText("?ms");
}

UserView::~UserView()
{
    delete ui;
}

void UserView::playNote(quint8 midiMessage)
{
    instrumentView->playNote(midiMessage);
}

void UserView::setLatency(qint16 latency)
{
    ui->pingLabel->setText(QString::number(latency)+" ms");
}

void UserView::setUsername(QString username)
{
    ui->usernameLabel->setText(username);
}

InstrumentType UserView::getInstrumentType()
{
    return instrumentView->type;
}
