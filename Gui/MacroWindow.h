
#ifndef MACROWINDOW_H
#define MACROWINDOW_H

//#include "MacroDef.h"
#include <Macro/MacroDef.h>


#include <QMainWindow>

#include <memory>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class MacroWindow; }
QT_END_NAMESPACE

class MacroWindow : public QMainWindow
{
    Q_OBJECT

public:
    MacroWindow(QWidget *parent = nullptr);
    ~MacroWindow();

private slots:
    void on_buttonAddMacro_pressed();
    void on_buttonRemoveMacro_pressed();
    void on_buttonAddCommand_pressed();
    void on_buttonRemoveLastCommand_pressed();
    void on_buttonClearCommand_pressed();


    void on_actionLoad_triggered();
    void on_actionSave_triggered();
    void on_actionNew_triggered();

private:
    void ouputDataToText();
    void updateOurDropDown();


    std::unique_ptr<Ui::MacroWindow> ui;

    std::map<uint16_t, emm::MacroDef> mMacroMap;
};

#endif // MacroWindow
