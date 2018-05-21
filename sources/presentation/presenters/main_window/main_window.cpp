#include "main_window.h"
#include "ui_mainwindow.h"

#include "settings_provider.h"

#include "service_registry.h"
#include "channel.h"
#include "command.h"
#include "da_converter_service.h"
#include "freq_generator_service.h"
#include "digital_potentiometer_service.h"
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
    domain::DigitalPotentiometerService* digitalPotentiometerService;
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
    d->digitalPotentiometerService = serviceRegistry->digitalPotentiometerSerivce();
    d->transferService = serviceRegistry->transferService();

    //===setting ranges/steps for fields===
    //ui->ch1Box->setMinimum(settingsProvider->value(settings::freq_generator::minFrequency).toInt());
    //ui->ch1Box->setMaximum(settingsProvider->value(settings::freq_generator::maxFrequency).toInt());
    //ui->ch2Box->setMinimum(settingsProvider->value(settings::freq_generator::minFrequency).toInt());
    //ui->ch2Box->setMaximum(settingsProvider->value(settings::freq_generator::maxFrequency).toInt());
    ui->ch1Box->setDecimals(4);
    ui->ch2Box->setDecimals(4);

    qreal biasStep = 0.1;
    ui->biasBox->setMaximum(settingsProvider->value(settings::dac::vRef).toDouble());
    ui->biasBox->setSingleStep(biasStep);
    ui->biasSlider->setMaximum(qRound(settingsProvider->value(settings::dac::vRef).toDouble()/biasStep));
    ui->biasSlider->setValue(ui->biasSlider->maximum());

    qreal gainStep = 1;
    ui->gainBox->setMinimum(settingsProvider->value(settings::digital_potentiometer::minK).toInt());
    ui->gainBox->setMaximum(settingsProvider->value(settings::digital_potentiometer::maxK).toInt());
    ui->gainBox->setSingleStep(gainStep);
    ui->gainSlider->setMinimum(settingsProvider->value(settings::digital_potentiometer::minK).toInt()/gainStep);
    ui->gainSlider->setMaximum(settingsProvider->value(settings::digital_potentiometer::maxK).toInt()/gainStep);

    qreal laserPwrStep = 0.1;
    ui->laserPwrBox->setMaximum(settingsProvider->value(settings::dac::vRef).toDouble());
    ui->laserPwrBox->setSingleStep(laserPwrStep);
    //========================

    //===setting status bar===
    d->statusIcon = new QLabel(this);
    d->statusIcon->setPixmap(QPixmap(":/icons/usb_disconnected_status.png").scaledToHeight(statusBar()->height()/2));
    d->statusIcon->setToolTip("usb status");
    statusBar()->insertPermanentWidget(0,d->statusIcon, 0);
    statusBar()->showMessage("usb device disconnected");
    //========================

    connect(d->transferService, &domain::TransferService::hotPlugDeviceLeft, this, [this]()
    {
        if (!d->transferService->deviceOpened())
        {
            ui->mainToolBar->actions().at(0)->setIcon(QIcon(":/icons/usb_disconnected.png"));
            updateStatusBar();
        }
    });

    for (QPushButton* chButton : d->chButtons)
    {
        connect(chButton, &QPushButton::clicked, this, [chButton, this] ()
        {
            double minFreq = settingsProvider->value(settings::freq_generator::minFrequency).toDouble();
            double maxFreq = settingsProvider->value(settings::freq_generator::maxFrequency).toInt();
            double value = d->chBoxes.at( d->chButtons.indexOf(chButton) )->value();
            if ( value >=
                    utils::UnitConversion::frequencyConvert(minFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnitCurrentIndex))
                 && value <= utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnitCurrentIndex))
                 )
                updateGenerator(d->chButtons.indexOf(chButton), d->chBoxes.at(d->chButtons.indexOf(chButton))->value());
            else
                QMessageBox::warning(0, qApp->applicationName()+tr(" warning"), tr("Frequency out of range!\nMin value: ")+
                                     QString::number(utils::UnitConversion::frequencyConvert(minFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnitCurrentIndex)))+
                                     " "+d->chUnits.at(d->chButtons.indexOf(chButton))->currentText()+tr("\nMax value: ")+
                                     QString::number(utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnitCurrentIndex)))+
                                     " "+d->chUnits.at(d->chButtons.indexOf(chButton))->currentText());
        });
    }

    for(QComboBox* chUnit : d->chUnits)
    {
        connect(chUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [chUnit, this](int index)
        {
            double val = d->chBoxes.at(d->chUnits.indexOf(chUnit))->value();
            double minFreq = settingsProvider->value(settings::freq_generator::minFrequency).toDouble();
            double maxFreq = settingsProvider->value(settings::freq_generator::maxFrequency).toInt();

            val = utils::UnitConversion::frequencyConvert(val,::availableUnits.at(d->chUnitCurrentIndex), ::availableUnits.at(index));
            minFreq = utils::UnitConversion::frequencyConvert(minFreq, dto::Channel::Hz, ::availableUnits.at(index));
            maxFreq = utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(index));
            //int tempCurr = d->chUnitCurrentIndex;
            d->chUnitCurrentIndex = index; //change it here for proper generatorUpdate

            if (settingsProvider->value(settings::freq_generator::autoUnitConversion).toBool())
            {
                if (::availableUnits.at(d->chUnitCurrentIndex) == dto::Channel::Hz)
                    d->chBoxes.at(d->chUnits.indexOf(chUnit))->setDecimals(0);
                else if (::availableUnits.at(d->chUnitCurrentIndex) == dto::Channel::KHz)
                    d->chBoxes.at(d->chUnits.indexOf(chUnit))->setDecimals(3);
                else
                    d->chBoxes.at(d->chUnits.indexOf(chUnit))->setDecimals(4);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMinimum(minFreq);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setValue(val);
            }
        });
    }


    connect(ui->biasSlider, &QSlider::valueChanged, this, [biasStep, this](int value)
    {
        ui->biasBox->blockSignals(true);
        ui->biasBox->setValue(value*biasStep);
        ui->biasBox->blockSignals(false);
    });
    connect(ui->biasBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [biasStep, this](double value)
    {
        ui->biasSlider->blockSignals(true);
        ui->biasSlider->setValue(qRound(value/biasStep));
        ui->biasSlider->blockSignals(false);
    });
    connect(ui->setBiasButton, &QPushButton::clicked, this, [this]()
    {
        d->daConverterService->updateDAC(0, utils::UnitConversion::voltsToDAC(ui->biasBox->value()));
    });

    connect(ui->gainSlider, &QSlider::valueChanged, this, [gainStep, this](int value)
    {
        ui->gainBox->blockSignals(true);
        ui->gainBox->setValue(value*gainStep);
        ui->gainBox->blockSignals(false);
    });
    connect(ui->gainBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value)
    {
        ui->gainSlider->blockSignals(true);
        ui->gainSlider->setValue(value);
        ui->gainSlider->blockSignals(false);
    });
    connect(ui->setGainButton, &QPushButton::clicked, this, [this]()
    {
        d->digitalPotentiometerService->updatePotentiometer(0, utils::UnitConversion::kToDAC(ui->gainBox->value()));
    });

    connect(ui->setLaserPowButton, &QPushButton::clicked, this, [this]()
    {
        d->daConverterService->updateDAC(1, utils::UnitConversion::voltsToDAC(ui->laserPwrBox->value()));
        //d = (256*2,667*Plaz)/3.3
    });

    Switch* pulseBoostSwitch = new Switch(this);
    QGridLayout* photodetectorLayout = (QGridLayout*) ui->photoDetectorGroup->layout();
    photodetectorLayout->addWidget(pulseBoostSwitch, 0, 2);
    pulseBoostSwitch->setMaximumWidth(40);
    connect(pulseBoostSwitch, &Switch::toggled, this, [=](bool isOn)
    {
        dto::Command cmd;
        cmd.setType(dto::Command::hv_ctrl);
        cmd.addArgument(isOn);
        if (!d->transferService->transferCommand(cmd))
            pulseBoostSwitch->setToggle(!isOn);
    });

    connect(d->daConverterService, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        dto::Command cmd;
        cmd.setType(dto::Command::dac);
        cmd.addArgument(QVariant::fromValue(channel.data()));
        d->transferService->transferCommand(cmd);
    });
    connect(d->freqGenService, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        dto::Command cmd;
        cmd.setType(dto::Command::gen);
        cmd.addArgument(QVariant::fromValue(channel.data()));
        d->transferService->transferCommand(cmd);
    });
    connect(d->digitalPotentiometerService, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        dto::Command cmd;
        cmd.setType(dto::Command::digital_pot);
        cmd.addArgument(QVariant::fromValue(channel.data()));
        d->transferService->transferCommand(cmd);
    });

    connect(ui->measureButton, &QPushButton::clicked, this, &MainWindow::onMeasureClick);
}

