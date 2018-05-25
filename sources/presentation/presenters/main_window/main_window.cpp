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

    QMap< QString, QAction* > actionMap;
    //==generator==
    QList<QComboBox*> chUnits;
    QList<QPushButton*> chButtons;
    QList<QDoubleSpinBox*> chBoxes;
    QList<Switch*> chSwitches;
    //========
    QLabel* statusIcon;
};

namespace
{
    QList<dto::Channel::FrequencyUnits> availableUnits =
    {
        dto::Channel::Hz, dto::Channel::KHz, dto::Channel::MHz
    };
    const char* prevIndex = "prevIndex";
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
    d->chSwitches = {new Switch(this), new Switch(this)};

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
    ui->ch1Box->setValue(10);
    ui->ch2Box->setValue(10.0050);
    ui->ch1Units->setCurrentIndex(2);
    ui->ch2Units->setCurrentIndex(2);
    ui->ch1Units->setProperty(::prevIndex, 2);
    ui->ch2Units->setProperty(::prevIndex, 2);

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

    //=====generator=====
    QGridLayout* generatorLayout = (QGridLayout*) ui->generatorGroup->layout();
    generatorLayout->addWidget(d->chSwitches.at(0), 0, 2, Qt::AlignRight);
    generatorLayout->addWidget(d->chSwitches.at(1), 3, 2, Qt::AlignRight);
    for (Switch* chSwitch : d->chSwitches)
    {
        chSwitch->setMaximumWidth(40);
        chSwitch->setMaximumWidth(40);
        connect(chSwitch, &Switch::toggled, this, [=](bool isOn)
        {
            dto::Command cmd;
            cmd.setType(dto::Command::gen_channel_switch);
            cmd.addArgument(d->chSwitches.indexOf(chSwitch));
            cmd.addArgument(isOn);
            if (!d->transferService->transferCommand(cmd))
                chSwitch->setToggle(!isOn);
        });
    }

    for (QPushButton* chButton : d->chButtons)
    {
        connect(chButton, &QPushButton::clicked, this, [chButton, this] ()
        {
            int chId = d->chButtons.indexOf(chButton);
            qreal minFreq = settingsProvider->value(settings::freq_generator::minFrequency).toDouble();
            qreal maxFreq = settingsProvider->value(settings::freq_generator::maxFrequency).toInt();
            qreal value = d->chBoxes.at(chId)->value();
            if ( value >=
                    utils::UnitConversion::frequencyConvert(minFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnits.at(chId)->currentIndex()))
                 && value <= utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnits.at(chId)->currentIndex()))
                 )
                d->freqGenService->updateGenerator(chId,
                                                   utils::UnitConversion::frequencyConvert(value,
                                                                                           ::availableUnits.value(d->chUnits.at(chId)->currentIndex()),
                                                                                           dto::Channel::Hz));
            else
                QMessageBox::warning(0, qApp->applicationName()+tr(" warning"), tr("Frequency out of range!\nMin value: ")+
                                     QString::number(utils::UnitConversion::frequencyConvert(minFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnits.at(chId)->currentIndex())))+
                                     " "+d->chUnits.at(chId)->currentText()+tr("\nMax value: ")+
                                     QString::number(utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(d->chUnits.at(chId)->currentIndex())))+
                                     " "+d->chUnits.at(chId)->currentText());
        });
    }

    for(QComboBox* chUnit : d->chUnits)
    {
        connect(chUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [chUnit, this](int index)
        {
            if (settingsProvider->value(settings::freq_generator::autoUnitConversion).toBool())
            {
                int chId = d->chUnits.indexOf(chUnit);
                double val = d->chBoxes.at(chId)->value();
                double minFreq = settingsProvider->value(settings::freq_generator::minFrequency).toDouble();
                double maxFreq = settingsProvider->value(settings::freq_generator::maxFrequency).toInt();

                val = utils::UnitConversion::frequencyConvert(val, ::availableUnits.at(chUnit->property(::prevIndex).toInt()), ::availableUnits.at(index));
                minFreq = utils::UnitConversion::frequencyConvert(minFreq, dto::Channel::Hz, ::availableUnits.at(index));
                maxFreq = utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(index));
                chUnit->setProperty(::prevIndex, index); //change it here for proper generatorUpdate

                if (::availableUnits.at(index) == dto::Channel::Hz)
                    d->chBoxes.at(d->chUnits.indexOf(chUnit))->setDecimals(0);
                else if (::availableUnits.at(index) == dto::Channel::KHz)
                    d->chBoxes.at(d->chUnits.indexOf(chUnit))->setDecimals(3);
                else
                    d->chBoxes.at(d->chUnits.indexOf(chUnit))->setDecimals(4);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMinimum(minFreq);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setValue(val);
            }
        });
    }
    //================================

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

void MainWindow::onMeasureClick()
{
    if (!d->transferService->deviceOpened())
    {
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
