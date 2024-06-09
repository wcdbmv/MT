#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <cmath>

namespace {

int Round(double value) {
  return static_cast<int>(std::round(value));
}

void ConnectSpinBoxAndSlider(QDoubleSpinBox* spin_box, QSlider* slider) {
  const auto spin_box_minimum = spin_box->minimum();
  const auto spin_box_maximum = spin_box->maximum();
  const auto spin_box_step = spin_box->singleStep();
  const auto spin_box_count = Round((spin_box_maximum - spin_box_minimum) / spin_box_step);

  const auto slider_minimum = slider->minimum();
  const auto slider_maximum = slider->maximum();
  const auto slider_step = slider->singleStep();
  const auto slider_count = (slider_maximum - slider_minimum) / slider_step;

  assert(spin_box_count == slider_count);

  QObject::connect(spin_box, &QDoubleSpinBox::valueChanged, slider, [=](double value) {
    const auto count = Round((value - spin_box_minimum) / spin_box_step);
    slider->setValue(slider_minimum + slider_step * count);
  });

  QObject::connect(slider, &QSlider::valueChanged, spin_box, [=](int value) {
    const auto count = (value - slider_minimum) / slider_step;
    spin_box->setValue(spin_box_minimum + spin_box_step * count);
  });
}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ConnectSpinBoxAndSlider(ui->rDoubleSpinBox, ui->rHorizontalSlider);
}

MainWindow::~MainWindow() {
  delete ui;
}
