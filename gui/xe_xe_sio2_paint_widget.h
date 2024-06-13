#pragma once

#include <QObject>
#include <QWidget>

class QPaintEvent;

class XeXeSiO2PaintWidget : public QWidget {
  Q_OBJECT
 public:
  explicit XeXeSiO2PaintWidget(QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
};
