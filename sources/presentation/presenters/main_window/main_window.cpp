#include "main_window.h"
#include "ui_mainwindow.h"

#include "da_converter.h"
#include "settings_provider.h"

#include "service_registry.h"
#include "channel.h"
#include "freq_generator_service.h"
#include "unit_conversion.h"

#include <QDebug>

using namespace presentation;

class MainWindow::Impl
{
public:
    dto::DAConverter* DAC;

    domain::FreqGeneratorService* freqGenService;
    dto::ChannelPtr genChannel;

    QMap< QString, QAction* > actionMap;
    QList<QComboBox*> chUnits;
    QList<QDial*> chDials;
    QList<QDoubleSpinBox*> chBoxes;
    int chUnitCurrentIndex = 0;
};

namespace
{
    QList<dto::Channel::Units> availableUnits =
    {
        dto::Channel::Hz, dto::Channel::KHz, dto::Channel::MHz
    };
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), d(new Impl)
{
    ui->setupUi(this);
    ui->menuFile->setEnabled(false);
    d->actionMap["settings"] = ui->actionSettings;
    d->actionMap["about"] = ui->actionAbout;

    d->chUnits = {ui->ch1Units, ui->ch2Units, ui->ch3Units};
    d->chDials =  {ui->ch1Dial, ui->ch2Dial, ui->ch3Dial};
    d->chBoxes = {ui->ch1Box, ui->ch2Box, ui->ch3Box};

    int genChannelAmount = settingsProvider->value(settings::FreqGeneratorSettings::channelCount).toInt();
    if ((d->chUnits.size() != d->chBoxes.size() || d->chBoxes.size() != d->chDials.size())
            && d->chBoxes.size() != genChannelAmount)
        qFatal("amount of generator components must agree");

    for(QComboBox* units : d->chUnits)
    {
        units->addItem("Hz");
        units->addItem("KHz");
        units->addItem("MHz");
    }

    d->freqGenService = serviceRegistry->freqGeneratorService();

    for (QDoubleSpinBox* chBox : d->chBoxes)
    {
        connect(chBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [chBox, this](double value)
        {
            updateGenerator(value, d->chBoxes.indexOf(chBox));

            d->chDials.at(d->chBoxes.indexOf(chBox))->blockSignals(true);
            d->chDials.at(d->chBoxes.indexOf(chBox))->setValue(value);
            d->chDials.at(d->chBoxes.indexOf(chBox))->blockSignals(false);
        });
    }
    for (QDial* chDial : d->chDials)
    {
        connect(chDial, QOverload<int>::of(&QDial::valueChanged), this, [chDial, this](double value)
        {
            updateGenerator(value, d->chDials.indexOf(chDial));

            d->chBoxes.at(d->chDials.indexOf(chDial))->blockSignals(true);
            d->chBoxes.at(d->chDials.indexOf(chDial))->setValue(value);
            d->chBoxes.at(d->chDials.indexOf(chDial))->blockSignals(false);
        });
    }

    for(QComboBox* chUnit : d->chUnits)
    {
        connect(chUnit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [chUnit, this](int index)
        {
            double val = d->chBoxes.at(d->chUnits.indexOf(chUnit))->value();
            double maxFreq = settingsProvider->value(settings::FreqGeneratorSettings::maxFrequency).toDouble();

            val = utils::UnitConversion::frequencyConvert(val,::availableUnits.at(d->chUnitCurrentIndex), ::availableUnits.at(index));
            maxFreq = utils::UnitConversion::frequencyConvert(maxFreq, dto::Channel::Hz, ::availableUnits.at(index));
            int tempCurr = d->chUnitCurrentIndex;
            d->chUnitCurrentIndex = index; //change it here for proper generatorUpdate

            if (index<tempCurr)
            {
                d->chDials.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
            }

            d->chBoxes.at(d->chUnits.indexOf(chUnit))->setValue(val);
            d->chDials.at(d->chUnits.indexOf(chUnit))->setValue(val);

            if (index>tempCurr)
            {
                d->chDials.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
                d->chBoxes.at(d->chUnits.indexOf(chUnit))->setMaximum(maxFreq);
            }


        });
    }

    //TODO: add DAConverterService
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug()<<"main window destroyed";
}

const QMap<QString, QAction*>& MainWindow::actionMap() const
{
    return d->actionMap;
}

void MainWindow::updateGenerator(double value, int chId)
{
    //d->freqGenService->update(d->chBoxes.indexOf(chBox), value);
    d->genChannel = d->freqGenService->load(chId);
    //convert to Hz before set
    double HzValue = utils::UnitConversion::frequencyConvert(value, ::availableUnits.value(d->chUnitCurrentIndex), dto::Channel::Hz);
    qDebug()<<"channel ID:"<<chId+1<<"displayed val: "<<value<<"in Hz: "<<HzValue;
    d->genChannel->setFreq(HzValue);
    d->freqGenService->update(d->genChannel);
}