MainWindow::~MainWindow()
{
    if (d->transferService->deviceOpened())
    {
        d->transferService->closeDevice();
    }
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

void MainWindow::updateGenerator(int chId, double value)
{
    //d->freqGenService->update(d->chBoxes.indexOf(chBox), value);
    d->channel = d->freqGenService->load(chId);
    //convert to Hz before set
    quint32 HzValue = utils::UnitConversion::frequencyConvert(value, ::availableUnits.value(d->chUnitCurrentIndex), dto::Channel::Hz);
    qDebug()<<"channel ID:"<<chId+1<<"displayed val: "<<value<<"in Hz: "<<HzValue;
    d->channel->setValue(HzValue);
    d->freqGenService->updateGenerator(d->channel);
}

void MainWindow::onMeasureClick()
{
    if (!d->transferService->deviceOpened())
    {
        qDebug()<<"Cannot start measure! usb device not opened";
        QMessageBox::warning(0, qApp->applicationName()+tr(" warning"), tr("Cannot start measure! Device not connected!"));
        return;
    }

    dto::Command cmd;
    cmd.setType(dto::Command::startMeasure);
    d->transferService->transferCommand(cmd);
    presentationManager->chartWindow()->startReading();
    presentationManager->chartWindow()->show();
}

void MainWindow::initToolBar()
{
    ui->mainToolBar->addAction(QIcon(":/icons/usb_disconnected.png"), "usb connection", this, SLOT(usbToggle()));
}
