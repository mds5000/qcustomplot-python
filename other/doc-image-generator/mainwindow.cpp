#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  defaultBrush(QColor(80, 215, 10, 70))
{
  ui->setupUi(this);
  setGeometry(300, 300, 500, 500);
  
  dir.setPath("./");
  dir.mkdir("images");
  if (!dir.cd("images"))
  {
    QMessageBox::critical(this, "Error", tr("Couldn't create and access image directory:\n%1").arg(dir.filePath("images")));
  } else
  {
    genItemPixmap();      
    genItemRect();        
    genItemEllipse();     
    genItemLine();        
    genItemStraightLIne();
    genItemCurve();       
    genItemBracket();     
    genItemText();        
    genItemTracer();      
    genLineEnding(); 
    genScatterStyles();
  }
  QTimer::singleShot(0, qApp, SLOT(quit()));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::genScatterStyles()
{
  resetPlot();
  // Different scatter styles in separate images:
  customPlot->xAxis->setRange(-1, 1);
  customPlot->yAxis->setRange(-1, 1);
  customPlot->addGraph();
  customPlot->graph()->addData(0, 0);
  customPlot->graph()->setScatterSize(8.5);
  customPlot->graph()->setPen(QPen(Qt::black));
  customPlot->setColor(Qt::transparent);
  QMap<QCP::ScatterStyle, QString> scatterStyles;
  scatterStyles.insert(QCP::ssDot, "ssDot");
  scatterStyles.insert(QCP::ssCross, "ssCross");
  scatterStyles.insert(QCP::ssPlus, "ssPlus");
  scatterStyles.insert(QCP::ssCircle, "ssCircle");
  scatterStyles.insert(QCP::ssDisc, "ssDisc");
  scatterStyles.insert(QCP::ssSquare, "ssSquare");
  scatterStyles.insert(QCP::ssDiamond, "ssDiamond");
  scatterStyles.insert(QCP::ssStar, "ssStar");
  scatterStyles.insert(QCP::ssTriangle, "ssTriangle");
  scatterStyles.insert(QCP::ssTriangleInverted, "ssTriangleInverted");
  scatterStyles.insert(QCP::ssCrossSquare, "ssCrossSquare");
  scatterStyles.insert(QCP::ssPlusSquare, "ssPlusSquare");
  scatterStyles.insert(QCP::ssCrossCircle, "ssCrossCircle");
  scatterStyles.insert(QCP::ssPlusCircle, "ssPlusCircle");
  scatterStyles.insert(QCP::ssPeace, "ssPeace");
  QMapIterator<QCP::ScatterStyle, QString> scatterIt(scatterStyles);
  while (scatterIt.hasNext())
  {
    scatterIt.next();
    customPlot->graph()->setScatterStyle(scatterIt.key());
    QPixmap pm = customPlot->pixmap(16, 16);
    pm.save(dir.filePath(scatterIt.value()+".png"));
  }
}

void MainWindow::genItemPixmap()
{
  resetPlot();
  QCPItemPixmap *pixmapItem = new QCPItemPixmap(customPlot);
  customPlot->addItem(pixmapItem);
  pixmapItem->setPixmap(QPixmap("./gnu.png"));
  pixmapItem->setScaled(true, Qt::IgnoreAspectRatio);
  pixmapItem->topLeft->setCoords(-0.2, 1);
  pixmapItem->bottomRight->setCoords(0.3, 0);
  labelItemAnchors(pixmapItem);
  QCPItemPixmap *pixmapItem2 = new QCPItemPixmap(customPlot);
  customPlot->addItem(pixmapItem2);
  pixmapItem2->setPixmap(QPixmap("./gnu.png"));
  pixmapItem2->setScaled(true, Qt::IgnoreAspectRatio);
  pixmapItem2->topLeft->setCoords(1.2, 0);
  pixmapItem2->bottomRight->setCoords(0.7, 1);
  labelItemAnchors(pixmapItem2);
  customPlot->savePng(dir.filePath("QCPItemPixmap.png"), 460, 160);
}

void MainWindow::genItemRect()
{
  resetPlot();
  QCPItemRect *rect = new QCPItemRect(customPlot);
  customPlot->addItem(rect);
  rect->setBrush(defaultBrush);
  rect->topLeft->setCoords(0, 1);
  rect->bottomRight->setCoords(1, 0);
  labelItemAnchors(rect);
  customPlot->savePng(dir.filePath("QCPItemRect.png"), 230, 160);
}

void MainWindow::genItemEllipse()
{
  resetPlot();
  QCPItemEllipse *ellipse = new QCPItemEllipse(customPlot);
  customPlot->addItem(ellipse);
  ellipse->setBrush(defaultBrush);
  ellipse->topLeft->setCoords(-0.15, 1.1);
  ellipse->bottomRight->setCoords(1.15, 0);
  labelItemAnchors(ellipse);
  customPlot->savePng(dir.filePath("QCPItemEllipse.png"), 300, 200);
}

void MainWindow::genItemLine()
{
  resetPlot();
  QCPItemLine *line = new QCPItemLine(customPlot);
  customPlot->addItem(line);
  line->start->setCoords(-0.1, 0.8);
  line->end->setCoords(1.1, 0.2);
  line->setHead(QCPLineEnding::esSpikeArrow);
  labelItemAnchors(line);
  customPlot->savePng(dir.filePath("QCPItemLine.png"), 230, 160);
}

void MainWindow::genItemStraightLIne()
{
  resetPlot();
  QCPItemStraightLine *straightLine = new QCPItemStraightLine(customPlot);
  customPlot->addItem(straightLine);
  straightLine->point1->setCoords(0, 0.4);
  straightLine->point2->setCoords(1, 0.6);
  labelItemAnchors(straightLine);
  customPlot->savePng(dir.filePath("QCPItemStraightLine.png"), 230, 160);
}

void MainWindow::genItemCurve()
{
  resetPlot();
  QCPItemCurve *curve = new QCPItemCurve(customPlot);
  customPlot->addItem(curve);
  curve->start->setCoords(0, 1);
  curve->startDir->setCoords(0.5, 1);
  curve->endDir->setCoords(0.7, 0.2);
  curve->end->setCoords(1, 0);
  curve->setHead(QCPLineEnding::esSpikeArrow);
  curve->setTail(QCPLineEnding::esLineArrow);
  labelItemAnchors(curve);
  customPlot->savePng(dir.filePath("QCPItemCurve.png"), 230, 160);
}

void MainWindow::genItemBracket()
{
  resetPlot();
  QCPItemBracket *bracket = new QCPItemBracket(customPlot);
  customPlot->addItem(bracket);
  bracket->left->setCoords(-0.2, 0.35);
  bracket->right->setCoords(1.2, 0.65);
  bracket->setLength(12);
  labelItemAnchors(bracket, 8, true, false);
  customPlot->savePng(dir.filePath("QCPItemBracket.png"), 230, 160);
  customPlot->clearItems();
  
  // generate image of multiple bracket lengths/styles:
  for (int i=0; i<4; ++i)
  {
    QCPItemBracket *bracket = new QCPItemBracket(customPlot);
    customPlot->addItem(bracket);
    bracket->setStyle(QCPItemBracket::bsCalligraphic);
    bracket->left->setCoords(-0.35+i*0.18, 0.95);
    bracket->right->setCoords(-0.15+i*0.18, 0.05);
    bracket->setLength(10+i*5);
    labelItemAnchors(bracket, 0, true, false);
    QCPItemText *label = new QCPItemText(customPlot);
    customPlot->addItem(label);
    label->setText(QString::number(bracket->length()));
    label->position->setParentAnchor(bracket->right);
    label->position->setCoords(-5, 20);
    label->setFont(QFont(font().family(), 9));
  }
  for (int i=0; i<4; ++i)
  {
    QCPItemBracket *bracket = new QCPItemBracket(customPlot);
    customPlot->addItem(bracket);
    bracket->setStyle(QCPItemBracket::bsSquare);
    bracket->left->setCoords(0.55+i*0.18, 0.95);
    bracket->right->setCoords(0.75+i*0.18, 0.05);
    bracket->setLength(10+i*5);
    labelItemAnchors(bracket, 0, true, false);
    QCPItemText *label = new QCPItemText(customPlot);
    customPlot->addItem(label);
    label->setText(QString::number(bracket->length()));
    label->position->setParentAnchor(bracket->right);
    label->position->setCoords(-5, 20);
    label->setFont(QFont(font().family(), 9));
  }
  QCPItemText *topLabel1 = new QCPItemText(customPlot);
  customPlot->addItem(topLabel1);
  topLabel1->setText("bsCalligraphic");
  topLabel1->position->setCoords(-0.05, 1.1);
  topLabel1->setFont(QFont(font().family(), 10));
  QCPItemText *topLabel2 = new QCPItemText(customPlot);
  customPlot->addItem(topLabel2);
  topLabel2->setText("bsSquare");
  topLabel2->position->setCoords(0.85, 1.1);
  topLabel2->setFont(QFont(font().family(), 10));
  customPlot->savePng(dir.filePath("QCPItemBracket-length.png"), 450, 200);
}

void MainWindow::genItemText()
{
  resetPlot();
  QCPItemText *text = new QCPItemText(customPlot);
  customPlot->addItem(text);
  text->position->setCoords(0.5, 0.5);
  text->setText("QCustomPlot\nWidget");
  text->setFont(QFont(font().family(), 26));
  text->setRotation(12);
  text->setBrush(defaultBrush);
  labelItemAnchors(text);
  customPlot->savePng(dir.filePath("QCPItemText.png"), 300, 170);
}

void MainWindow::genItemTracer()
{
  resetPlot();
  customPlot->addGraph();
  QVector<double> x(50), y(50);
  for (int i=0; i<50; ++i)
  {
    x[i] = -0.4+1.8*i/49.0;
    y[i] = qSin(x[i]*M_PI*0.9)*0.4+0.4;
  }
  customPlot->graph()->setData(x, y);
  customPlot->graph()->setPen(QPen(Qt::red));
  QCPItemTracer *tracer = new QCPItemTracer(customPlot);
  customPlot->addItem(tracer);
  tracer->setStyle(QCPItemTracer::tsCrosshair);
  tracer->setGraph(customPlot->graph());
  tracer->setGraphKey(0.8);
  labelItemAnchors(tracer, 8, true, false);
  customPlot->savePng(dir.filePath("QCPItemTracer.png"), 230, 160);
}

void MainWindow::genLineEnding()
{
  resetPlot();
  QMap<QCPLineEnding::EndingStyle, QString> endingStyles;
  endingStyles.insert(QCPLineEnding::esNone, "esNone");
  endingStyles.insert(QCPLineEnding::esFlatArrow, "esFlatArrow");
  endingStyles.insert(QCPLineEnding::esSpikeArrow, "esSpikeArrow");
  endingStyles.insert(QCPLineEnding::esLineArrow, "esLineArrow");
  endingStyles.insert(QCPLineEnding::esDisc, "esDisc");
  endingStyles.insert(QCPLineEnding::esSquare, "esSquare");
  endingStyles.insert(QCPLineEnding::esDiamond, "esDiamond");
  endingStyles.insert(QCPLineEnding::esBar, "esBar");
  QMapIterator<QCPLineEnding::EndingStyle, QString> endingIt(endingStyles);
  int i = 0;
  double offset = -0.2;
  double step = 1.4/((double)endingStyles.size()-1);
  while (endingIt.hasNext())
  {
    endingIt.next();
    QCPItemLine *line = new QCPItemLine(customPlot);
    customPlot->addItem(line);
    line->start->setCoords(offset+i*step-0.1, -0.2);
    line->end->setCoords(offset+i*step, 0.5);
    line->setHead(endingIt.key());
    QCPItemText *text = new QCPItemText(customPlot);
    customPlot->addItem(text);
    text->position->setParentAnchor(line->end);
    text->position->setCoords(8, -15-(i%2)*15);
    text->setFont(QFont(font().family(), 8));
    text->setText(endingIt.value());
    ++i;
  }
  customPlot->savePng(dir.filePath("QCPLineEnding.png"), 400, 100);
}

void MainWindow::labelItemAnchors(QCPAbstractItem *item, double fontSize, bool circle, bool labelBelow)
{
  QList<QCPItemAnchor*> anchors = item->anchors();
  for (int i=0; i<anchors.size(); ++i)
  {
    if (circle)
    {
      QCPItemEllipse *circ = new QCPItemEllipse(item->parentPlot());
      item->parentPlot()->addItem(circ);
      circ->topLeft->setParentAnchor(anchors.at(i));
      circ->bottomRight->setParentAnchor(anchors.at(i));
      circ->topLeft->setCoords(-4, -4);
      circ->bottomRight->setCoords(4, 4);
      QPen p(Qt::blue, 0, Qt::CustomDashLine);
#if (QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)) // Qt 4.8 changed the meaning of this completely...
      p.setDashPattern(QVector<qreal>()<<2<<1);
#else
      p.setDashPattern(QVector<qreal>()<<1<<3);
#endif
      circ->setPen(p);
      if (dynamic_cast<QCPItemPosition*>(anchors.at(i)))
      {
        QCPItemEllipse *circ2 = new QCPItemEllipse(item->parentPlot());
        item->parentPlot()->addItem(circ2);
        circ2->topLeft->setParentAnchor(anchors.at(i));
        circ2->bottomRight->setParentAnchor(anchors.at(i));
        circ2->topLeft->setCoords(-2.5, -2.5);
        circ2->bottomRight->setCoords(2.5, 2.5);
        circ2->setPen(Qt::NoPen);
        circ2->setBrush(Qt::blue);
      }
    }
    if (fontSize > 0)
    {
      QCPItemText *label = new QCPItemText(item->parentPlot());
      item->parentPlot()->addItem(label);
      label->setFont(QFont(font().family(), fontSize));
      label->setColor(Qt::blue);
      label->setText(QString("%2 (%1)").arg(i).arg(anchors.at(i)->name()));
      label->position->setParentAnchor(anchors.at(i));
      if (circle)
        label->position->setCoords(0, fontSize*2*(labelBelow?1:-1));
      else
        label->position->setCoords(0, 0);
      label->setTextAlignment(Qt::AlignCenter);
    }
  }
}

void MainWindow::resetPlot()
{
  customPlot = new QCustomPlot(this);
  setCentralWidget(customPlot);
  customPlot->xAxis->setRange(-0.4, 1.4);
  customPlot->yAxis->setRange(-0.2, 1.2);
  customPlot->xAxis->setVisible(false);
  customPlot->yAxis->setVisible(false);
  customPlot->setAutoMargin(false);
  customPlot->setMargin(0, 0, 0, 0);
  
}
