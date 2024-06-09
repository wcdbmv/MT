#include "xe_paint_widget.h"

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

XePaintWidget::XePaintWidget(QWidget* parent) : QWidget{parent} {}

void XePaintWidget::paintEvent(QPaintEvent* /* event */) {
  QPainter painter{this};

  painter.fillRect(rect(), QBrush{Qt::white});

  auto* win = dynamic_cast<MainWindow*>(window());
  const auto scale = std::min(width(), height()) / 1.01;
  const auto side = static_cast<int>(scale);
  const auto n = static_cast<std::size_t>(win->ui->xeNSpinBox->value());

  Float min = 9999999999.0_F;
  Float max = -min;
  if (win->xe_res.has_value()) {
    assert(win->xe_res->absorbed_plasma.size() == n);
    assert(win->xe_res->absorbed_plasma3.size() == n);
    for (auto i3 : win->xe_res->absorbed_plasma3) {
      min = std::min(i3, min);
      max = std::max(i3, max);
    }
  }

  painter.setPen(QPen{Qt::black, 4});
  if (win->xe_res.has_value()) {
    painter.setBrush(QBrush{QColor{
        255,
        static_cast<int>(
            255 *
            (1.0 - (win->xe_res->absorbed_plasma3.back() - min) / (max - min))),
        0}});
  }

  painter.drawEllipse((width() - side) / 2, (height() - side) / 2, side, side);

  painter.setPen(QPen{Qt::black, 1});
  for (std::size_t i = n - 1; i > 0; --i) {
    QBrush brush{};
    if (win->xe_res.has_value()) {
      brush = QBrush{QColor{
          255,
          static_cast<int>(
              255 * (1.0 - ((win->xe_res->absorbed_plasma3[i - 1] - min) /
                            (max - min)))),
          0}};
    }
    painter.setBrush(brush);

    const auto side_i = side * static_cast<int>(i) / static_cast<int>(n);
    painter.drawEllipse((width() - side_i) / 2, (height() - side_i) / 2, side_i,
                        side_i);
  }
}
