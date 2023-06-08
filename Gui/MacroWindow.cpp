
#include "MacroWindow.h"
#include "ui_MacroWindow.h"

#include <iostream>

#include <qstring>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qfiledialog.h>



static void ConvertKeyToMouseClick(uint16_t& key)
{
    if (key == '`')
        key = emm::vk::BUTTON_LEFT;
    if (key == '~')
        key = emm::vk::BUTTON_RIGHT;
}

static void TestData()
{
    auto textData = emm::ReadFile(emm::MACRO_FILE);
    auto fileMacro = emm::decodeObject<std::vector<emm::MacroDef>>(textData.data(), (uint32_t)textData.size());
    emm::MacroCommand cm;
    cm.key = 'a';
    cm.downTimeUs = 5;
    cm.amount = 8;
    cm.timePeriodUs = 11;
    emm::MacroCommand cm2;
    cm2.key = 'b';
    cm2.downTimeUs = 51;
    cm2.amount = 80;
    cm2.timePeriodUs = 21;
    emm::MacroDef myDef;
    myDef.key = 'i';
    myDef.commands.push_back(cm);
    myDef.commands.push_back(cm2);
    std::cout << "cm key is a, hex value of " << std::hex << +cm.key << std::dec << std::endl;
    auto sstring = emm::encodeObject(cm);
    auto scm = emm::decodeObject<emm::MacroCommand>(sstring.data(), (uint32_t)sstring.size());
    std::cout << "the 2 commands are equal: " << (cm.key == scm.key) << ", " << std::hex << +cm.key << " vs " << +scm.key << std::dec << std::endl;
    std::cout << "The 2 commands are Equal? " << (cm == scm) << std::endl;
    std::cout << cm.toString() << std::endl << scm.toString() << std::endl;
    auto string2 = emm::encodeObject(myDef);
    auto myDef2 = emm::decodeObject<emm::MacroDef>(string2.data(), (uint32_t)string2.size());
    std::vector<emm::MacroDef> defList;
    defList.push_back(myDef);
    myDef.key = 'u';
    defList.push_back(myDef);
    string2 = emm::encodeObject(defList);
    emm::OutputFile(std::string(emm::MACRO_FILE), string2.data(), (uint32_t)string2.size());
}


MacroWindow::MacroWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MacroWindow)
{
    ui->setupUi(this);
}
MacroWindow::~MacroWindow() = default;


void MacroWindow::ouputDataToText()
{
    QString text;

    for (const auto &[key, value] : mMacroMap)
    {
        std::string k((char*)&value.key, 1);
        text += QString::fromStdString(k) + "[ ";

        for (const auto& command : value.commands)
            text += QString::fromStdString(command.toString()) + ", ";

        if(!value.commands.empty())
            text.chop(2);
        text += " ]\n";
    }


    ui->textBrowser->setText(text);
}

void MacroWindow::updateOurDropDown()
{
    ui->comboCurrentKey->blockSignals(true);
    ui->comboCurrentKey->clear();

    for (const auto& [key, value] : this->mMacroMap)
    {
        QVariant data = QVariant::fromValue<uint16_t>(key);
        std::string text = "key: " + std::string((const char*)&key,1);
        ui->comboCurrentKey->addItem(QString::fromStdString(text), data);
    }


    ui->comboCurrentKey->blockSignals(false);
}

void MacroWindow::on_buttonAddMacro_pressed()
{
    QString text = ui->lineEditMacroKey->text();
    if (text.isEmpty())
        return;
    auto phyKey = (uint16_t)text.toStdU16String()[0];
    ConvertKeyToMouseClick(phyKey);


    if (!mMacroMap.contains(phyKey))
    {
        mMacroMap[phyKey] = emm::MacroDef();
        mMacroMap.at(phyKey).key = phyKey;
        updateOurDropDown();
        ouputDataToText();
    }
}
void MacroWindow::on_buttonRemoveMacro_pressed()
{
    QString text = ui->lineEditMacroKey->text();
    if (text.isEmpty())
        return;
    auto phyKey = (uint16_t)text.toStdU16String()[0];
    ConvertKeyToMouseClick(phyKey);

    if (mMacroMap.contains(phyKey))
    {
        mMacroMap.erase(phyKey);
        updateOurDropDown();
    }
}

void MacroWindow::on_buttonAddCommand_pressed()
{
    if (ui->comboCurrentKey->currentIndex() < 0)
        return;

    auto lineKey = ui->lineKeyCommand->text();
    auto freq = ui->boxFrequency->value();
    auto amount = ui->boxAmount->value();
    auto downTime = ui->boxDownTime->value();

    if (lineKey.isEmpty() || freq <= 0 || amount <= 0 || downTime <= 0)
        return;

    auto phyKey = ui->comboCurrentKey->currentData().value<uint16_t>();
    ConvertKeyToMouseClick(phyKey);
    auto& item = mMacroMap.at(phyKey);


    emm::MacroCommand c;
    auto ccKey = (uint16_t)lineKey.toStdU16String().front();
    ConvertKeyToMouseClick(ccKey);
    c.key = ccKey;
    c.timePeriodUs = freq;
    c.amount = amount;
    c.downTimeUs = downTime;
    item.commands.push_back(std::move(c));
    ouputDataToText();
}
void MacroWindow::on_buttonRemoveLastCommand_pressed()
{
    if (ui->comboCurrentKey->currentIndex() < 0)
        return;
    auto phyKey = ui->comboCurrentKey->currentData().value<uint16_t>();
    auto& item = mMacroMap.at(phyKey);
    if (!item.commands.empty())
    {
        item.commands.pop_back();
        ouputDataToText();
    }
}
void MacroWindow::on_buttonClearCommand_pressed() 
{
    if (ui->comboCurrentKey->currentIndex() < 0)
        return;
    auto phyKey = ui->comboCurrentKey->currentData().value<uint16_t>();
    auto& item = mMacroMap.at(phyKey);
    item.commands.clear();
    ouputDataToText();
}

void MacroWindow::on_actionLoad_triggered()
{
    mMacroMap.clear();
    auto fileName = QFileDialog::getOpenFileName(this, "Load Macros", "", "Text Files (*.txt)");
    if (fileName.isEmpty())
    {
        updateOurDropDown();
        return;
    }

    auto data = emm::ReadFile(fileName.toStdString());
    auto defList = emm::decodeObject<std::vector<emm::MacroDef>>(data.data(), (uint32_t)data.size());
    for (const auto& item : defList)
        mMacroMap[item.key] = item;

    updateOurDropDown();
    ouputDataToText();
}

void MacroWindow::on_actionSave_triggered()
{
    auto fileName = QFileDialog::getSaveFileName(this,"Save Macros","","Text Files (*.txt)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }


    std::vector<emm::MacroDef> outputToSave;
    for (const auto [key, value] : mMacroMap)
        outputToSave.push_back(value);

    auto data = emm::encodeObject(outputToSave);
    emm::OutputFile(fileName.toStdString(), data.data(), (int)data.size());
}

void MacroWindow::on_actionNew_triggered()
{
    mMacroMap.clear();
    updateOurDropDown();
    ouputDataToText();
}