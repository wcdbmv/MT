#include "xe_plot_widget.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>

#include <QChart>
#include <QLegend>
#include <QLineSeries>
#include <QPointF>
#include <QValueAxis>
#include <QXYSeries>
#include <Qt>

XePlotWidget::XePlotWidget(QWidget* parent)
    : QChartView{parent},
      chart_{new QChart},
      axis_x_{new QValueAxis},
      series_{new QLineSeries} {
  chart_->addSeries(series_);
  chart_->legend()->hide();

  axis_x_->setTitleText("z = r/R");
  axis_x_->setLabelFormat("%g");
  chart_->addAxis(axis_x_, Qt::AlignBottom);
  series_->attachAxis(axis_x_);

  setChart(chart_);
}

void XePlotWidget::setAxisY(const QString& title) {
  assert(!axis_y_);

  axis_y_ = new QValueAxis;
  axis_y_->setTitleText(title);
  axis_y_->setLabelFormat("%g");
  chart_->addAxis(axis_y_, Qt::AlignLeft);
  series_->attachAxis(axis_y_);
}

void XePlotWidget::setData(const std::vector<Float>& y) {
  assert(axis_y_);

  const auto xt = width() / 100;
  const auto yt = height() / 50;
  axis_x_->setTickCount(xt + xt & 1);
  axis_y_->setTickCount(yt + yt & 1);

  const auto [min_it, max_it] = std::minmax_element(y.begin(), y.end());

  auto min = *min_it;
  auto scale = kOne;
  while (min >= 10) {
    min /= 10;
    scale *= 10;
  }
  while (min <= 0) {
    min *= 10;
    scale /= 10;
  }
  axis_y_->setMin(std::floor(min * 100) / 100 * scale);

  auto max = *max_it * 1.01_F;
  scale = kOne;
  while (max >= 10) {
    max /= 10;
    scale *= 10;
  }
  while (max < 1) {
    max *= 10;
    scale /= 10;
  }
  axis_y_->setMax(std::ceil(max * 100) / 100 * scale);

  series_->clear();

  const auto step = kOne / static_cast<Float>(y.size());

  for (std::size_t i = 0; i < y.size(); ++i) {
    *series_ << QPointF{step * static_cast<Float>(i + 1), y[i]};
  }
}
