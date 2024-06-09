#pragma once

#include <QObject>
#include <QWidget>

class XePaintWidget : public QWidget {
  Q_OBJECT
 public:
  explicit XePaintWidget(QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
};
