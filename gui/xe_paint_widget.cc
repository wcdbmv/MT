#include "xe_paint_widget.h"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <vector>

#include <QColor>
#include <QDoubleSpinBox>
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

  Float min = 9999999999.0_F;
  Float max = -min;
  if (win->xe_res.has_value()) {
    for (std::size_t i = 0; i < win->xe_res->absorbed_plasma.size(); ++i) {
      const auto i3 =
          win->xe_res->absorbed_plasma[i] /
          (win->ui->xeRDoubleSpinBox->value() / win->ui->xeNSpinBox->value() *
           (static_cast<Float>(i) + 0.5_F));
      min = std::min(i3, min);
      max = std::max(i3, max);
    }
  }

  painter.setPen(QPen{Qt::black, 4});
  if (win->xe_res.has_value()) {
    const auto i3 =
        win->xe_res->absorbed_plasma.back() /
        (win->ui->xeRDoubleSpinBox->value() / win->ui->xeNSpinBox->value() *
         (static_cast<Float>(win->xe_res->absorbed_plasma.size()) - 0.5_F));
    painter.setBrush(QBrush{QColor{
        255, static_cast<int>(255 * (1.0 - (i3 - min) / (max - min))), 0}});
  }

  painter.drawEllipse((width() - side) / 2, (height() - side) / 2, side, side);

  painter.setPen(QPen{Qt::black, 1});
  const auto n = win->ui->xeNSpinBox->value();
  for (int i = n - 1; i > 0; --i) {
    QBrush brush{};
    if (win->xe_res.has_value()) {
      const auto i3 =
          win->xe_res->absorbed_plasma[static_cast<std::size_t>(i) - 1] /
          (win->ui->xeRDoubleSpinBox->value() / win->ui->xeNSpinBox->value() *
           (static_cast<Float>(i) - 0.5_F));
      brush = QBrush{QColor{
          255, static_cast<int>(255 * (1.0 - (i3 - min) / (max - min))), 0}};
    }
    painter.setBrush(brush);

    const auto side_i = side * i / n;
    painter.drawEllipse((width() - side_i) / 2, (height() - side_i) / 2, side_i,
                        side_i);
  }
}
