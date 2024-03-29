#include "qtcreditcalcview.h"

#include "ui_qtcreditcalcview.h"

QtCreditCalcView::QtCreditCalcView(QWidget *parent)
    : QWidget(parent),
      controller((BaseModel *)&model),
      ui(new Ui::QtCreditCalcView) {
  ui->setupUi(this);

  // MVC staff
  model.registerObserver(this);

  // Setup sliders & spin boxes
  // Copy default values before connecting
  ui->creditTermSlider->setValue(ui->creditTermSpinBox->value());
  ui->creditAmountSlider->setValue(ui->creditAmountSpinBox->value());
  ui->interestRateSlider->setValue(ui->interestRateSpinBox->value());
  // Connect sliders and spins together
  connect(ui->creditTermSpinBox, SIGNAL(valueChanged(int)),
          ui->creditTermSlider, SLOT(setValue(int)));
  connect(ui->creditTermSlider, SIGNAL(valueChanged(int)),
          ui->creditTermSpinBox, SLOT(setValue(int)));
  connect(ui->creditAmountSpinBox, SIGNAL(valueChanged(double)),
          ui->creditAmountSlider, SLOT(doubleSetValue(double)));
  connect(ui->creditAmountSlider, SIGNAL(doubleValueChanged(double)),
          ui->creditAmountSpinBox, SLOT(setValue(double)));
  connect(ui->interestRateSpinBox, SIGNAL(valueChanged(double)),
          ui->interestRateSlider, SLOT(doubleSetValue(double)));
  connect(ui->interestRateSlider, SIGNAL(doubleValueChanged(double)),
          ui->interestRateSpinBox, SLOT(setValue(double)));
}

QtCreditCalcView::~QtCreditCalcView() { delete ui; }

void QtCreditCalcView::observerUpdate(const CreditModelData *model_data) {
  m_data = *(model_data);
  QLocale rus(QLocale::Russian, QLocale::Russia);
  rus.setNumberOptions(QLocale::DefaultNumberOptions);
  ui->monthlyPaymentLabel->setText(
      rus.toCurrencyString(m_data.monthly_payment) +
      ((m_data.type == m_data.kDifferentiated)
           ? " ... " + rus.toCurrencyString(m_data.monthly_payment_min)
           : ""));
  ui->overPaymentLabel->setText(rus.toCurrencyString(m_data.overpayment));
  ui->totalPaymentLabel->setText(rus.toCurrencyString(m_data.total_payment));
  ui->resultFrame->setEnabled(true);
}

void QtCreditCalcView::on_calculateButton_pressed() {
  m_data.type = ui->creditTypeComboBox->currentText() == "Annuity"
                    ? m_data.kAnnuity
                    : m_data.kDifferentiated;
  m_data.amount = ui->creditAmountSpinBox->value();
  m_data.duration = ui->creditTermSpinBox->value();
  m_data.rate = ui->interestRateSpinBox->value();
  m_data.round = ui->round->isChecked();
  try {
    controller.userAction((BaseCalcData *)&m_data);
    emit showStatus("Success");
  } catch (std::invalid_argument &e) {
    ui->resultFrame->setEnabled(false);
    emit showStatus(e.what());
  }
}
