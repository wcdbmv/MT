#include "xe_paint_widget.h"

#include <QPainter>

#include "main_window.h"
#include "./ui_main_window.h"

XePaintWidget::XePaintWidget(QWidget* parent) : QWidget{parent} {}

void XePaintWidget::paintEvent(QPaintEvent* event) {
  QPainter painter{this};

  painter.fillRect(rect(), QBrush{Qt::white});

  auto* win = static_cast<MainWindow*>(window());
  const auto scale = std::min(width(), height()) / (1.01 * win->ui->xeRDoubleSpinBox->maximum());
  const auto side = win->ui->xeRDoubleSpinBox->value() * scale;

  painter.setPen(QPen{Qt::black, 4});
  painter.drawEllipse((width() - side) / 2, (height() - side) / 2, side, side);

  painter.setPen(QPen{Qt::black, 1});
  const auto n = win->ui->xeNSpinBox->value();
  for (int i = 1; i < n; ++i) {
    const auto side_i = side * i / n;
    painter.drawEllipse((width() - side_i) / 2, (height() - side_i) / 2, side_i, side_i);
  }
}
