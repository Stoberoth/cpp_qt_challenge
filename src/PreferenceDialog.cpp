#include <qboxlayout.h>
#include <qdialog.h>
#include <qevent.h>
#include <qsettings.h>
#include <qsize.h>
#include <qspinbox.h>
#include <qwidget.h>

#include <PreferenceDialog.hpp>

PreferenceDialog::PreferenceDialog(QWidget* parent, QSettings& setting) : QDialog(parent), m_settings(setting)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* layout = new QVBoxLayout();

    m_heightSpinBox = new QSpinBox(this);
    m_widthSpinBox = new QSpinBox(this);

    m_heightSpinBox->setMaximum(3000);
    m_widthSpinBox->setMaximum(3000);

    m_heightSpinBox->setPrefix("Height : ");
    m_widthSpinBox->setPrefix("Width : ");

    m_heightSpinBox->setValue(m_settings.value("window/size", QSize(800, 600)).toSize().height());
    m_widthSpinBox->setValue(m_settings.value("window/size", QSize(800, 600)).toSize().width());

    layout->addWidget(m_heightSpinBox);
    layout->addWidget(m_widthSpinBox);
    this->setLayout(layout);
}

void PreferenceDialog::closeEvent(QCloseEvent* event)
{
    m_settings.setValue("window/size", QSize(m_widthSpinBox->value(), m_heightSpinBox->value()));
    m_settings.sync();

    emit settingsChanged();
}