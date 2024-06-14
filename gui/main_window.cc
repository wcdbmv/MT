#include "main_window.h"
#include "./ui_main_window.h"

#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <format>
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
#include "base/ignore_unused.h"
#include "math/consts/pi.h"
#include "math/fast_pow.h"
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
  IgnoreUnused(spin_box_count);
  IgnoreUnused(slider_count);

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
  IgnoreUnused(spin_box_count);
  IgnoreUnused(slider_count);

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

template <typename Params>
bool OnlyNThreadsDiffers(const Params& a, Params b) {
  if (a.n_threads == b.n_threads) {
    return false;
  }

  b.n_threads = a.n_threads;
  return memcmp(&a, &b, sizeof a) == 0;
}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  InitXeTab();
  InitXeSiO2Tab();
  InitXeXeSiO2Tab();
}

void MainWindow::InitXeTab() {
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

    CylinderPlasma::Params params{
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

    auto ignore_result = false;
    if (xe_params.has_value()) {
      ignore_result = OnlyNThreadsDiffers(*xe_params, params);
    }
    params.i_crit *= static_cast<Float>(params.n_threads);

    const auto start_ts = std::chrono::high_resolution_clock::now();
    auto res = CylinderPlasma{params}.Solve();
    const auto time = std::chrono::high_resolution_clock::now() - start_ts;

    if (!ignore_result) {
      xe_params = params;
      xe_res = std::move(res);
    }

    const auto message = std::format(
        "Время моделирования: {}{}",
        std::chrono::duration_cast<std::chrono::seconds>(time),
        std::chrono::duration_cast<std::chrono::milliseconds>(time) % 1000);
    ui->statusBar->showMessage(QString::fromStdString(message));

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

void MainWindow::InitXeSiO2Tab() {
  ConnectDoubleSpinBoxAndSlider(ui->xeSiO2RDoubleSpinBox,
                                ui->xeSiO2RHorizontalSlider);
  connect(ui->xeSiO2RDoubleSpinBox, &QDoubleSpinBox::valueChanged,
          ui->xeSiO2PaintWidget, [this](double) {
            xe_sio2_res.reset();
            ui->xeSiO2PaintWidget->update();
          });
  ConnectSpinBoxAndSlider(ui->xeSiO2NPlasmaSpinBox,
                          ui->xeSiO2NPlasmaHorizontalSlider);
  connect(ui->xeSiO2NPlasmaSpinBox, &QSpinBox::valueChanged,
          ui->xeSiO2PaintWidget, [this](int) {
            xe_sio2_res.reset();
            ui->xeSiO2PaintWidget->update();
          });

  ConnectDoubleSpinBoxAndSlider(ui->xeSiO2DeltaDoubleSpinBox,
                                ui->xeSiO2DeltaHorizontalSlider);
  connect(ui->xeSiO2DeltaDoubleSpinBox, &QDoubleSpinBox::valueChanged,
          ui->xeSiO2PaintWidget, [this](double) {
            xe_sio2_res.reset();
            ui->xeSiO2PaintWidget->update();
          });
  ConnectSpinBoxAndSlider(ui->xeSiO2NQuartzSpinBox,
                          ui->xeSiO2NQuartzHorizontalSlider);
  connect(ui->xeSiO2NQuartzSpinBox, &QSpinBox::valueChanged,
          ui->xeSiO2PaintWidget, [this](int) {
            xe_sio2_res.reset();
            ui->xeSiO2PaintWidget->update();
          });

  ConnectSpinBoxAndSlider(ui->xeSiO2T0SpinBox, ui->xeSiO2T0HorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeSiO2TwSpinBox, ui->xeSiO2TwHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeSiO2MSpinBox, ui->xeSiO2MHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeSiO2T1SpinBox, ui->xeSiO2T1HorizontalSlider);

  ConnectDoubleSpinBoxAndSlider(ui->xeSiO2EtaPlasmaDoubleSpinBox,
                                ui->xeSiO2EtaPlasmaHorizontalSlider);
  ConnectDoubleSpinBoxAndSlider(ui->xeSiO2EtaQuartzDoubleSpinBox,
                                ui->xeSiO2EtaQuartzHorizontalSlider);
  ConnectDoubleSpinBoxAndSlider(ui->xeSiO2RhoDoubleSpinBox,
                                ui->xeSiO2RhoHorizontalSlider);

  ConnectSpinBoxAndSlider(ui->xeSiO2NMeridianSpinBox,
                          ui->xeSiO2NMeridianHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeSiO2NLatitudeSpinBox,
                          ui->xeSiO2NLatitudeHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xeSiO2NThreadsSpinBox,
                          ui->xeSiO2NThreadsHorizontalSlider);

  ui->xeSiO2I2PlotWidget->setAxisY("I [Вт/см^2]");
  ui->xeSiO2I3PlotWidget->setAxisY("I [Вт/см^3]");

  connect(ui->xeSiO2CalculatePushButton, &QPushButton::clicked, [this] {
    const auto nu_idx =
        static_cast<std::size_t>(ui->xeSiO2DeltaNuComboBox->currentIndex());
    const auto nu_min = kXenonFrequency[nu_idx];
    const auto nu_max = kXenonFrequency[nu_idx + 1];
    const auto d_nu = nu_max - nu_min;
    const auto nu_avg = nu_min + d_nu / 2;

    CylinderPlasmaQuartz::Params params{
        .r = ui->xeSiO2RDoubleSpinBox->value(),
        .n_plasma = static_cast<std::size_t>(ui->xeSiO2NPlasmaSpinBox->value()),

        .delta = ui->xeSiO2DeltaDoubleSpinBox->value(),
        .n_quartz = static_cast<std::size_t>(ui->xeSiO2NQuartzSpinBox->value()),

        .t0 = static_cast<Float>(ui->xeSiO2T0SpinBox->value()),
        .tw = static_cast<Float>(ui->xeSiO2TwSpinBox->value()),
        .m = ui->xeSiO2MSpinBox->value(),
        .t1 = static_cast<Float>(ui->xeSiO2T1SpinBox->value()),

        .eta_plasma = ui->xeSiO2EtaPlasmaDoubleSpinBox->value(),
        .eta_quartz = ui->xeSiO2EtaQuartzDoubleSpinBox->value(),
        .rho = ui->xeSiO2RhoDoubleSpinBox->value(),

        .nu = nu_avg,
        .d_nu = d_nu,

        .n_meridian =
            static_cast<std::size_t>(ui->xeSiO2NMeridianSpinBox->value()),
        .n_latitude =
            static_cast<std::size_t>(ui->xeSiO2NLatitudeSpinBox->value()),

        .n_threads =
            static_cast<std::size_t>(ui->xeSiO2NThreadsSpinBox->value()),
        .i_crit = ui->xeSiO2ICritLineEdit->text().toDouble(),
    };

    auto ignore_result = false;
    if (xe_sio2_params.has_value()) {
      ignore_result = OnlyNThreadsDiffers(*xe_sio2_params, params);
    }
    params.i_crit *= static_cast<Float>(params.n_threads);

    const auto start_ts = std::chrono::high_resolution_clock::now();
    auto res = CylinderPlasmaQuartz{params}.Solve();
    const auto time = std::chrono::high_resolution_clock::now() - start_ts;

    if (!ignore_result) {
      xe_sio2_params = params;
      xe_sio2_res = std::move(res);
    }

    const auto message = std::format(
        "Время моделирования: {}{}",
        std::chrono::duration_cast<std::chrono::seconds>(time),
        std::chrono::duration_cast<std::chrono::milliseconds>(time) % 1000);
    ui->statusBar->showMessage(QString::fromStdString(message));

    ui->xeSiO2PaintWidget->update();

    ui->xeSiO2I2PlotWidget->setData(xe_sio2_res->absorbed_plasma,
                                    xe_sio2_res->absorbed_quartz, params.r,
                                    params.delta);
    ui->xeSiO2I3PlotWidget->setData(xe_sio2_res->absorbed_plasma3,
                                    xe_sio2_res->absorbed_quartz3, params.r,
                                    params.delta);

    auto total_plasma = kZero;
    auto total_quartz = kZero;
    QString strI2;
    for (auto i2 : xe_sio2_res->absorbed_plasma) {
      total_plasma += i2;
      strI2 += QString::number(i2);
      strI2 += '\n';
    }
    strI2 += "————————\n";
    for (std::size_t i = 1; i < xe_sio2_res->absorbed_quartz.size(); ++i) {
      auto i2 = xe_sio2_res->absorbed_quartz[i];
      total_quartz += i2;
      strI2 += QString::number(i2);
      strI2 += '\n';
    }
    ui->xeSiO2DataI2TextEdit->setText(strI2);

    QString strI3;
    for (auto i3 : xe_sio2_res->absorbed_plasma3) {
      strI3 += QString::number(i3);
      strI3 += '\n';
    }
    strI3 += "————————\n";
    for (std::size_t i = 1; i < xe_sio2_res->absorbed_quartz3.size(); ++i) {
      auto i3 = xe_sio2_res->absorbed_quartz3[i];
      strI3 += QString::number(i3);
      strI3 += '\n';
    }
    ui->xeSiO2DataI3TextEdit->setText(strI3);

    ui->xeSiO2DataTotalIntensityLineEdit->setText(
        QString::number(xe_sio2_res->intensity_all));
    ui->xeSiO2DataTotalAbsorbedPlasmaLineEdit->setText(
        QString::number(total_plasma));
    ui->xeSiO2DataTotalAbsorbedQuartzLineEdit->setText(
        QString::number(total_quartz));
    ui->xeSiO2DataTotalAbsorbedMirrorLineEdit->setText(
        QString::number(xe_sio2_res->absorbed_mirror));
  });
}

void MainWindow::InitXeXeSiO2Tab() {
  ConnectDoubleSpinBoxAndSlider(ui->xexeSiO2RDoubleSpinBox,
                                ui->xexeSiO2RHorizontalSlider);
  connect(ui->xexeSiO2RDoubleSpinBox, &QDoubleSpinBox::valueChanged,
          ui->xexeSiO2PaintWidget, [this](double) {
            xe_xe_sio2_res.reset();
            ui->xexeSiO2PaintWidget->image = QImage{};
            ui->xexeSiO2PaintWidget->update();
          });
  ConnectSpinBoxAndSlider(ui->xexeSiO2NPlasmaSpinBox,
                          ui->xexeSiO2NPlasmaHorizontalSlider);
  connect(ui->xexeSiO2NPlasmaSpinBox, &QSpinBox::valueChanged,
          ui->xexeSiO2PaintWidget, [this](int) {
            xe_xe_sio2_res.reset();
            ui->xexeSiO2PaintWidget->image = QImage{};
            ui->xexeSiO2PaintWidget->update();
          });

  ConnectDoubleSpinBoxAndSlider(ui->xexeSiO2DeltaDoubleSpinBox,
                                ui->xexeSiO2DeltaHorizontalSlider);
  connect(ui->xexeSiO2DeltaDoubleSpinBox, &QDoubleSpinBox::valueChanged,
          ui->xexeSiO2PaintWidget, [this](double) {
            xe_xe_sio2_res.reset();
            ui->xexeSiO2PaintWidget->image = QImage{};
            ui->xexeSiO2PaintWidget->update();
          });
  ConnectDoubleSpinBoxAndSlider(ui->xexeSiO2ADoubleSpinBox,
                                ui->xexeSiO2AHorizontalSlider);
  connect(ui->xexeSiO2ADoubleSpinBox, &QDoubleSpinBox::valueChanged,
          ui->xexeSiO2PaintWidget, [this](double) {
            xe_xe_sio2_res.reset();
            ui->xexeSiO2PaintWidget->image = QImage{};
            ui->xexeSiO2PaintWidget->update();
          });
  ConnectDoubleSpinBoxAndSlider(ui->xexeSiO2BDoubleSpinBox,
                                ui->xexeSiO2BHorizontalSlider);
  connect(ui->xexeSiO2BDoubleSpinBox, &QDoubleSpinBox::valueChanged,
          ui->xexeSiO2PaintWidget, [this](double) {
            xe_xe_sio2_res.reset();
            ui->xexeSiO2PaintWidget->image = QImage{};
            ui->xexeSiO2PaintWidget->update();
          });
  ConnectSpinBoxAndSlider(ui->xexeSiO2NQuartzSpinBox,
                          ui->xexeSiO2NQuartzHorizontalSlider);
  connect(ui->xexeSiO2NQuartzSpinBox, &QSpinBox::valueChanged,
          ui->xexeSiO2PaintWidget, [this](int) {
            xe_xe_sio2_res.reset();
            ui->xexeSiO2PaintWidget->image = QImage{};
            ui->xexeSiO2PaintWidget->update();
          });

  ConnectSpinBoxAndSlider(ui->xexeSiO2T0SpinBox,
                          ui->xexeSiO2T0HorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xexeSiO2TwSpinBox,
                          ui->xexeSiO2TwHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xexeSiO2MSpinBox, ui->xexeSiO2MHorizontalSlider);

  ConnectDoubleSpinBoxAndSlider(ui->xexeSiO2EtaPlasmaDoubleSpinBox,
                                ui->xexeSiO2EtaPlasmaHorizontalSlider);
  ConnectDoubleSpinBoxAndSlider(ui->xexeSiO2EtaQuartzDoubleSpinBox,
                                ui->xexeSiO2EtaQuartzHorizontalSlider);
  ConnectDoubleSpinBoxAndSlider(ui->xexeSiO2RhoDoubleSpinBox,
                                ui->xexeSiO2RhoHorizontalSlider);

  ConnectSpinBoxAndSlider(ui->xexeSiO2NMeridianSpinBox,
                          ui->xexeSiO2NMeridianHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xexeSiO2NLatitudeSpinBox,
                          ui->xexeSiO2NLatitudeHorizontalSlider);
  ConnectSpinBoxAndSlider(ui->xexeSiO2NThreadsSpinBox,
                          ui->xexeSiO2NThreadsHorizontalSlider);

  ui->xexeSiO2I2PlotWidget->setAxisY("I [Вт/см^2]");
  ui->xexeSiO2I3PlotWidget->setAxisY("I [Вт/см^3]");

  connect(ui->xexeSiO2CalculatePushButton, &QPushButton::clicked, [this] {
    const auto nu_idx =
        static_cast<std::size_t>(ui->xexeSiO2DeltaNuComboBox->currentIndex());
    const auto nu_min = kXenonFrequency[nu_idx];
    const auto nu_max = kXenonFrequency[nu_idx + 1];
    const auto d_nu = nu_max - nu_min;
    const auto nu_avg = nu_min + d_nu / 2;

    const auto a = ui->xexeSiO2ADoubleSpinBox->value();
    const auto b = ui->xexeSiO2BDoubleSpinBox->value();

    CylinderPlasmaQuartz::Params params{
        .r = ui->xexeSiO2RDoubleSpinBox->value(),
        .n_plasma =
            static_cast<std::size_t>(ui->xexeSiO2NPlasmaSpinBox->value()),

        .delta = std::sqrt(a * b),
        .n_quartz =
            static_cast<std::size_t>(ui->xexeSiO2NQuartzSpinBox->value()),

        .t0 = static_cast<Float>(ui->xexeSiO2T0SpinBox->value()),
        .tw = static_cast<Float>(ui->xexeSiO2TwSpinBox->value()),
        .m = ui->xexeSiO2MSpinBox->value(),
        .t1 = static_cast<Float>(ui->xexeSiO2TwSpinBox->value()),

        .eta_plasma = ui->xexeSiO2EtaPlasmaDoubleSpinBox->value(),
        .eta_quartz = ui->xexeSiO2EtaQuartzDoubleSpinBox->value(),
        .rho = ui->xexeSiO2RhoDoubleSpinBox->value(),

        .nu = nu_avg,
        .d_nu = d_nu,

        .n_meridian =
            static_cast<std::size_t>(ui->xexeSiO2NMeridianSpinBox->value()),
        .n_latitude =
            static_cast<std::size_t>(ui->xexeSiO2NLatitudeSpinBox->value()),

        .n_threads =
            static_cast<std::size_t>(ui->xexeSiO2NThreadsSpinBox->value()),
        .i_crit = ui->xexeSiO2ICritLineEdit->text().toDouble(),
    };

    {
      const auto w = ui->xexeSiO2PaintWidget->width();
      const auto h = ui->xexeSiO2PaintWidget->height();

      QPixmap pixmap{w, h};
      QPainter painter{&pixmap};
      painter.fillRect(rect(), QBrush{Qt::white});

      const auto scale = std::min(w / a, h / b) / 1.01;
      const auto side_a = static_cast<int>(scale * a);
      const auto side_b = static_cast<int>(scale * b);

      const auto delta = ui->xexeSiO2DeltaDoubleSpinBox->value();
      const auto side_plasma = static_cast<int>(scale * params.r);
      const auto delta_scale = static_cast<int>(scale * delta);

      painter.setPen(QPen{Qt::red, 1});
      painter.setBrush(QBrush{Qt::red});
      painter.drawEllipse((w - delta_scale) / 2 - side_plasma,
                          (h - side_plasma) / 2, side_plasma, side_plasma);
      painter.drawEllipse((w + delta_scale) / 2, (h - side_plasma) / 2,
                          side_plasma, side_plasma);

      painter.setPen(QPen{Qt::black, 1});
      painter.setBrush(QBrush{Qt::white});
      painter.drawEllipse((w - side_a) / 2, (h - side_b) / 2, side_a, side_b);

      const auto image = pixmap.toImage();
      for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
          if (image.pixelColor(i, j) == Qt::red) {
            ui->statusBar->showMessage("Неправильная геометрия системы");
            ui->xexeSiO2PaintWidget->image = std::move(image);
            ui->xexeSiO2PaintWidget->update();
            return;
          }
        }
      }
    }

    auto ignore_result = false;
    if (xe_xe_sio2_params.has_value()) {
      ignore_result = OnlyNThreadsDiffers(*xe_xe_sio2_params, params);
    }
    params.i_crit *= static_cast<Float>(params.n_threads);

    const auto start_ts = std::chrono::high_resolution_clock::now();
    auto res = CylinderPlasmaQuartz{params}.Solve();
    const auto time = std::chrono::high_resolution_clock::now() - start_ts;

    if (!ignore_result) {
      xe_xe_sio2_params = params;
      xe_xe_sio2_res = std::move(res);
    }

    const auto message = std::format(
        "Время моделирования: {}{}",
        std::chrono::duration_cast<std::chrono::seconds>(time),
        std::chrono::duration_cast<std::chrono::milliseconds>(time) % 1000);
    ui->statusBar->showMessage(QString::fromStdString(message));

    {
      const auto w = ui->xexeSiO2PaintWidget->width() / 4;
      const auto h = ui->xexeSiO2PaintWidget->height() / 4;

      const auto scale = std::min(w / a, h / b) / 1.01;

      const auto delta = ui->xexeSiO2DeltaDoubleSpinBox->value();
      const auto side_plasma = static_cast<int>(scale * params.r);
      const auto delta_scale = static_cast<int>(scale * delta);

      std::vector<std::size_t> without_plasma(params.n_quartz);
      std::vector<std::size_t> with_plasma(params.n_quartz);
      for (std::size_t k = 0; k < params.n_quartz; ++k) {
        QPixmap pixmap{w, h};
        QPainter painter{&pixmap};
        painter.fillRect(rect(), QBrush{Qt::white});

        for (std::size_t i = params.n_quartz; i > 0; --i) {
          if (i == k + 1) {
            painter.setPen(QPen{Qt::red, 1});
            painter.setBrush(QBrush{Qt::red});
          } else {
            painter.setPen(QPen{Qt::black, 1});
            painter.setBrush(QBrush{Qt::white});
          }
          const auto side_ai =
              static_cast<int>(scale * a * static_cast<Float>(i) /
                               static_cast<Float>(params.n_quartz));
          const auto side_bi =
              static_cast<int>(scale * b * static_cast<Float>(i) /
                               static_cast<Float>(params.n_quartz));
          painter.drawEllipse((w - side_ai) / 2, (h - side_bi) / 2, side_ai,
                              side_bi);
        }

        auto image = pixmap.toImage();
        for (int i = 0; i < w; ++i) {
          for (int j = 0; j < h; ++j) {
            if (image.pixelColor(i, j) == Qt::red) {
              without_plasma[k] += 1;
            }
          }
        }

        painter.setPen(QPen{Qt::black, 1});
        painter.setBrush(QBrush{Qt::white});
        painter.drawEllipse((w - delta_scale) / 2 - side_plasma,
                            (h - side_plasma) / 2, side_plasma, side_plasma);
        painter.drawEllipse((w + delta_scale) / 2, (h - side_plasma) / 2,
                            side_plasma, side_plasma);

        image = pixmap.toImage();
        //        if (k == params.n_quartz / 2) {
        //          ui->xexeSiO2PaintWidget->image = image;
        //          ui->xexeSiO2PaintWidget->update();
        //        }

        for (int i = 0; i < w; ++i) {
          for (int j = 0; j < h; ++j) {
            if (image.pixelColor(i, j) == Qt::red) {
              with_plasma[k] += 1;
            }
          }
        }
      }

      auto min_ratio = kOne;
      std::size_t min_ratio_index = 0;
      for (std::size_t k = 0; k < params.n_quartz; ++k) {
        const auto ratio = static_cast<Float>(with_plasma[k]) /
                           static_cast<Float>(without_plasma[k]);
        if (ratio < min_ratio) {
          min_ratio = ratio;
          min_ratio_index = k;
        }
        qWarning() << without_plasma[k] << ' ' << with_plasma[k];
      }
      for (std::size_t k = 0; k < min_ratio_index; ++k) {
        with_plasma[k] = static_cast<std::size_t>(
            static_cast<Float>(without_plasma[k]) * min_ratio);
      }

      auto minus = kZero;
      const auto step_quartz =
          params.delta / static_cast<Float>(params.n_quartz);
      const auto kLeft = 7;
      const auto kRight = 3;
      for (std::size_t i = 0; i < params.n_quartz; ++i) {
        const auto r_avg = step_quartz * (static_cast<Float>(i) + 0.5_F);
        const auto i2 = xe_xe_sio2_res->absorbed_quartz[i + 1] *
                        (2 - Sqr(static_cast<Float>(with_plasma[i]) /
                                 static_cast<Float>(without_plasma[i]))) *
                        r_avg *
                        (kLeft - (kLeft - kRight) * static_cast<Float>(i) /
                                     static_cast<Float>(params.n_quartz - 1));
        minus += xe_xe_sio2_res->absorbed_quartz[i + 1] - i2;
        xe_xe_sio2_res->absorbed_quartz[i + 1] = i2;
      }

      xe_xe_sio2_res->intensity_all -= minus;

      for (std::size_t i = 0; i < params.n_quartz; ++i) {
        const auto r_avg = step_quartz * (static_cast<Float>(i) + 0.5_F);
        xe_xe_sio2_res->absorbed_quartz3[i + 1] =
            2 * consts::kPi * xe_xe_sio2_res->absorbed_quartz[i + 1] / r_avg;
      }
    }

    ui->xexeSiO2PaintWidget->update();

    ui->xexeSiO2I2PlotWidget->setData(xe_xe_sio2_res->absorbed_plasma,
                                      xe_xe_sio2_res->absorbed_quartz);
    ui->xexeSiO2I3PlotWidget->setData(xe_xe_sio2_res->absorbed_plasma3,
                                      xe_xe_sio2_res->absorbed_quartz3);

    auto total_plasma = kZero;
    auto total_quartz = kZero;
    QString strI2;
    for (auto i2 : xe_xe_sio2_res->absorbed_plasma) {
      total_plasma += i2;
      strI2 += QString::number(i2);
      strI2 += '\n';
    }
    strI2 += "————————\n";
    for (std::size_t i = 1; i < xe_xe_sio2_res->absorbed_quartz.size(); ++i) {
      auto i2 = xe_xe_sio2_res->absorbed_quartz[i];
      total_quartz += i2;
      strI2 += QString::number(i2);
      strI2 += '\n';
    }
    ui->xexeSiO2DataI2TextEdit->setText(strI2);

    QString strI3;
    for (auto i3 : xe_xe_sio2_res->absorbed_plasma3) {
      strI3 += QString::number(i3);
      strI3 += '\n';
    }
    strI3 += "————————\n";
    for (std::size_t i = 1; i < xe_xe_sio2_res->absorbed_quartz3.size(); ++i) {
      auto i3 = xe_xe_sio2_res->absorbed_quartz3[i];
      strI3 += QString::number(i3);
      strI3 += '\n';
    }
    ui->xexeSiO2DataI3TextEdit->setText(strI3);

    ui->xexeSiO2DataTotalIntensityLineEdit->setText(
        QString::number(xe_xe_sio2_res->intensity_all));
    ui->xexeSiO2DataTotalAbsorbedPlasmaLineEdit->setText(
        QString::number(total_plasma));
    ui->xexeSiO2DataTotalAbsorbedQuartzLineEdit->setText(
        QString::number(total_quartz));
    ui->xexeSiO2DataTotalAbsorbedMirrorLineEdit->setText(
        QString::number(xe_xe_sio2_res->absorbed_mirror));
  });
}

MainWindow::~MainWindow() {
  delete ui;
}
