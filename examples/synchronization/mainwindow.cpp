#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  srand(QDateTime::currentDateTime().toTime_t());
  ui->setupUi(this);
  
  ui->customPlot->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  ui->customPlot->setRangeZoom(Qt::Horizontal|Qt::Vertical);
  ui->customPlot->setupFullAxesBox();
  
  ui->customPlot2->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  ui->customPlot2->setRangeZoom(Qt::Horizontal|Qt::Vertical);
  ui->customPlot2->setupFullAxesBox();
  
  // setup the signal/slot connections that realize the bi-directional synchronization:
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->xAxis, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->yAxis, SLOT(setRange(QCPRange)));
  connect(ui->customPlot2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis, SLOT(setRange(QCPRange)));
  connect(ui->customPlot2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis, SLOT(setRange(QCPRange)));
  // make the customPlots replot synchronously (don't worry, won't cause infinite replot-recursion):
  connect(ui->customPlot, SIGNAL(afterReplot()), ui->customPlot2, SLOT(replot()));
  connect(ui->customPlot2, SIGNAL(afterReplot()), ui->customPlot, SLOT(replot()));
  
  addRandomGraph(ui->customPlot);
  addRandomGraph(ui->customPlot);
  addRandomGraph(ui->customPlot);
  addRandomGraph(ui->customPlot2);
  addRandomGraph(ui->customPlot2);
  addRandomGraph(ui->customPlot2);
  
  // make bottom and left axes transfer their ranges to top and right axes:
  connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->customPlot2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->yAxis2, SLOT(setRange(QCPRange)));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::addRandomGraph(QCustomPlot *plot)
{
  int n = 100;
  double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
  double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
  double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
  double yOffset = (rand()/(double)RAND_MAX - 0.5)*5;
  double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
  double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
  QVector<double> x(n), y(n);
  for (int i=0; i<n; i++)
  {
    x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
    y[i] = (sin(x[i]*r1*5)*sin(cos(x[i]*r2)*r4*3)+r3*cos(sin(x[i])*r4*2))*yScale + yOffset;
  }
  
  plot->addGraph();
  plot->graph()->setName(QString("New graph %1").arg(plot->graphCount()-1));
  plot->graph()->setData(x, y);
  plot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
  if (rand()%100 > 75)
    plot->graph()->setScatterStyle((QCP::ScatterStyle)(rand()%9+1));
  QPen graphPen;
  graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
  graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
  plot->graph()->setPen(graphPen);
}
