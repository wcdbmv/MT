#include "xe_sio2_plot_widget.h"

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

XeSiO2PlotWidget::XeSiO2PlotWidget(QWidget* parent)
    : QChartView{parent},
      chart_{new QChart},
      axis_x_{new QValueAxis},
      series_{new QLineSeries},
      vr_{new QLineSeries} {
  chart_->addSeries(series_);
  chart_->addSeries(vr_);
  chart_->legend()->hide();

  axis_x_->setTitleText("z = r/R");
  axis_x_->setLabelFormat("%g");
  chart_->addAxis(axis_x_, Qt::AlignBottom);
  series_->attachAxis(axis_x_);
  vr_->attachAxis(axis_x_);

  setChart(chart_);
}

void XeSiO2PlotWidget::setAxisY(const QString& title) {
  assert(!axis_y_);

  axis_y_ = new QValueAxis;
  axis_y_->setTitleText(title);
  axis_y_->setLabelFormat("%g");
  chart_->addAxis(axis_y_, Qt::AlignLeft);
  series_->attachAxis(axis_y_);
  vr_->attachAxis(axis_y_);
}

void XeSiO2PlotWidget::setData(const std::vector<Float>& plasma,
                               const std::vector<Float>& quartz,
                               Float r,
                               Float delta) {
  assert(axis_y_);

  const auto xt = width() / 100;
  const auto yt = height() / 50;
  axis_x_->setTickCount(xt + xt & 1);
  axis_y_->setTickCount(yt + yt & 1);

  axis_x_->setMax(std::ceil((1 + delta / r) * 100) / 100);

  const auto [min_it_plasma, max_it_plasma] =
      std::minmax_element(plasma.begin(), plasma.end());
  auto min = *min_it_plasma;
  auto max = *max_it_plasma * 1.01_F;
  const auto [min_it_quartz, max_it_quartz] =
      std::minmax_element(quartz.begin() + 1, quartz.end());
  min = std::min(*min_it_quartz, min);
  max = std::max(*max_it_quartz * 1.01_F, max);

  assert(min >= 0);
  assert(max >= 0);

  auto scale = kOne;
  while (min >= 10) {
    min /= 10;
    scale *= 10;
  }
  while (min < 0) {
    min *= 10;
    scale /= 10;
  }
  axis_y_->setMin(std::floor(min * 100) / 100 * scale);

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

  auto step = kOne / static_cast<Float>(plasma.size());
  for (std::size_t i = 0; i < plasma.size(); ++i) {
    *series_ << QPointF{step * static_cast<Float>(i + 1), plasma[i]};
  }

  step = delta / (r * static_cast<Float>(quartz.size() - 1));
  for (std::size_t i = 1; i < quartz.size(); ++i) {
    *series_ << QPointF{1 + step * static_cast<Float>(i - 1), quartz[i]};
  }

  auto last_y = 2 * quartz.back() - quartz[quartz.size() - 2];
  if (last_y < 0) {
    last_y = 0.95 * quartz.back();
  }
  *series_ << QPointF{1 + delta / r, last_y};

  vr_->clear();
  *vr_ << QPointF{1, 0};
  *vr_ << QPointF{1, max};
}
