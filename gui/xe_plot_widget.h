#pragma once

#include <QChartView>
#include <QObject>
#include <QString>

#include <vector>

#include "base/config/float.h"

class QChart;
class QLineSeries;
class QValueAxis;
class QWidget;

class XePlotWidget : public QChartView {
  Q_OBJECT
 public:
  explicit XePlotWidget(QWidget* parent = nullptr);

  void setAxisY(const QString& title);
  void setData(const std::vector<Float>& y);

 private:
  QChart* chart_;
  QValueAxis* axis_x_;
  QValueAxis* axis_y_;
  QLineSeries* series_;
};
