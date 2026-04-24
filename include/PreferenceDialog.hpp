#include <qdialog.h>
#include <qsettings.h>
#include <qspinbox.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class PreferenceDialog : public QDialog
{
    Q_OBJECT

   public:
    PreferenceDialog(QWidget* parent, QSettings& setting);

   signals:
    void settingsChanged();

   private:
    QSpinBox* m_heightSpinBox;
    QSpinBox* m_widthSpinBox;

    QSettings& m_settings;

    void closeEvent(QCloseEvent*) override;
};