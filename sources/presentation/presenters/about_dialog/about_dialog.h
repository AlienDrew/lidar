#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>

namespace Ui {
    class AboutDialog;
}
namespace presentation {
    class AboutDialog : public QDialog
    {
        Q_OBJECT

    public:
         AboutDialog(QWidget *parent = 0);
        ~AboutDialog() override;

    private:
        Ui::AboutDialog* ui;
    };
}

#endif // ABOUT_DIALOG_H
