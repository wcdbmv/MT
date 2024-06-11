#include "xe_sio2_paint_widget.h"

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

XeSiO2PaintWidget::XeSiO2PaintWidget(QWidget* parent) : QWidget{parent} {}

void XeSiO2PaintWidget::paintEvent(QPaintEvent* /* event */) {
  QPainter painter{this};

  painter.fillRect(rect(), QBrush{Qt::white});

  auto* win = dynamic_cast<MainWindow*>(window());
  const auto scale = std::min(width(), height()) / 1.01;
  const auto side = static_cast<int>(scale);

  const auto r = win->ui->xeSiO2RDoubleSpinBox->value();
  const auto delta = win->ui->xeSiO2DeltaDoubleSpinBox->value();
  const auto r1 = r + delta;
    const auto n_plasma =
        static_cast<std::size_t>(win->ui->xeSiO2NPlasmaSpinBox->value());
  const auto n_quartz =
      static_cast<std::size_t>(win->ui->xeSiO2NQuartzSpinBox->value());

    Float min = 9999999999.0_F;
    Float max = -min;
    if (win->xe_sio2_res.has_value()) {
      assert(win->xe_sio2_res->absorbed_plasma.size() == n_plasma);
      assert(win->xe_sio2_res->absorbed_plasma3.size() == n_plasma);
      assert(win->xe_sio2_res->absorbed_quartz.size() == n_quartz + 1);
      assert(win->xe_sio2_res->absorbed_quartz3.size() == n_quartz + 1);
      for (auto i3 : win->xe_sio2_res->absorbed_plasma3) {
        min = std::min(i3, min);
        max = std::max(i3, max);
      }
      for (std::size_t i = 1; i < win->xe_sio2_res->absorbed_quartz3.size(); ++i) {
        const auto i3 = win->xe_sio2_res->absorbed_quartz3[i];
        min = std::min(i3, min);
        max = std::max(i3, max);
      }
    }

  painter.setPen(QPen{Qt::black, 4});
  if (win->xe_sio2_res.has_value()) {
      painter.setBrush(QBrush{QColor{
          255,
          static_cast<int>(
              255 * (1.0 - (win->xe_sio2_res->absorbed_quartz3.back() - min) /
                               (max - min))),
          0}});
  }

  painter.drawEllipse((width() - side) / 2, (height() - side) / 2, side, side);

  painter.setPen(QPen{Qt::black, 1});
  for (std::size_t i = n_quartz - 1; i > 0; --i) {
        QBrush brush{};
        if (win->xe_sio2_res.has_value()) {
          brush = QBrush{QColor{
              255,
              static_cast<int>(
                  255 * (1.0 - ((win->xe_sio2_res->absorbed_quartz3[i] - min) /
                                (max - min)))),
              0}};
        }
        painter.setBrush(brush);

    const auto side_i = static_cast<int>(static_cast<Float>(
        side * (r / r1 + static_cast<Float>(i) * delta /
                             (static_cast<Float>(n_quartz) * r1))));
    painter.drawEllipse((width() - side_i) / 2, (height() - side_i) / 2, side_i,
                        side_i);
  }

  painter.setPen(QPen{Qt::black, 4});
  if (win->xe_sio2_res.has_value()) {
    painter.setBrush(QBrush{QColor{
        255,
        static_cast<int>(
            255 * (1.0 - (win->xe_sio2_res->absorbed_plasma3.back() - min) /
                             (max - min))),
        0}});
  }
  const auto side_plasma = static_cast<int>(side * r / r1);
  painter.drawEllipse((width() - side_plasma) / 2, (height() - side_plasma) / 2,
                      side_plasma, side_plasma);

  painter.setPen(QPen{Qt::black, 1});
  for (std::size_t i = n_plasma - 1; i > 0; --i) {
    QBrush brush{};
    if (win->xe_sio2_res.has_value()) {
          brush = QBrush{QColor{
              255,
              static_cast<int>(
                  255 * (1.0 - ((win->xe_sio2_res->absorbed_plasma3[i - 1] - min) /
                                (max - min)))),
              0}};
    }
    painter.setBrush(brush);

    const auto side_i = static_cast<int>(
        static_cast<Float>(side_plasma * (static_cast<Float>(i) /
                                          (static_cast<Float>(n_plasma)))));
    painter.drawEllipse((width() - side_i) / 2, (height() - side_i) / 2, side_i,
                        side_i);
  }
}
