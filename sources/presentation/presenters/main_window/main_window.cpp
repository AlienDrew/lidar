#include "main_window.h"
#include "ui_mainwindow.h"

#include "settings_provider.h"

#include "service_registry.h"
#include "channel.h"
#include "da_converter_service.h"
#include "freq_generator_service.h"
#include "transfer_service.h"
#include "unit_conversion.h"
#include "presentation_manager.h"
#include "switch.h"

#include <QDebug>
#include <QMessageBox>

using namespace presentation;

class MainWindow::Impl
{
public:
    domain::DAConverterService* daConverterService;
    domain::FreqGeneratorService* freqGenService;
    domain::TransferService* transferService;
    dto::ChannelPtr channel;

    QMap< QString, QAction* > actionMap;
    QList<QComboBox*> chUnits;
    QList<QPushButton*> chButtons;
    QList<QDoubleSpinBox*> chBoxes;
    QLabel* statusIcon;
    int chUnitCurrentIndex = 0;
};

namespace
{
    QList<dto::Channel::FrequencyUnits> availableUnits =
    {
        dto::Channel::Hz, dto::Channel::KHz, dto::Channel::MHz
    };
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), d(new Impl)
{
    ui->setupUi(this);
    initToolBar();
    ui->menuFile->setEnabled(false);
    d->actionMap["settings"] = ui->actionSettings;
    d->actionMap["about"] = ui->actionAbout;

    d->chUnits = {ui->ch1Units, ui->ch2Units};
    d->chBoxes = {ui->ch1Box, ui->ch2Box};
    d->chButtons = {ui->setCh1Button, ui->setCh2Button};

    int genChannelAmount = settingsProvider->value(settings::freq_generator::channelCount).toInt();
    if ((d->chUnits.size() != d->chBoxes.size() || d->chBoxes.size() != d->chButtons.size())
            || d->chBoxes.size() != genChannelAmount)
        qFatal("amount of generator components must agree");

    for(QComboBox* units : d->chUnits)
    {
        units->addItem("Hz");
        units->addItem("KHz");
        units->addItem("MHz");
    }

    d->daConverterService = serviceRegistry->daConverterService();
    d->freqGenService = serviceRegistry->freqGeneratorService();
    d->transferService = serviceRegistry->transferService();

    d->statusIcon = new QLabel(this);
    d->statusIcon->setPixmap(QPixmap(":/icons/usb_disconnected_status.png").scaledToHeight(statusBar()->height()/2));
    d->statusIcon->setToolTip("usb status");
    statusBar()->insertPermanentWidget(0,d->statusIcon, 0);
    statusBar()->showMessage("usb device disconnected");

    for (QPushButton* chButton : d->chButtons)
    {
        connect(chButton, &QPushButton::clicked, this, [chButton, this] ()
        {
            updateGenerator(d->chButtons.indexOf(chButton), d->chBoxes.at( d->chButtons.indexOf(chButton) )->value());
        });
    }

    for(QComboBox* chUnit : d->chUnits)
    {
        connect(chUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [chUnit, this](int index)
        {
            double val = d->chBoxes.at(d->chUnits.indexOf(chUnit))->value();
            double maxFreq = settingsProvider->value(settings::freq_generator::maxFrequency).toDouble();

            val = utils::UnitConversion::frequencyConvert(val,::availableUnits.at(d->chUnitCurrentIndex), ::availableUnits.at(index));
            maxFreq = utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(index));
            int tempCurr = d->chUnitCurrentIndex;
            d->chUnitCurrentIndex = index; //change it here for proper generatorUpdate

            if (index<tempCurr)
            {
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
            }

            d->chBoxes.at(d->chUnits.indexOf(chUnit))->setValue(val);

            if (index>tempCurr)
            {
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
            }
        });
    }


