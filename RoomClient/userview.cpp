#include "userview.h"
#include "ui_userview.h"

UserView::UserView(InstrumentView *v, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserView)
{
    ui->setupUi(this);
    this->instrumentView=v;
    v->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    ui->instrumentView->addWidget(v);

    connect(ui->volumeSlider, &QSlider::valueChanged,
            [=](){this->volume=ui->volumeSlider->value()/100.0f;});

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
