#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include "../../src/qcp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  // image generators:
  void genScatterStyles();
  void genItemPixmap();
  void genItemRect();
  void genItemEllipse();
  void genItemLine();
  void genItemStraightLIne();
  void genItemCurve();
  void genItemBracket();
  void genItemText();
  void genItemTracer();
  void genLineEnding();
  
  // helpers:
  void labelItemAnchors(QCPAbstractItem *item, double fontSize=8, bool circle=true, bool labelBelow=true);
  void resetPlot();
private:
  Ui::MainWindow *ui;
  QCustomPlot *customPlot;
  QDir dir; // target directory for images
  QBrush defaultBrush;
};

#endif // MAINWINDOW_H