//    connect(ui->biasDial, &QDial::valueChanged, this, [this](int value)
//    {
//        ui->biasBox->blockSignals(true);
//        ui->biasBox->setValue(value);
//        ui->biasBox->blockSignals(false);
//    });
//    connect(ui->biasBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](int value)
//    {
//        ui->biasDial->blockSignals(true);
//        ui->biasDial->setValue(value);
//        ui->biasDial->blockSignals(false);
//    });
    connect(ui->setBiasButton, &QPushButton::clicked, this, [this]()
    {
        updateDAC(0, utils::UnitConversion::voltsToDAC(ui->biasBox->value()));
    });

    connect(ui->setLaserPowButton, &QPushButton::clicked, this, [this]()
    {
        updateDAC(1, utils::UnitConversion::voltsToDAC(ui->laserPwrBox->value()));
    });

    Switch *pulseBoostSwitch = new Switch(this);
    ui->laserGroup->layout()->addWidget(pulseBoostSwitch);
    pulseBoostSwitch->setMaximumWidth(40);
    connect(pulseBoostSwitch, &Switch::toggled, this, [=](bool isOn)
    {
        if (!d->transferService->transferCommand(domain::TransferService::hv_ctrl, (uint8_t)isOn))
            pulseBoostSwitch->setToggle(!isOn);
    });

    connect(d->daConverterService, &domain::DAConverterService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        d->transferService->transferChannel(domain::TransferService::dac, channel);
    });
    connect(d->freqGenService, &domain::FreqGeneratorService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        d->transferService->transferChannel(domain::TransferService::gen, channel);
    });

    connect(ui->measureButton, &QPushButton::clicked, this, &MainWindow::onMeasureClick);
}

MainWindow::~MainWindow()
{
    d->transferService->closeDevice();
    delete ui;
    qDebug()<<"main window destroyed";
}

const QMap<QString, QAction*>& MainWindow::actionMap() const
{
    return d->actionMap;
}

void MainWindow::usbToggle()
{
    if (d->transferService->deviceOpened())
    {
        d->transferService->closeDevice();
        ui->mainToolBar->actions().at(0)->setIcon(QIcon(":/icons/usb_disconnected.png"));
    }
    else
    {
        if (d->transferService->openDevice())
            ui->mainToolBar->actions().at(0)->setIcon(QIcon(":/icons/usb_connected.png"));
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    if (d->transferService->deviceOpened())
    {
        d->statusIcon->setPixmap(QPixmap(":/icons/usb_connected_status.png").scaledToHeight(statusBar()->height()/1.4));
        statusBar()->showMessage("usb device connected");
    }
    else
    {
        d->statusIcon->setPixmap(QPixmap(":/icons/usb_disconnected_status.png").scaledToHeight(statusBar()->height()/1.4));
        statusBar()->showMessage("usb device disconnected");
    }
}

void MainWindow::updateDAC(int chId, int value)
{
    qDebug()<<value;
    d->daConverterService->updateDAC(chId, value);
}

void MainWindow::updateGenerator(int chId, double value)
{
    //d->freqGenService->update(d->chBoxes.indexOf(chBox), value);
    d->channel = d->freqGenService->load(chId);
    //convert to Hz before set
    quint32 HzValue = utils::UnitConversion::frequencyConvert(value, ::availableUnits.value(d->chUnitCurrentIndex), dto::Channel::Hz);
    qDebug()<<"channel ID:"<<chId+1<<"displayed val: "<<value<<"in Hz: "<<HzValue;
    d->channel->setValue(HzValue);
    d->freqGenService->update(d->channel);
}

void MainWindow::onMeasureClick()
{
    if (!d->transferService->deviceOpened())
    {
        qDebug()<<"Cannot start measure! usb device not opened";
        QMessageBox::critical(0, "Warning", "Cannot start measure! Device not connected!");
        return;
    }

    d->transferService->transferCommand(domain::TransferService::startMeasure);
    presentationManager->chartWindow()->startReading();
    presentationManager->chartWindow()->show();
}

void MainWindow::initToolBar()
{
    ui->mainToolBar->addAction(QIcon(":/icons/usb_disconnected.png"), "usb connection", this, SLOT(usbToggle()));
}
