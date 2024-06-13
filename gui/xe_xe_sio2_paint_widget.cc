#include "xe_xe_sio2_paint_widget.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <vector>

#include <QColor>
#include <QGradient>
#include <QPainter>
#include <QPen>
#include <QSpinBox>
#include <Qt>

#include "./ui_main_window.h"
#include "main_window.h"

#include "base/config/float.h"

XeXeSiO2PaintWidget::XeXeSiO2PaintWidget(QWidget* parent) : QWidget{parent} {}

void XeXeSiO2PaintWidget::paintEvent(QPaintEvent* /* event */) {
  QPainter painter{this};

  painter.fillRect(rect(), QBrush{Qt::white});

  if (!image.isNull()) {
    painter.drawImage(rect(), image);
    return;
  }

  auto* win = dynamic_cast<MainWindow*>(window());
  //  const auto scale = std::min(width(), height()) / 1.01;
  //  const auto side = static_cast<int>(scale);

  const auto a = win->ui->xexeSiO2ADoubleSpinBox->value();
  const auto b = win->ui->xexeSiO2BDoubleSpinBox->value();

  const auto scale = std::min(width() / a, height() / b) / 1.01;
  const auto side_a = static_cast<int>(scale * a);
  const auto side_b = static_cast<int>(scale * b);

  const auto r = win->ui->xexeSiO2RDoubleSpinBox->value();
  const auto delta = win->ui->xexeSiO2DeltaDoubleSpinBox->value();
  const auto n_plasma =
      static_cast<std::size_t>(win->ui->xexeSiO2NPlasmaSpinBox->value());
  const auto n_quartz =
      static_cast<std::size_t>(win->ui->xexeSiO2NQuartzSpinBox->value());

  Float min = 9999999999.0_F;
  Float max = -min;
  if (win->xe_xe_sio2_res.has_value()) {
    assert(win->xe_xe_sio2_res->absorbed_plasma.size() == n_plasma);
    assert(win->xe_xe_sio2_res->absorbed_plasma3.size() == n_plasma);
    assert(win->xe_xe_sio2_res->absorbed_quartz.size() == n_quartz + 1);
    assert(win->xe_xe_sio2_res->absorbed_quartz3.size() == n_quartz + 1);
    for (auto i3 : win->xe_xe_sio2_res->absorbed_plasma3) {
      min = std::min(i3, min);
      max = std::max(i3, max);
    }
    for (std::size_t i = 1; i < win->xe_xe_sio2_res->absorbed_quartz3.size();
         ++i) {
      const auto i3 = win->xe_xe_sio2_res->absorbed_quartz3[i];
      min = std::min(i3, min);
      max = std::max(i3, max);
    }
  }

  painter.setPen(QPen{Qt::black, 4});
  if (win->xe_xe_sio2_res.has_value()) {
    painter.setBrush(QBrush{QColor{
        255,
        static_cast<int>(
            255 * (1.0 - (win->xe_xe_sio2_res->absorbed_quartz3.back() - min) /
                             (max - min))),
        0}});
  }

  painter.drawEllipse((width() - side_a) / 2, (height() - side_b) / 2, side_a,
                      side_b);

  painter.setPen(QPen{Qt::black, 1});
  for (std::size_t i = n_quartz - 1; i > 0; --i) {
    QBrush brush{};
    if (win->xe_xe_sio2_res.has_value()) {
      brush = QBrush{QColor{
          255,
          static_cast<int>(
              255 * (1.0 - ((win->xe_xe_sio2_res->absorbed_quartz3[i] - min) /
                            (max - min)))),
          0}};
    }
    painter.setBrush(brush);

    const auto side_ai = static_cast<int>(scale * a * static_cast<Float>(i) /
                                          static_cast<Float>(n_quartz));
    const auto side_bi = static_cast<int>(scale * b * static_cast<Float>(i) /
                                          static_cast<Float>(n_quartz));
    painter.drawEllipse((width() - side_ai) / 2, (height() - side_bi) / 2,
                        side_ai, side_bi);
  }

  painter.setPen(QPen{Qt::black, 4});
  painter.setBrush(QBrush{Qt::white});
  if (win->xe_xe_sio2_res.has_value()) {
    painter.setBrush(QBrush{QColor{
        255,
        static_cast<int>(
            255 * (1.0 - (win->xe_xe_sio2_res->absorbed_plasma3.back() - min) /
                             (max - min))),
        0}});
  }
  const auto side_plasma = static_cast<int>(scale * r);
  const auto delta_scale = static_cast<int>(scale * delta);
  painter.drawEllipse((width() - delta_scale) / 2 - side_plasma,
                      (height() - side_plasma) / 2, side_plasma, side_plasma);
  painter.drawEllipse((width() + delta_scale) / 2, (height() - side_plasma) / 2,
                      side_plasma, side_plasma);

  painter.setPen(QPen{Qt::black, 1});
  for (std::size_t i = n_plasma - 1; i > 0; --i) {
    QBrush brush{};
    if (win->xe_xe_sio2_res.has_value()) {
      brush = QBrush{QColor{
          255,
          static_cast<int>(
              255 *
              (1.0 - ((win->xe_xe_sio2_res->absorbed_plasma3[i - 1] - min) /
                      (max - min)))),
          0}};
    }
    painter.setBrush(brush);

    const auto side_i = static_cast<int>(scale * r * static_cast<Float>(i) /
                                         static_cast<Float>(n_plasma));
    painter.drawEllipse((width() - delta_scale - side_plasma - side_i) / 2,
                        (height() - side_i) / 2, side_i, side_i);
    painter.drawEllipse((width() + delta_scale + side_plasma - side_i) / 2,
                        (height() - side_i) / 2, side_i, side_i);
  }
}
