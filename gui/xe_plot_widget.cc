#include "xe_plot_widget.h"

#include <QPainter>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

XePlotWidget::XePlotWidget(QWidget* parent) : QWidget{parent} {}

void XePlotWidget::paintEvent(QPaintEvent* event) {
  QPainter painter{this};

  painter.fillRect(rect(), QBrush{Qt::white});

  auto* win = static_cast<MainWindow*>(window());
  const auto scale = std::min(width(), height()) / (1.01 * win->ui->xeRDoubleSpinBox->maximum());
  const auto side = win->ui->xeRDoubleSpinBox->value() * scale;

  painter.setPen(QPen{Qt::black, 4});
  painter.drawEllipse((width() - side) / 2, (height() - side) / 2, side, side);
}
