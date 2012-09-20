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
  
  // tests:
  void setupItemAnchorTest(QCustomPlot *customPlot);
  void setupItemTracerTest(QCustomPlot *customPlot);
  void setupGraphTest(QCustomPlot *customPlot);
  void setupExportTest(QCustomPlot *customPlot);
  void setupLogErrorsTest(QCustomPlot *customPlot);
  void setupSelectTest(QCustomPlot *customPlot);
  void setupDateTest(QCustomPlot *customPlot);
  void setupTickLabelTest(QCustomPlot *customPlot);
  void setupDaqPerformance(QCustomPlot *customPlot);
  
  // presets:
  void presetInteractive(QCustomPlot *customPlot);
  
  // helpers:
  void labelItemAnchors(QCPAbstractItem *item, double fontSize=8, bool circle=true, bool labelBelow=true);
  void showSelectTestColorMap(QCustomPlot *customPlot);
  
  // testbed:
  void setupTestbed(QCustomPlot *customPlot);
  
  // special use cases test:
  void setupIntegerTickStepCase(QCustomPlot *customPlot);
  
public slots:
  void tracerTestMouseMove(QMouseEvent *event);
  void selectTestColorMapRefresh();
  void mouseMoveRotateTickLabels(QMouseEvent *event);
  void tickLabelTestTimerSlot();
  
  void daqPerformanceDataSlot();
  void daqPerformanceReplotSlot();
  
  void integerTickStepCase_xRangeChanged(QCPRange newRange);
  void integerTickStepCase_yRangeChanged(QCPRange newRange);
  
private:
  Ui::MainWindow *ui;
  QCustomPlot *mCustomPlot;
  QCPItemTracer *tracerTestTracer;
  QTimer mReplotTimer;
  QTimer mDataTimer;
};

#endif // MAINWINDOW_H
