#include "main_window.h"
#include "./ui_main_window.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QTextEdit>

#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QString>

#include "xe_paint_widget.h"
#include "xe_plot_widget.h"

#include "base/config/float.h"
#include "physics/params/xenon_absorption_coefficient.h"

namespace {

int Round(double value) {
  return static_cast<int>(std::round(value));
}

void ConnectDoubleSpinBoxAndSlider(QDoubleSpinBox* spin_box, QSlider* slider) {
  const auto spin_box_minimum = spin_box->minimum();
  const auto spin_box_maximum = spin_box->maximum();
  const auto spin_box_step = spin_box->singleStep();
  const auto spin_box_count =
      Round((spin_box_maximum - spin_box_minimum) / spin_box_step);

  const auto slider_minimum = slider->minimum();
  const auto slider_maximum = slider->maximum();
  const auto slider_step = slider->singleStep();
  const auto slider_count = (slider_maximum - slider_minimum) / slider_step;

  assert(spin_box_count == slider_count);

  QObject::connect(
      spin_box, &QDoubleSpinBox::valueChanged, slider, [=](double value) {
        const auto count = Round((value - spin_box_minimum) / spin_box_step);
        const auto slider_value = slider_minimum + slider_step * count;
        if (slider_value != slider->value()) {
          slider->setValue(slider_value);
        }
      });

  QObject::connect(slider, &QSlider::valueChanged, spin_box, [=](int value) {
    const auto count = (value - slider_minimum) / slider_step;
    const auto spin_box_count =
        Round((spin_box->value() - spin_box_minimum) / spin_box_step);
    if (count != spin_box_count) {
      spin_box->setValue(spin_box_minimum + spin_box_step * count);
    }
  });
}

void ConnectSpinBoxAndSlider(QSpinBox* spin_box, QSlider* slider) {
  const auto spin_box_minimum = spin_box->minimum();
  const auto spin_box_maximum = spin_box->maximum();
  const auto spin_box_step = spin_box->singleStep();
  const auto spin_box_count =
      (spin_box_maximum - spin_box_minimum) / spin_box_step;

  const auto slider_minimum = slider->minimum();
  const auto slider_maximum = slider->maximum();
  const auto slider_step = slider->singleStep();
  const auto slider_count = (slider_maximum - slider_minimum) / slider_step;

  assert(spin_box_count == slider_count);

  QObject::connect(spin_box, &QSpinBox::valueChanged, slider, [=](int value) {
    const auto count = (value - spin_box_minimum) / spin_box_step;
    const auto slider_value = slider_minimum + slider_step * count;
    if (slider_value != slider->value()) {
      slider->setValue(slider_value);
    }
  });

  QObject::connect(slider, &QSlider::valueChanged, spin_box, [=](int value) {
    const auto count = (value - slider_minimum) / slider_step;
    const auto spin_box_value = spin_box_minimum + spin_box_step * count;
    if (spin_box_value != spin_box->value()) {
      spin_box->setValue(spin_box_value);
    }
  });
}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ConnectDoubleSpinBoxAndSlider(ui->xeRDoubleSpinBox, ui->xeRHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeNSpinBox, ui->xeNHorizontalSlider);
  connect(ui->xeNSpinBox, &QSpinBox::valueChanged, ui->xePaintWidget,
          [this](int) {
            xe_res.reset();
            ui->xePaintWidget->update();
          });

  ConnectSpinBoxAndSlider(ui->xeT0SpinBox, ui->xeT0HorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeTwSpinBox, ui->xeTwHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeMSpinBox, ui->xeMHorizontalSlider);

  ConnectDoubleSpinBoxAndSlider(ui->xeRhoDoubleSpinBox,
                                ui->xeRhoHorizontalSlider);

  ConnectSpinBoxAndSlider(ui->xeNMeridianSpinBox,
                          ui->xeNMeridianHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeNLatitudeSpinBox,
                          ui->xeNLatitudeHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeNThreadsSpinBox,
                          ui->xeNThreadsHorizontalSlider);

  ui->xeI2PlotWidget->setAxisY("I [Вт/см^2]");
  ui->xeI3PlotWidget->setAxisY("I [Вт/см^3]");

  connect(ui->xeCalculatePushButton, &QPushButton::clicked, [this] {
    const auto nu_idx =
        static_cast<std::size_t>(ui->xeDeltaNuComboBox->currentIndex());
    const auto nu_min = kXenonFrequency[nu_idx];
    const auto nu_max = kXenonFrequency[nu_idx + 1];
    const auto d_nu = nu_max - nu_min;
    const auto nu_avg = nu_min + d_nu / 2;

    const CylinderPlasma::Params params{
        .r = ui->xeRDoubleSpinBox->value(),
        .n_plasma = static_cast<std::size_t>(ui->xeNSpinBox->value()),

        .t0 = static_cast<Float>(ui->xeT0SpinBox->value()),
        .tw = static_cast<Float>(ui->xeTwSpinBox->value()),
        .m = ui->xeMSpinBox->value(),

        .rho = ui->xeRhoDoubleSpinBox->value(),

        .nu = nu_avg,
        .d_nu = d_nu,

        .n_meridian = static_cast<std::size_t>(ui->xeNMeridianSpinBox->value()),
        .n_latitude = static_cast<std::size_t>(ui->xeNLatitudeSpinBox->value()),

        .n_threads = static_cast<std::size_t>(ui->xeNThreadsSpinBox->value()),
    };

    xe_res = CylinderPlasma{params}.Solve();

    ui->xePaintWidget->update();

    ui->xeI2PlotWidget->setData(xe_res->absorbed_plasma);
    ui->xeI3PlotWidget->setData(xe_res->absorbed_plasma3);

    auto total_plasma = kZero;
    QString strI2;
    for (auto i2 : xe_res->absorbed_plasma) {
      total_plasma += i2;
      strI2 += QString::number(i2);
      strI2 += '\n';
    }
    ui->xeDataI2TextEdit->setText(strI2);

    QString strI3;
    for (auto i3 : xe_res->absorbed_plasma3) {
      strI3 += QString::number(i3);
      strI3 += '\n';
    }
    ui->xeDataI3TextEdit->setText(strI3);

    ui->xeDataTotalIntensityLineEdit->setText(
        QString::number(xe_res->intensity_all));
    ui->xeDataTotalAbsorbedPlasmaLineEdit->setText(
        QString::number(total_plasma));
    ui->xeDataTotalAbsorbedMirrorLineEdit->setText(
        QString::number(xe_res->absorbed_mirror));
  });
}

MainWindow::~MainWindow() {
  delete ui;
}
