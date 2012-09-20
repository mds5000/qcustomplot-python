#include <QtGui>
#include <QtCore>
#include <QtTest/QtTest>

#include <../../qcustomplot.h>
#include <../../qcustomplot.cpp>

class Benchmark : public QObject
{
  Q_OBJECT
private slots:
  void init();
  void cleanup();
  
  void QCPGraph_Standard();
  void QCPGraph_ManyPoints();
  void QCPGraph_ManyLines();
  
  void QCPAxis_TickLabels();
  void QCPAxis_TickLabelsCached();
  
private:
  QCustomPlot *mPlot;
};

QTEST_MAIN(Benchmark)
#include "benchmark.moc"

////////////////////////////////////////////////////////////////
////// Benchmark Implementation
////////////////////////////////////////////////////////////////


void Benchmark::init()
{
  mPlot = new QCustomPlot(0);
  mPlot->setGeometry(0, 0, 640, 360);
}

void Benchmark::cleanup()
{
  delete mPlot;
}

void Benchmark::QCPGraph_Standard()
{
  QCPGraph *graph1 = mPlot->addGraph();
  QCPGraph *graph2 = mPlot->addGraph();
  QCPGraph *graph3 = mPlot->addGraph();
  graph1->setBrush(QBrush(QColor(100, 0, 0, 100)));
  int n = 500;
  QVector<double> x(n), y1(n), y2(n), y3(n);
  for (int i=0; i<n; ++i)
  {
    x[i] = i/(double)n;
    y1[i] = qSin(x[i]*10*M_PI);
    y2[i] = qCos(x[i]*40*M_PI);
    y3[i] = x[i];
  }
  graph1->setData(x, y1);
  graph2->setData(x, y2);
  graph3->setData(x, y3);
  mPlot->rescaleAxes();
  mPlot->xAxis->scaleRange(0.7, mPlot->xAxis->range().center());
  
  QBENCHMARK
  {
    mPlot->replot();
  }
 
}

void Benchmark::QCPGraph_ManyPoints()
{
  QCPGraph *graph1 = mPlot->addGraph();
  QCPGraph *graph2 = mPlot->addGraph();
  QCPGraph *graph3 = mPlot->addGraph();
  graph1->setBrush(QBrush(QColor(100, 0, 0, 100)));
  graph1->setScatterStyle(QCP::ssCross);
  graph2->setScatterStyle(QCP::ssCircle);
  graph3->setScatterStyle(QCP::ssDiamond);
  graph1->setLineStyle(QCPGraph::lsNone);
  graph2->setLineStyle(QCPGraph::lsNone);
  graph3->setLineStyle(QCPGraph::lsNone);
  int n = 50000;
  QVector<double> x(n), y1(n), y2(n), y3(n);
  for (int i=0; i<n; ++i)
  {
    x[i] = i/(double)n;
    y1[i] = qSin(x[i]*10*M_PI);
    y2[i] = qCos(x[i]*40*M_PI);
    y3[i] = x[i];
  }
  graph1->setData(x, y1);
  graph2->setData(x, y2);
  graph3->setData(x, y3);
  mPlot->rescaleAxes();
  mPlot->xAxis->scaleRange(0.7, mPlot->xAxis->range().center());
  
  QBENCHMARK
  {
    mPlot->replot();
  }
}

void Benchmark::QCPGraph_ManyLines()
{
  QCPGraph *graph1 = mPlot->addGraph();
  QCPGraph *graph2 = mPlot->addGraph();
  QCPGraph *graph3 = mPlot->addGraph();
  graph1->setBrush(QBrush(QColor(100, 0, 0, 100)));
  graph1->setScatterStyle(QCP::ssNone);
  graph2->setScatterStyle(QCP::ssNone);
  graph3->setScatterStyle(QCP::ssNone);
  graph1->setLineStyle(QCPGraph::lsLine);
  graph2->setLineStyle(QCPGraph::lsLine);
  graph3->setLineStyle(QCPGraph::lsLine);
  int n = 50000;
  QVector<double> x(n), y1(n), y2(n), y3(n);
  for (int i=0; i<n; ++i)
  {
    x[i] = i/(double)n;
    y1[i] = qSin(x[i]*10*M_PI);
    y2[i] = qCos(x[i]*40*M_PI);
    y3[i] = x[i];
  }
  graph1->setData(x, y1);
  graph2->setData(x, y2);
  graph3->setData(x, y3);
  mPlot->rescaleAxes();
  mPlot->xAxis->scaleRange(0.7, mPlot->xAxis->range().center());
  
  QBENCHMARK
  {
    mPlot->replot();
  }
}

void Benchmark::QCPAxis_TickLabels()
{
  mPlot->setPlottingHint(QCP::phCacheLabels, false);
  mPlot->setupFullAxesBox();
  mPlot->xAxis2->setTickLabels(true);
  mPlot->yAxis2->setTickLabels(true);
  mPlot->xAxis->setRange(-10, 10);
  mPlot->yAxis->setRange(0.001, 0.002);
  mPlot->xAxis2->setRange(1e6, 1e8);
  mPlot->yAxis2->setRange(-1e100, 1e100);
  QBENCHMARK
  {
    mPlot->replot();
  }
}

void Benchmark::QCPAxis_TickLabelsCached()
{
  mPlot->setPlottingHint(QCP::phCacheLabels, true);
  mPlot->setupFullAxesBox();
  mPlot->xAxis2->setTickLabels(true);
  mPlot->yAxis2->setTickLabels(true);
  mPlot->xAxis->setRange(-10, 10);
  mPlot->yAxis->setRange(0.001, 0.002);
  mPlot->xAxis2->setRange(1e6, 1e8);
  mPlot->yAxis2->setRange(-1e100, 1e100);
  QBENCHMARK
  {
    mPlot->replot();
  }
}
