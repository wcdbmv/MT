#include "main_window.h"
#include "./ui_main_window.h"

#include <cassert>
#include <cmath>

#include <QArgument>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include "xe_paint_widget.h"

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
  connect(ui->xeRDoubleSpinBox, &QDoubleSpinBox::valueChanged,
          ui->xePaintWidget, [this](double) { ui->xePaintWidget->update(); });

  ConnectSpinBoxAndSlider(ui->xeNSpinBox, ui->xeNHorizontalSlider);
  connect(ui->xeNSpinBox, &QSpinBox::valueChanged, ui->xePaintWidget,
          [this](int) { ui->xePaintWidget->update(); });

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

  connect(ui->xeCalculatePushButton, &QPushButton::clicked, [] {

  });
}

MainWindow::~MainWindow() {
  delete ui;
}
