///***************************************************************************
//**                                                                        **
//**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
//**  Copyright (C) 2011, 2012 Emanuel Eichhammer                           **
//**                                                                        **
//**  This program is free software: you can redistribute it and/or modify  **
//**  it under the terms of the GNU General Public License as published by  **
//**  the Free Software Foundation, either version 3 of the License, or     **
//**  (at your option) any later version.                                   **
//**                                                                        **
//**  This program is distributed in the hope that it will be useful,       **
//**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
//**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
//**  GNU General Public License for more details.                          **
//**                                                                        **
//**  You should have received a copy of the GNU General Public License     **
//**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
//**                                                                        **
//****************************************************************************
//**           Author: Emanuel Eichhammer                                   **
//**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
//**             Date: 09.06.12                                             **
//****************************************************************************/

%Module qcustomplot
%Import QtGui/QtGuimod.sip

namespace QCP
{
%TypeHeaderCode
#include "qcustomplot.h"
%End
enum ScatterStyle { ssNone
                    ,ssDot       
                    ,ssCross     
                    ,ssPlus      
                    ,ssCircle    
                    ,ssDisc      
                    ,ssSquare    ///< \enumimage{ssSquare.png} a square which is not filled
                    ,ssDiamond   ///< \enumimage{ssDiamond.png} a diamond which is not filled
                    ,ssStar      ///< \enumimage{ssStar.png} a star with eight arms, i.e. a combination of cross and plus
                    ,ssTriangle  ///< \enumimage{ssTriangle.png} an equilateral triangle which is not filled, standing on baseline
                    ,ssTriangleInverted ///< \enumimage{ssTriangleInverted.png} an equilateral triangle which is not filled, standing on corner
                    ,ssCrossSquare      ///< \enumimage{ssCrossSquare.png} a square which is not filled, with a cross inside
                    ,ssPlusSquare       ///< \enumimage{ssPlusSquare.png} a square which is not filled, with a plus inside
                    ,ssCrossCircle      ///< \enumimage{ssCrossCircle.png} a circle which is not filled, with a cross inside
                    ,ssPlusCircle       ///< \enumimage{ssPlusCircle.png} a circle which is not filled, with a plus inside
                    ,ssPeace     ///< \enumimage{ssPeace.png} a circle which is not filled, with one vertical and two downward diagonal lines
                    ,ssPixmap    ///< \enumimage{ssPixmap.png} a custom pixmap specified by setScatterPixmap, centered on the data point coordinates
                  };
enum AntialiasedElement { aeAxes           = 0x0001 ///< <tt>0x0001</tt> Axis base line and tick marks
                          ,aeGrid          = 0x0002 ///< <tt>0x0002</tt> Grid lines
                          ,aeSubGrid       = 0x0004 ///< <tt>0x0004</tt> Sub grid lines
                          ,aeLegend        = 0x0008 ///< <tt>0x0008</tt> Legend box
                          ,aeLegendItems   = 0x0010 ///< <tt>0x0010</tt> Legend items
                          ,aePlottables    = 0x0020 ///< <tt>0x0020</tt> Main lines of plottables (excluding error bars, see element \ref aeErrorBars)
                          ,aeItems         = 0x0040 ///< <tt>0x0040</tt> Main lines of items
                          ,aeScatters      = 0x0080 ///< <tt>0x0080</tt> Scatter symbols of plottables (excluding scatter symbols of type ssPixmap)
                          ,aeErrorBars     = 0x0100 ///< <tt>0x0100</tt> Error bars
                          ,aeFills         = 0x0200 ///< <tt>0x0200</tt> Borders of fills (e.g. under or between graphs)
                          ,aeZeroLine      = 0x0400 ///< <tt>0x0400</tt> Zero-lines, see \ref QCPAxis::setZeroLinePen
                          ,aeAll           = 0xFFFF ///< <tt>0xFFFF</tt> All elements
                          ,aeNone          = 0x0000 ///< <tt>0x0000</tt> No elements
                        }; 
typedef QFlags<QCP::AntialiasedElement> AntialiasedElements;

enum PlottingHint { phNone            = 0x000 ///< <tt>0x000</tt> No hints are set
                    ,phFastPolylines  = 0x001 ///< <tt>0x001</tt> Graph/Curve lines are drawn with a faster method. This reduces the quality
                                              ///<                especially of the line segment joins. (Only used for solid line pens.)
                    ,phForceRepaint   = 0x002 ///< <tt>0x002</tt> causes an immediate repaint() instead of a soft update() when QCustomPlot::replot() is called. This is set by default
                                              ///<                on Windows-Systems to prevent the plot from freezing on fast consecutive replots (e.g. user drags ranges with mouse).
                    ,phCacheLabels    = 0x004 ///< <tt>0x004</tt> axis (tick) labels will be cached as pixmaps, increasing replot performance.
                  };
typedef QFlags<QCP::PlottingHint> PlottingHints;
};

class QCPPainter : QPainter
{

%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPPainter();
  QCPPainter(QPaintDevice *device);
  ~QCPPainter();
  
  // getters:
  QPixmap scatterPixmap() const;
  bool antialiasing() const;
  bool pdfExportMode() const;
  bool scaledExportMode() const;
  
  // setters:
  void setScatterPixmap(const QPixmap pm);
  void setAntialiasing(bool enabled);
  void setPdfExportMode(bool enabled);
  void setScaledExportMode(bool enabled);
 
  // methods hiding non-virtual base class functions (QPainter bug workarounds):
  void setPen(const QPen &pen);
  void setPen(const QColor &color);
  void setPen(Qt::PenStyle penStyle);
  void drawLine(const QLineF &line);
  void drawLine(const QPointF &p1, const QPointF &p2);
  void save();
  void restore();

  // helpers:
  void fixScaledPen();
  void drawScatter(double x, double y, double size, QCP::ScatterStyle style);
};


class  QCPLayer
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPLayer(QCustomPlot* parentPlot, const QString &layerName);
  ~QCPLayer();
  
  // getters:
  QCustomPlot *parentPlot() const;
  QString name() const;
  int index() const;
  QList<QCPLayerable*> children() const;
  
protected:
  void addChild(QCPLayerable *layerable, bool prepend);
  void removeChild(QCPLayerable *layerable);

private:
  QCPLayer(const QCPLayer &);
  
};

class  QCPLayerable : public QObject
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPLayerable(QCustomPlot *parentPlot);
  ~QCPLayerable();
  
  // getters:
  bool visible() const;
  QCustomPlot *parentPlot() const;
  QCPLayer *layer() const;
  bool antialiased() const;
  
  // setters:
  void setVisible(bool on);
  bool setLayer(QCPLayer *layer);
  bool setLayer(const QString &layerName);
  void setAntialiased(bool enabled);
  
protected:
  // non-property methods:
  bool moveToLayer(QCPLayer *layer, bool prepend);
  
  void applyAntialiasingHint(QCPPainter *painter, bool localAntialiased, QCP::AntialiasedElement overrideElement) const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const = 0;
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  
private:
  QCPLayerable(const QCPLayerable &);
};


class  QCPRange
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  double lower;
  double upper;
  QCPRange();
  QCPRange(double lower, double upper);
  double size() const;
  double center() const;
  void normalize();
  QCPRange sanitizedForLogScale() const;
  QCPRange sanitizedForLinScale() const;
  bool contains(double value) const;
  
  static bool validRange(double lower, double upper);
  static bool validRange(const QCPRange &range);
  static const double minRange; //1e-280;
  static const double maxRange; //1e280;
};


class  QCPGrid : public QCPLayerable
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPGrid(QCPAxis *parentAxis);
  ~QCPGrid();
  
  // getters:
  bool subGridVisible() const;
  bool antialiasedSubGrid() const;
  bool antialiasedZeroLine() const;
  QPen pen() const;
  QPen subGridPen() const;
  QPen zeroLinePen() const;
  
  // setters:
  void setSubGridVisible(bool visible);
  void setAntialiasedSubGrid(bool enabled);
  void setAntialiasedZeroLine(bool enabled);
  void setPen(const QPen &pen);
  void setSubGridPen(const QPen &pen);
  void setZeroLinePen(const QPen &pen);
  
protected:
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter);
  // drawing helpers:
  void drawGridLines(QCPPainter *painter) const;
  void drawSubGridLines(QCPPainter *painter) const;
};


class  QCPAxis : public QCPLayerable
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum AxisType { atLeft    ///< Axis is vertical and on the left side of the axis rect of the parent QCustomPlot
                  ,atRight  ///< Axis is vertical and on the right side of the axis rect of the parent QCustomPlot
                  ,atTop    ///< Axis is horizontal and on the top side of the axis rect of the parent QCustomPlot
                  ,atBottom ///< Axis is horizontal and on the bottom side of the axis rect of the parent QCustomPlot
                };
  enum LabelType { ltNumber    ///< Tick coordinate is regarded as normal number and will be displayed as such. (see \ref setNumberFormat)
                   ,ltDateTime ///< Tick coordinate is regarded as a date/time (seconds since 1970-01-01T00:00:00 UTC, see QDateTime::toTime_t) and will be displayed and formatted as such. (see \ref setDateTimeFormat)
                 };
  enum ScaleType { stLinear       ///< Normal linear scaling
                   ,stLogarithmic ///< Logarithmic scaling with correspondingly transformed plots and (major) tick marks at every base power (see \ref setScaleLogBase).
                 };
  enum SelectablePart { spNone        = 0      ///< None of the selectable parts
                        ,spAxis       = 0x001  ///< The axis backbone and tick marks
                        ,spTickLabels = 0x002  ///< Tick labels (numbers) of this axis (as a whole, not individually)
                        ,spAxisLabel  = 0x004  ///< The axis label
                      };
  typedef QFlags<QCPAxis::SelectablePart> SelectableParts;
  
  explicit QCPAxis(QCustomPlot *parentPlot, AxisType type);
  virtual ~QCPAxis();
      
  // getters:
  AxisType axisType() const;
  QRect axisRect() const;
  ScaleType scaleType() const;
  double scaleLogBase() const;
  const QCPRange range() const;
  bool rangeReversed() const;
  bool antialiasedGrid() const;
  bool antialiasedSubGrid() const;
  bool antialiasedZeroLine() const;
  bool grid() const;
  bool subGrid() const;
  bool autoTicks() const;
  int autoTickCount() const;
  bool autoTickLabels() const;
  bool autoTickStep() const;
  bool autoSubTicks() const;
  bool ticks() const;
  bool tickLabels() const;
  int tickLabelPadding() const;
  LabelType tickLabelType() const;
  QFont tickLabelFont() const;
  QColor tickLabelColor() const;
  double tickLabelRotation() const;
  QString dateTimeFormat() const;
  QString numberFormat() const;
  int numberPrecision() const;
  double tickStep() const;
  QVector<double> tickVector() const;
  QVector<QString> tickVectorLabels() const;
  int tickLengthIn() const;
  int tickLengthOut() const;
  int subTickCount() const;
  int subTickLengthIn() const;
  int subTickLengthOut() const;
  QPen basePen() const;
  QPen gridPen() const;
  QPen subGridPen() const;
  QPen zeroLinePen() const;
  QPen tickPen() const;
  QPen subTickPen() const;
  QFont labelFont() const;
  QColor labelColor() const;
  QString label() const;
  int labelPadding() const;
  int padding() const;
  SelectableParts selected() const;
  SelectableParts selectable() const;
  QFont selectedTickLabelFont() const;
  QFont selectedLabelFont() const;
  QColor selectedTickLabelColor() const;
  QColor selectedLabelColor() const;
  QPen selectedBasePen() const;
  QPen selectedTickPen() const;
  QPen selectedSubTickPen() const;
  
  // setters:
  void setScaleType(ScaleType type);
  void setScaleLogBase(double base);
  void setRange(double lower, double upper);
  void setRange(double position, double size, Qt::AlignmentFlag alignment);
  void setRangeLower(double lower);
  void setRangeUpper(double upper);
  void setRangeReversed(bool reversed);
  void setAntialiasedGrid(bool enabled);
  void setAntialiasedSubGrid(bool enabled);
  void setAntialiasedZeroLine(bool enabled);
  void setGrid(bool show);
  void setSubGrid(bool show);
  void setAutoTicks(bool on);
  void setAutoTickCount(int approximateCount);
  void setAutoTickLabels(bool on);
  void setAutoTickStep(bool on);
  void setAutoSubTicks(bool on);
  void setTicks(bool show);
  void setTickLabels(bool show);
  void setTickLabelPadding(int padding);
  void setTickLabelType(LabelType type);
  void setTickLabelFont(const QFont &font);
  void setTickLabelColor(const QColor &color);
  void setTickLabelRotation(double degrees);
  void setDateTimeFormat(const QString &format);
  void setNumberFormat(const QString &formatCode);
  void setNumberPrecision(int precision);
  void setTickStep(double step);
  void setTickVector(const QVector<double> &vec);
  void setTickVectorLabels(const QVector<QString> &vec);
  void setTickLength(int inside, int outside=0);
  void setSubTickCount(int count);
  void setSubTickLength(int inside, int outside=0);
  void setBasePen(const QPen &pen);
  void setGridPen(const QPen &pen);
  void setSubGridPen(const QPen &pen);
  void setZeroLinePen(const QPen &pen);
  void setTickPen(const QPen &pen);
  void setSubTickPen(const QPen &pen);
  void setLabelFont(const QFont &font);
  void setLabelColor(const QColor &color);
  void setLabel(const QString &str);
  void setLabelPadding(int padding);
  void setPadding(int padding);
  void setSelectedTickLabelFont(const QFont &font);
  void setSelectedLabelFont(const QFont &font);
  void setSelectedTickLabelColor(const QColor &color);
  void setSelectedLabelColor(const QColor &color);
  void setSelectedBasePen(const QPen &pen);
  void setSelectedTickPen(const QPen &pen);
  void setSelectedSubTickPen(const QPen &pen);
  
  // non-property methods:
  Qt::Orientation orientation() const;
  void moveRange(double diff);
  void scaleRange(double factor, double center);
  void setScaleRatio(const QCPAxis *otherAxis, double ratio=1.0);
  double pixelToCoord(double value) const;
  double coordToPixel(double value) const;
  SelectablePart selectTest(const QPointF &pos) const;
  
public slots:
  // slot setters:
  void setRange(const QCPRange &range);
  void setSelectable(const QCPAxis::SelectableParts &selectable);
  void setSelected(const QCPAxis::SelectableParts &selected);
  
signals:
  void ticksRequest();
  void rangeChanged(const QCPRange &newRange);
  void selectionChanged(QCPAxis::SelectableParts selection);

protected:
  // internal setters:
  void setAxisType(AxisType type);
  void setAxisRect(const QRect &rect);
  
  // introduced methods:
  virtual void setupTickVectors();
  virtual void generateAutoTicks();
  virtual int calculateAutoSubTickCount(double tickStep) const;
  virtual int calculateMargin() const;
  virtual bool handleAxisSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  
  // drawing:
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter); 
  virtual void placeTickLabel(QCPPainter *painter, double position, int distanceToAxis, const QString &text, QSize *tickLabelsSize);
  
  // tick label drawing/caching helpers:
  //virtual void drawTickLabel(QCPPainter *painter, double x, double y, const TickLabelData &labelData) const;
  //virtual TickLabelData getTickLabelData(const QFont &font, const QString &text) const;
  //virtual QPointF getTickLabelDrawOffset(const TickLabelData &labelData) const;
  //virtual void getMaxTickLabelSize(const QFont &font, const QString &text, QSize *tickLabelsSize) const;
  
  // basic non virtual helpers:
  void visibleTickBounds(int &lowIndex, int &highIndex) const;
  double baseLog(double value) const;
  double basePow(double value) const;
  
  // helpers to get the right pen/font depending on selection state:
  QPen getBasePen() const;
  QPen getTickPen() const;
  QPen getSubTickPen() const;
  QFont getTickLabelFont() const;
  QFont getLabelFont() const;
  QColor getTickLabelColor() const;
  QColor getLabelColor() const;
  
private:
  QCPAxis(const QCPAxis &);
};


class  QCPAbstractLegendItem : public QObject /Abstract/
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPAbstractLegendItem(QCPLegend *parent);
  virtual ~QCPAbstractLegendItem();
  
  // getters:
  bool antialiased() const;
  QFont font() const;
  QColor textColor() const;
  QFont selectedFont() const;
  QColor selectedTextColor() const;
  bool selectable() const;
  bool selected() const;
  
  // setters:
  void setAntialiased(bool enabled);
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);
  void setSelectable(bool selectable);
  void setSelected(bool selected);
  
signals:
  void selectionChanged(bool selected);
  
protected:
  virtual void draw(QCPPainter *painter, const QRect &rect) const = 0;
  virtual QSize size(const QSize &targetSize) const = 0;
  void applyAntialiasingHint(QCPPainter *painter) const;

private:
  QCPAbstractLegendItem(const QCPAbstractLegendItem &);
};


class  QCPPlottableLegendItem : public QCPAbstractLegendItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPPlottableLegendItem(QCPLegend *parent, QCPAbstractPlottable *plottable);
  virtual ~QCPPlottableLegendItem();
  
  // getters:
  QCPAbstractPlottable *plottable();
  bool textWrap() const;
  
  // setters:
  void setTextWrap(bool wrap);
  
protected:
  QPen getIconBorderPen() const;
  QColor getTextColor() const;
  QFont getFont() const;

  virtual void draw(QCPPainter *painter, const QRect &rect) const;
  virtual QSize size(const QSize &targetSize) const;
};


class  QCPLegend : public QCPLayerable
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum PositionStyle { psManual       ///< Position is not changed automatically. Set manually via \ref setPosition
                      ,psTopLeft      ///< Legend is positioned in the top left corner of the axis rect with distance to the border corresponding to the currently set top and left margins
                      ,psTop          ///< Legend is horizontally centered at the top of the axis rect with distance to the border corresponding to the currently set top margin
                      ,psTopRight     ///< Legend is positioned in the top right corner of the axis rect with distance to the border corresponding to the currently set top and right margins
                      ,psRight        ///< Legend is vertically centered at the right of the axis rect with distance to the border corresponding to the currently set right margin
                      ,psBottomRight  ///< Legend is positioned in the bottom right corner of the axis rect with distance to the border corresponding to the currently set bottom and right margins
                      ,psBottom       ///< Legend is horizontally centered at the bottom of the axis rect with distance to the border corresponding to the currently set bottom margin
                      ,psBottomLeft   ///< Legend is positioned in the bottom left corner of the axis rect with distance to the border corresponding to the currently set bottom and left margins
                      ,psLeft         ///< Legend is vertically centered at the left of the axis rect with distance to the border corresponding to the currently set left margin
                     };
  enum SelectablePart { spNone       = 0      ///< None
                       ,spLegendBox  = 0x001  ///< The legend box (frame)
                       ,spItems      = 0x002  ///< Legend items individually (see \ref selectedItems)
                      };
  typedef QFlags<QCPLegend::SelectablePart> SelectableParts;
  
  explicit QCPLegend(QCustomPlot *parentPlot);
  virtual ~QCPLegend();
  
  // getters:
  QPen borderPen() const;
  QBrush brush() const;
  QFont font() const;
  QColor textColor() const;
  PositionStyle positionStyle() const;
  QPoint position() const;
  bool autoSize() const;
  QSize size() const;
  QSize minimumSize() const;
  int paddingLeft() const;
  int paddingRight() const;
  int paddingTop() const;
  int paddingBottom() const;
  int marginLeft() const;
  int marginRight() const;
  int marginTop() const;
  int marginBottom() const;
  int itemSpacing() const;
  QSize iconSize() const;
  int iconTextPadding() const;
  QPen iconBorderPen() const;
  SelectableParts selectable() const;
  SelectableParts selected() const;
  QPen selectedBorderPen() const;
  QPen selectedIconBorderPen() const;
  QBrush selectedBrush() const;
  QFont selectedFont() const;
  QColor selectedTextColor() const;
  
  // setters:
  void setBorderPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setTextColor(const QColor &color);
  void setPositionStyle(PositionStyle legendPositionStyle);
  void setPosition(const QPoint &pixelPosition);
  void setAutoSize(bool on);
  void setSize(const QSize &size);
  void setSize(int width, int height);
  void setMinimumSize(const QSize &size);
  void setMinimumSize(int width, int height);
  void setPaddingLeft(int padding);
  void setPaddingRight(int padding);
  void setPaddingTop(int padding);
  void setPaddingBottom(int padding);
  void setPadding(int left, int right, int top, int bottom);
  void setMarginLeft(int margin);
  void setMarginRight(int margin);
  void setMarginTop(int margin);
  void setMarginBottom(int margin);
  void setMargin(int left, int right, int top, int bottom);
  void setItemSpacing(int spacing);
  void setIconSize(const QSize &size);
  void setIconSize(int width, int height);
  void setIconTextPadding(int padding);
  void setIconBorderPen(const QPen &pen);
  void setSelectable(const SelectableParts &selectable);
  void setSelected(const SelectableParts &selected);
  void setSelectedBorderPen(const QPen &pen);
  void setSelectedIconBorderPen(const QPen &pen);
  void setSelectedBrush(const QBrush &brush);
  void setSelectedFont(const QFont &font);
  void setSelectedTextColor(const QColor &color);

  // non-property methods:
  QCPAbstractLegendItem *item(int index) const;
  QCPPlottableLegendItem *itemWithPlottable(const QCPAbstractPlottable *plottable) const;
  int itemCount() const;
  bool hasItem(QCPAbstractLegendItem *item) const;
  bool hasItemWithPlottable(const QCPAbstractPlottable *plottable) const;
  bool addItem(QCPAbstractLegendItem *item);
  bool removeItem(int index);
  bool removeItem(QCPAbstractLegendItem *item);
  void clearItems();
  QList<QCPAbstractLegendItem*> selectedItems() const;
  void reArrange();
  
  bool selectTestLegend(const QPointF &pos) const;
  QCPAbstractLegendItem *selectTestItem(const QPoint pos) const;
  
signals:
  void selectionChanged(QCPLegend::SelectableParts selection);
  
protected:
  virtual void updateSelectionState();
  virtual bool handleLegendSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  // introduced methods:
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter);
  virtual void calculateAutoSize();
  virtual void calculateAutoPosition();
  
  // drawing helpers:
  QPen getBorderPen() const;
  QBrush getBrush() const;

private:
  QCPLegend(const QCPLegend &);
  
};


class  QCPAbstractPlottable : public QCPLayerable /Abstract/
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPAbstractPlottable(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPAbstractPlottable();
  
  // getters:
  QString name() const;
  bool antialiasedFill() const;
  bool antialiasedScatters() const;
  bool antialiasedErrorBars() const;
  QPen pen() const;
  QPen selectedPen() const;
  QBrush brush() const;
  QBrush selectedBrush() const;
  QCPAxis *keyAxis() const;
  QCPAxis *valueAxis() const;
  bool selectable() const;
  bool selected() const;
  
  // setters:
  void setName(const QString &name);
  void setAntialiasedFill(bool enabled);
  void setAntialiasedScatters(bool enabled);
  void setAntialiasedErrorBars(bool enabled);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setKeyAxis(QCPAxis *axis);
  void setValueAxis(QCPAxis *axis);
  void setSelectable(bool selectable);
  void setSelected(bool selected);

  // non-property methods:
  void rescaleAxes(bool onlyEnlarge=false) const;
  void rescaleKeyAxis(bool onlyEnlarge=false) const;
  void rescaleValueAxis(bool onlyEnlarge=false) const;
  virtual void clearData() = 0;
  virtual double selectTest(const QPointF &pos) const = 0;
  virtual bool addToLegend();
  virtual bool removeFromLegend() const;
  
signals:
  void selectionChanged(bool selected);
  
protected:
  virtual QRect clipRect() const;
  virtual void draw(QCPPainter *painter) = 0;
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const = 0;
  //virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const = 0;
  //virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const = 0;
  
  // painting and coordinate transformation helpers:
  //void coordsToPixels(double key, double value, double &x, double &y) const;
  //const QPointF coordsToPixels(double key, double value) const;
  void pixelsToCoords(double x, double y, double &key, double &value) const;
  void pixelsToCoords(const QPointF &pixelPos, double &key, double &value) const;
  QPen mainPen() const;
  QBrush mainBrush() const;
  void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  void applyFillAntialiasingHint(QCPPainter *painter) const;
  void applyScattersAntialiasingHint(QCPPainter *painter) const;
  void applyErrorBarsAntialiasingHint(QCPPainter *painter) const;
  
  // selection test helpers:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;

private:
  QCPAbstractPlottable(const QCPAbstractPlottable &);

};


class  QCPItemAnchor
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemAnchor(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name, int anchorId=-1);
  virtual ~QCPItemAnchor();
  
  QString name() const;
  virtual QPointF pixelPoint() const;
  
protected:
  void addChild(QCPItemPosition* pos); // called from pos when this anchor is set as parent
  void removeChild(QCPItemPosition *pos); // called from pos when its parent anchor is reset or pos deleted
  
private:
  QCPItemAnchor(const QCPItemAnchor &);
};



class  QCPItemPosition : public QCPItemAnchor
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum PositionType { ptAbsolute        ///< Static positioning in pixels, starting from the top left corner of the viewport/widget.
                      ,ptViewportRatio  ///< Static positioning given by a ratio of the current viewport (coordinates 0 to 1).
                      ,ptAxisRectRatio  ///< Static positioning given by a ratio of the current axis rect (coordinates 0 to 1).
                      ,ptPlotCoords     ///< Dynamic positioning at a plot coordinate defined by two axes (see \ref setAxes).
                    };
  
  QCPItemPosition(QCustomPlot *parentPlot, QCPAbstractItem *parentItem, const QString name);
  virtual ~QCPItemPosition();
  
  // getters:
  PositionType type() const;
  QCPItemAnchor *parentAnchor() const;
  double key() const;
  double value() const;
  QPointF coords() const;
  QCPAxis *keyAxis() const;
  QCPAxis *valueAxis() const;
  virtual QPointF pixelPoint() const;
  
  // setters:
  void setType(PositionType type);
  bool setParentAnchor(QCPItemAnchor *parentAnchor, bool keepPixelPosition=false);
  void setCoords(double key, double value);
  void setCoords(const QPointF &coords);
  void setAxes(QCPAxis* keyAxis, QCPAxis* valueAxis);
  void setPixelPoint(const QPointF &pixelPoint);

private:
  QCPItemPosition(const QCPItemPosition &);
  
};


class  QCPAbstractItem : public QCPLayerable /Abstract/
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPAbstractItem(QCustomPlot *parentPlot);
  virtual ~QCPAbstractItem();
  
  // getters:
  bool clipToAxisRect() const;
  QCPAxis *clipKeyAxis() const;
  QCPAxis *clipValueAxis() const;
  bool selectable() const;
  bool selected() const;
  
  // setters:
  void setClipToAxisRect(bool clip);
  void setClipAxes(QCPAxis *keyAxis, QCPAxis *valueAxis);
  void setClipKeyAxis(QCPAxis *axis);
  void setClipValueAxis(QCPAxis *axis);
  void setSelectable(bool selectable);
  void setSelected(bool selected);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const = 0;
  QList<QCPItemPosition*> positions() const;
  QList<QCPItemAnchor*> anchors() const;
  QCPItemPosition *position(const QString &name) const;
  QCPItemAnchor *anchor(const QString &name) const;
  bool hasAnchor(const QString &name) const;
  
protected:
  virtual QRect clipRect() const;
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter) = 0;
  
  // helper functions for subclasses:
  double distSqrToLine(const QPointF &start, const QPointF &end, const QPointF &point) const;
  double rectSelectTest(const QRectF &rect, const QPointF &pos, bool filledRect) const;
  
  // anchor/position interface:
  virtual QPointF anchorPixelPoint(int anchorId) const;
  QCPItemPosition *createPosition(const QString &name);
  QCPItemAnchor *createAnchor(const QString &name, int anchorId);
  
signals:
  void selectionChanged(bool selected);
  
private:
  QCPAbstractItem(const QCPAbstractItem &);
};


class  QCPLineEnding
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum EndingStyle { esNone          ///< No ending decoration
                     ,esFlatArrow    ///< A filled arrow head with a straight/flat back (a triangle)
                     ,esSpikeArrow   ///< A filled arrow head with an indented back
                     ,esLineArrow    ///< A non-filled arrow head with open back
                     ,esDisc         ///< A filled circle
                     ,esSquare       ///< A filled square
                     ,esDiamond      ///< A filled diamond (45° rotated square)
                     ,esBar          ///< A bar perpendicular to the line
                   };
  
  QCPLineEnding();
  QCPLineEnding(EndingStyle style, double width=8, double length=10, bool inverted=false);
  
  // getters:
  EndingStyle style() const;
  double width() const;
  double length() const;
  bool inverted() const;
  
  // setters:
  void setStyle(EndingStyle style);
  void setWidth(double width);
  void setLength(double length);
  void setInverted(bool inverted);
  
  // non-property methods:
  double boundingDistance() const;
  void draw(QCPPainter *painter, const QVector2D &pos, const QVector2D &dir) const;
  void draw(QCPPainter *painter, const QVector2D &pos, double angle) const;
  
};


class  QCustomPlot : public QWidget
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref setRangeDrag, \ref setRangeDragAxes)
                     ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref setRangeZoom, \ref setRangeZoomAxes)
                     ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref setMultiSelectModifier while clicking
                     ,iSelectTitle      = 0x008 ///< <tt>0x008</tt> The plot title is selectable
                     ,iSelectPlottables = 0x010 ///< <tt>0x010</tt> Plottables are selectable
                     ,iSelectAxes       = 0x020 ///< <tt>0x020</tt> Axes are selectable (or parts of them, see QCPAxis::setSelectable)
                     ,iSelectLegend     = 0x040 ///< <tt>0x040</tt> Legends are selectable (or their child items, see QCPLegend::setSelectable)
                     ,iSelectItems      = 0x080 ///< <tt>0x080</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref QCPAbstractItem)
                   };
  typedef QFlags<QCustomPlot::Interaction> Interactions;

  enum LayerInsertMode { limBelow  ///< Layer is inserted below other layer
                         ,limAbove ///< Layer is inserted above other layer
                   };
  
  explicit QCustomPlot(QWidget *parent = 0);
  virtual ~QCustomPlot();
  
  // getters:
  QString title() const;
  QFont titleFont() const;
  QColor titleColor() const;
  QRect axisRect() const;
  QRect viewport() const;
  int marginLeft() const;
  int marginRight() const;
  int marginTop() const;
  int marginBottom() const;
  bool autoMargin() const;
  QColor color() const;
  Qt::Orientations rangeDrag() const;
  Qt::Orientations rangeZoom() const;
  QCPAxis *rangeDragAxis(Qt::Orientation orientation);
  QCPAxis *rangeZoomAxis(Qt::Orientation orientation);
  double rangeZoomFactor(Qt::Orientation orientation);
  QCP::AntialiasedElements antialiasedElements() const;
  QCP::AntialiasedElements notAntialiasedElements() const;
  bool autoAddPlottableToLegend() const;
  QPixmap axisBackground() const;
  bool axisBackgroundScaled() const;
  Qt::AspectRatioMode axisBackgroundScaledMode() const;
  const Interactions interactions() const;
  int selectionTolerance() const;
  QFont selectedTitleFont() const;
  QColor selectedTitleColor() const;
  bool titleSelected() const;
  bool noAntialiasingOnDrag() const;
  QCP::PlottingHints plottingHints() const;
  Qt::KeyboardModifier multiSelectModifier() const;

  // setters:
  void setTitle(const QString &title);
  void setTitleFont(const QFont &font);
  void setTitleColor(const QColor &color);
  void setAxisRect(const QRect &arect);
  void setMarginLeft(int margin);
  void setMarginRight(int margin);
  void setMarginTop(int margin);
  void setMarginBottom(int margin);
  void setMargin(int left, int right, int top, int bottom);
  void setAutoMargin(bool enabled);
  void setColor(const QColor &color);
  void setRangeDrag(Qt::Orientations orientations);
  void setRangeZoom(Qt::Orientations orientations);
  void setRangeDragAxes(QCPAxis *horizontal, QCPAxis *vertical);
  void setRangeZoomAxes(QCPAxis *horizontal, QCPAxis *vertical);
  void setRangeZoomFactor(double horizontalFactor, double verticalFactor);
  void setRangeZoomFactor(double factor);
  void setAntialiasedElements(const QCP::AntialiasedElements &antialiasedElements);
  void setAntialiasedElement(QCP::AntialiasedElement antialiasedElement, bool enabled=true);
  void setNotAntialiasedElements(const QCP::AntialiasedElements &notAntialiasedElements);
  void setNotAntialiasedElement(QCP::AntialiasedElement notAntialiasedElement, bool enabled=true);
  void setAutoAddPlottableToLegend(bool on);
  void setAxisBackground(const QPixmap &pm);
  void setAxisBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
  void setAxisBackgroundScaled(bool scaled);
  void setAxisBackgroundScaledMode(Qt::AspectRatioMode mode);
  void setInteractions(const Interactions &interactions);
  void setInteraction(const Interaction &interaction, bool enabled=true);
  void setSelectionTolerance(int pixels);
  void setSelectedTitleFont(const QFont &font);
  void setSelectedTitleColor(const QColor &color);
  void setTitleSelected(bool selected);
  void setNoAntialiasingOnDrag(bool enabled);
  void setPlottingHints(const QCP::PlottingHints &hints);
  void setPlottingHint(QCP::PlottingHint hint, bool enabled=true);
  void setMultiSelectModifier(Qt::KeyboardModifier modifier);
  
  // non-property methods:
  // plottable interface:
  QCPAbstractPlottable *plottable(int index);
  QCPAbstractPlottable *plottable();
  bool addPlottable(QCPAbstractPlottable *plottable);
  bool removePlottable(QCPAbstractPlottable *plottable);
  bool removePlottable(int index);
  int clearPlottables();
  int plottableCount() const;
  QList<QCPAbstractPlottable*> selectedPlottables() const;
  QCPAbstractPlottable *plottableAt(const QPointF &pos, bool onlySelectable=false) const;
  bool hasPlottable(QCPAbstractPlottable *plottable) const;

  // specialized interface for QCPGraph:
  QCPGraph *graph(int index) const;
  QCPGraph *graph() const;
  QCPGraph *addGraph(QCPAxis *keyAxis=0, QCPAxis *valueAxis=0);
  bool removeGraph(QCPGraph *graph);
  bool removeGraph(int index);
  int clearGraphs();
  int graphCount() const;
  QList<QCPGraph*> selectedGraphs() const;
  
  // item interface:
  QCPAbstractItem *item(int index) const;
  QCPAbstractItem *item() const;
  bool addItem(QCPAbstractItem* item);
  bool removeItem(QCPAbstractItem *item);
  bool removeItem(int index);
  int clearItems();
  int itemCount() const;
  QList<QCPAbstractItem*> selectedItems() const;
  QCPAbstractItem *itemAt(const QPointF &pos, bool onlySelectable=false) const;
  
  // layer interface:
  QCPLayer *layer(const QString &name) const;
  QCPLayer *layer(int index) const;
  QCPLayer *currentLayer() const;
  bool setCurrentLayer(const QString &name);
  bool setCurrentLayer(QCPLayer *layer);
  int layerCount() const;
  bool addLayer(const QString &name, QCPLayer *otherLayer=0, LayerInsertMode insertMode=limAbove);
  bool removeLayer(QCPLayer *layer);
  bool moveLayer(QCPLayer *layer, QCPLayer *otherLayer, LayerInsertMode insertMode=limAbove);
  
  QList<QCPAxis*> selectedAxes() const;
  QList<QCPLegend*> selectedLegends() const;
  void setupFullAxesBox();
  bool savePdf(const QString &fileName, bool noCosmeticPen=false, int width=0, int height=0);
  bool savePng(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveJpg(const QString &fileName, int width=0, int height=0, double scale=1.0, int quality=-1);
  bool saveBmp(const QString &fileName, int width=0, int height=0, double scale=1.0);
  bool saveRastered(const QString &fileName, int width, int height, double scale, const char *format, int quality=-1);
  QPixmap pixmap(int width=0, int height=0, double scale=1.0);
  
  QCPAxis *xAxis;
  QCPAxis *yAxis;
  QCPAxis *xAxis2;
  QCPAxis *yAxis2;
  QCPLegend *legend;
  
public slots:
  void deselectAll();
  void replot();
  void rescaleAxes();
  
signals:
  void mouseDoubleClick(QMouseEvent *event);
  void mousePress(QMouseEvent *event);
  void mouseMove(QMouseEvent *event);
  void mouseRelease(QMouseEvent *event);
  void mouseWheel(QWheelEvent *event);
  
  void plottableClick(QCPAbstractPlottable *plottable, QMouseEvent *event);
  void plottableDoubleClick(QCPAbstractPlottable *plottable, QMouseEvent *event);
  void itemClick(QCPAbstractItem *item, QMouseEvent *event);
  void itemDoubleClick(QCPAbstractItem *item, QMouseEvent *event);
  void axisClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
  void axisDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);
  void legendClick(QCPLegend *legend, QCPAbstractLegendItem *item, QMouseEvent *event);
  void legendDoubleClick(QCPLegend *legend,  QCPAbstractLegendItem *item, QMouseEvent *event);
  void titleClick(QMouseEvent *event);
  void titleDoubleClick(QMouseEvent *event);
  
  void selectionChangedByUser();
  void beforeReplot();
  void afterReplot();
  
protected:
  // reimplemented methods:
  virtual QSize minimumSizeHint() const;
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);
  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void wheelEvent(QWheelEvent *event);
  // event helpers:
  virtual bool handlePlottableSelection(QMouseEvent *event, bool additiveSelection, bool &modified);  
  virtual bool handleItemSelection(QMouseEvent *event, bool additiveSelection, bool &modified);  
  virtual bool handleAxisSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  virtual bool handleTitleSelection(QMouseEvent *event, bool additiveSelection, bool &modified);
  
  // introduced methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawAxisBackground(QCPPainter *painter);
  
  // helpers:
  void updateAxisRect();
  bool selectTestTitle(const QPointF &pos) const;
};


class  QCPData
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPData();
  QCPData(double key, double value);
  double key;
  double value;
  double keyErrorPlus;
  double keyErrorMinus;
  double valueErrorPlus;
  double valueErrorMinus;
};

typedef QMap<double, QCPData> QCPDataMap;
typedef QMapIterator<double, QCPData> QCPDataMapIterator;
typedef QMutableMapIterator<double, QCPData> QCPDataMutableMapIterator;


class  QCPGraph : public QCPAbstractPlottable
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum LineStyle { lsNone        ///< data points are not connected with any lines (e.g. data only represented
                                 ///< with symbols according to the scatter style, see \ref setScatterStyle)
                  ,lsLine        ///< data points are connected by a straight line
                  ,lsStepLeft    ///< line is drawn as steps where the step height is the value of the left data point
                  ,lsStepRight   ///< line is drawn as steps where the step height is the value of the right data point
                  ,lsStepCenter  ///< line is drawn as steps where the step is in between two data points
                  ,lsImpulse     ///< each data point is represented by a line parallel to the value axis, which reaches from the data point to the zero-value-line
                 };
  enum ErrorType { etNone   ///< No error bars are shown
                  ,etKey    ///< Error bars for the key dimension of the data point are shown
                  ,etValue  ///< Error bars for the value dimension of the data point are shown
                  ,etBoth   ///< Error bars for both key and value dimensions of the data point are shown
                 };
  
  explicit QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPGraph();
  
  // getters:
  //const QCPDataMap *data() const;
  LineStyle lineStyle() const;
  QCP::ScatterStyle scatterStyle() const;
  double scatterSize() const;
  const QPixmap scatterPixmap() const;
  ErrorType errorType() const;
  QPen errorPen() const;
  double errorBarSize() const;
  bool errorBarSkipSymbol() const;
  QCPGraph *channelFillGraph() const;
  
  // setters:
  //void setData(QCPDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError);
  void setDataKeyError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueError);
  void setDataValueError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyError, const QVector<double> &valueError);
  void setDataBothError(const QVector<double> &key, const QVector<double> &value, const QVector<double> &keyErrorMinus, const QVector<double> &keyErrorPlus, const QVector<double> &valueErrorMinus, const QVector<double> &valueErrorPlus);
  void setLineStyle(LineStyle ls);
  void setScatterStyle(QCP::ScatterStyle ss);
  void setScatterSize(double size);
  void setScatterPixmap(const QPixmap &pixmap);
  void setErrorType(ErrorType errorType);
  void setErrorPen(const QPen &pen);
  void setErrorBarSize(double size);
  void setErrorBarSkipSymbol(bool enabled);
  void setChannelFillGraph(QCPGraph *targetGraph);
  
  // non-property methods:
  //void addData(const QCPDataMap &dataMap);
  void addData(const QCPData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  virtual void rescaleAxes(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  virtual void rescaleKeyAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  virtual void rescaleValueAxis(bool onlyEnlarge, bool includeErrorBars) const; // overloads base class interface
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;

  // functions to generate plot data points in pixel coordinates:
  void getPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  // plot style specific functions to generate plot data, used by getPlotData:
  void getScatterPlotData(QVector<QCPData> *pointData) const;
  void getLinePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepLeftPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepRightPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getStepCenterPlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  void getImpulsePlotData(QVector<QPointF> *lineData, QVector<QCPData> *pointData) const;
  
  // helper functions for drawing:
  virtual void drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawScatterPlot(QCPPainter *painter, QVector<QCPData> *pointData) const;
  virtual void drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  void drawError(QCPPainter *painter, double x, double y, const QCPData &data) const;
  
  // helper functions:
  //void getVisibleDataBounds(QCPDataMap::const_iterator &lower, QCPDataMap::const_iterator &upper, int &count) const;
  void addFillBasePoints(QVector<QPointF> *lineData) const;
  void removeFillBasePoints(QVector<QPointF> *lineData) const;
  QPointF lowerFillBasePoint(double lowerKey) const;
  QPointF upperFillBasePoint(double upperKey) const;
  const QPolygonF getChannelFillPolygon(const QVector<QPointF> *lineData) const;
  int findIndexBelowX(const QVector<QPointF> *data, double x) const;
  int findIndexAboveX(const QVector<QPointF> *data, double x) const;
  int findIndexBelowY(const QVector<QPointF> *data, double y) const;
  int findIndexAboveY(const QVector<QPointF> *data, double y) const;
  double pointDistance(const QPointF &pixelPoint) const;
  //virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  //virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  //virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  //virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain, bool includeErrors) const; // overloads base class interface
  
};


class  QCPCurveData
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPCurveData();
  QCPCurveData(double t, double key, double value);
  double t;
  double key;
  double value;
};

typedef QMap<double, QCPCurveData> QCPCurveDataMap;
typedef QMapIterator<double, QCPCurveData> QCPCurveDataMapIterator;
typedef QMutableMapIterator<double, QCPCurveData> QCPCurveDataMutableMapIterator;


class  QCPCurve : public QCPAbstractPlottable
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum LineStyle { lsNone, ///< No line is drawn between data points (e.g. only scatters)
                   lsLine  ///< Data points are connected with a straight line
                 };
  explicit QCPCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPCurve();
  
  // getters:
  //const QCPCurveDataMap *data() const;
  QCP::ScatterStyle scatterStyle() const;
  double scatterSize() const;
  QPixmap scatterPixmap() const;
  LineStyle lineStyle() const;
  
  // setters:
  //void setData(QCPCurveDataMap *data, bool copy=false);
  void setData(const QVector<double> &t, const QVector<double> &key, const QVector<double> &value);
  void setData(const QVector<double> &key, const QVector<double> &value);
  void setScatterStyle(QCP::ScatterStyle style);
  void setScatterSize(double size);
  void setScatterPixmap(const QPixmap &pixmap);
  void setLineStyle(LineStyle style);
  
  // non-property methods:
  //void addData(const QCPCurveDataMap &dataMap);
  void addData(const QCPCurveData &data);
  void addData(double t, double key, double value);
  void addData(double key, double value);
  void addData(const QVector<double> &ts, const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double t);
  void removeDataAfter(double t);
  void removeData(double fromt, double tot);
  void removeData(double t);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  // drawing helpers:
  virtual void drawScatterPlot(QCPPainter *painter, const QVector<QPointF> *pointData) const;
  
  // helper functions:
  void getCurveData(QVector<QPointF> *lineData) const;
  double pointDistance(const QPointF &pixelPoint) const;

  QPointF outsideCoordsToPixels(double key, double value, int region) const;
  //virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  //virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
};


class  QCPBarData
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPBarData();
  QCPBarData(double key, double value);
  double key;
  double value;
};
typedef QMap<double, QCPBarData> QCPBarDataMap;
typedef QMapIterator<double, QCPBarData> QCPBarDataMapIterator;
typedef QMutableMapIterator<double, QCPBarData> QCPBarDataMutableMapIterator;


class  QCPBars : public QCPAbstractPlottable
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  explicit QCPBars(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPBars();
  
  // getters:
  double width() const;
  QCPBars *barBelow() const;
  QCPBars *barAbove() const;
  //const QCPBarDataMap *data() const;
  
  // setters:
  void setWidth(double width);
  //void setData(QCPBarDataMap *data, bool copy=false);
  void setData(const QVector<double> &key, const QVector<double> &value);
  
  // non-property methods:
  void moveBelow(QCPBars *bars);
  void moveAbove(QCPBars *bars);
  //void addData(const QCPBarDataMap &dataMap);
  void addData(const QCPBarData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  
  QPolygonF getBarPolygon(double key, double value) const;
  double getBaseValue(double key, bool positive) const;
  static void connectBars(QCPBars* lower, QCPBars* upper);
  //virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  //virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
};


class  QCPStatisticalBox : public QCPAbstractPlottable
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  explicit QCPStatisticalBox(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPStatisticalBox();
  
  // getters:
  double key() const;
  double minimum() const;
  double lowerQuartile() const;
  double median() const;
  double upperQuartile() const;
  double maximum() const;
  QVector<double> outliers() const;
  double width() const;
  double whiskerWidth() const;
  QPen whiskerPen() const;
  QPen whiskerBarPen() const;
  QPen medianPen() const;
  double outlierSize() const;
  QPen outlierPen() const;
  QCP::ScatterStyle outlierStyle() const;

  // setters:
  void setKey(double key);
  void setMinimum(double value);
  void setLowerQuartile(double value);
  void setMedian(double value);
  void setUpperQuartile(double value);
  void setMaximum(double value);
  void setOutliers(const QVector<double> &values);
  void setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum);
  void setWidth(double width);
  void setWhiskerWidth(double width);
  void setWhiskerPen(const QPen &pen);
  void setWhiskerBarPen(const QPen &pen);
  void setMedianPen(const QPen &pen);
  void setOutlierSize(double pixels);
  void setOutlierPen(const QPen &pen);
  void setOutlierStyle(QCP::ScatterStyle style);
  
  // non-property methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos) const;
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRect &rect) const;
  
  virtual void drawQuartileBox(QCPPainter *painter, QRectF *quartileBox=0) const;
  virtual void drawMedian(QCPPainter *painter) const;
  virtual void drawWhiskers(QCPPainter *painter) const;
  virtual void drawOutliers(QCPPainter *painter) const;
  //virtual QCPRange getKeyRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  //virtual QCPRange getValueRange(bool &validRange, SignDomain inSignDomain=sdBoth) const;
  
};


class  QCPItemStraightLine : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemStraightLine(QCustomPlot *parentPlot);
  virtual ~QCPItemStraightLine();
  
  // getters:
  QPen pen() const;
  QPen selectedPen() const;
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  //Removed const! for SIP
  QCPItemPosition *point1
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *point2
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  
  // helper functions:
  double distToStraightLine(const QVector2D &point1, const QVector2D &vec, const QVector2D &point) const;
  QLineF getRectClippedStraightLine(const QVector2D &point1, const QVector2D &vec, const QRect &rect) const;
  QPen mainPen() const;
};


class  QCPItemLine : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemLine(QCustomPlot *parentPlot);
  virtual ~QCPItemLine();
  
  // getters:
  QPen pen() const;
  QPen selectedPen() const;
  QCPLineEnding head() const;
  QCPLineEnding tail() const;
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const QCPLineEnding &head);
  void setTail(const QCPLineEnding &tail);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  //Removed const! for SIP
  QCPItemPosition *start
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *end
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  
  // helper functions:
  QLineF getRectClippedLine(const QVector2D &start, const QVector2D &end, const QRect &rect) const;
  QPen mainPen() const;
};


class  QCPItemCurve : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemCurve(QCustomPlot *parentPlot);
  virtual ~QCPItemCurve();
  
  // getters:
  QPen pen() const;
  QPen selectedPen() const;
  QCPLineEnding head() const;
  QCPLineEnding tail() const;
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setHead(const QCPLineEnding &head);
  void setTail(const QCPLineEnding &tail);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  //Removed const! for SIP
  QCPItemPosition *start
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *startDir
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *endDir
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *end
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  
  // helper functions:
  QPen mainPen() const;
};


class  QCPItemRect : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemRect(QCustomPlot *parentPlot);
  virtual ~QCPItemRect();
  
  // getters:
  QPen pen() const;
  QPen selectedPen() const;
  QBrush brush() const;
  QBrush selectedBrush() const;
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  //Removed const! for SIP
  QCPItemPosition *topLeft
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *bottomRight
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *top
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *topRight
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *right
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottom
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottomLeft
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *left
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class  QCPItemText : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemText(QCustomPlot *parentPlot);
  virtual ~QCPItemText();
  
  // getters:
  QColor color() const;
  QColor selectedColor() const;
  QPen pen() const;
  QPen selectedPen() const;
  QBrush brush() const;
  QBrush selectedBrush() const;
  QFont font() const;
  QFont selectedFont() const;
  QString text() const;
  Qt::Alignment positionAlignment() const;
  Qt::Alignment textAlignment() const;
  double rotation() const;
  QMargins padding() const;
  
  // setters;
  void setColor(const QColor &color);
  void setSelectedColor(const QColor &color);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setFont(const QFont &font);
  void setSelectedFont(const QFont &font);
  void setText(const QString &text);
  void setPositionAlignment(Qt::Alignment alignment);
  void setTextAlignment(Qt::Alignment alignment);
  void setRotation(double degrees);
  void setPadding(const QMargins &padding);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition *position
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *topLeft
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *top
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *topRight
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *right
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottomRight
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottom
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottomLeft
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *left
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPointF getTextDrawPoint(const QPointF &pos, const QRectF &rect, Qt::Alignment positionAlignment) const;
  QFont mainFont() const;
  QColor mainColor() const;
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class  QCPItemEllipse : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemEllipse(QCustomPlot *parentPlot);
  virtual ~QCPItemEllipse();
  
  // getters:
  QPen pen() const;
  QPen selectedPen() const;
  QBrush brush() const;
  QBrush selectedBrush() const;
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition *topLeft
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *bottomRight
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *topLeftRim
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *top
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *topRightRim
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *right
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottomRightRim
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottom
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottomLeftRim
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *left
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class  QCPItemPixmap : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  QCPItemPixmap(QCustomPlot *parentPlot);
  virtual ~QCPItemPixmap();
  
  // getters:
  QPixmap pixmap() const;
  bool scaled() const;
  Qt::AspectRatioMode aspectRatioMode() const;
  QPen pen() const;
  QPen selectedPen() const;
  
  // setters;
  void setPixmap(const QPixmap &pixmap);
  void setScaled(bool scaled, Qt::AspectRatioMode aspectRatioMode=Qt::KeepAspectRatio);
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition *topLeft
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *bottomRight
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *top
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *topRight
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *right
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottom
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *bottomLeft
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *left
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  void updateScaledPixmap(QRect finalRect=QRect(), bool flipHorz=false, bool flipVert=false);
  QRect getFinalRect(bool *flippedHorz=0, bool *flippedVert=0) const;
  QPen mainPen() const;
};


class  QCPItemTracer : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum TracerStyle { tsNone        ///< The tracer is not visible
                     ,tsPlus       ///< A plus shaped crosshair with limited size
                     ,tsCrosshair  ///< A plus shaped crosshair which spans the complete axis rect
                     ,tsCircle     ///< A circle
                     ,tsSquare     ///< A square
                   };

  QCPItemTracer(QCustomPlot *parentPlot);
  virtual ~QCPItemTracer();

  // getters:
  QPen pen() const;
  QPen selectedPen() const;
  QBrush brush() const;
  QBrush selectedBrush() const;
  double size() const;
  TracerStyle style() const;
  QCPGraph *graph() const;
  double graphKey() const;
  bool interpolating() const;

  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setBrush(const QBrush &brush);
  void setSelectedBrush(const QBrush &brush);
  void setSize(double size);
  void setStyle(TracerStyle style);
  void setGraph(QCPGraph *graph);
  void setGraphKey(double key);
  void setInterpolating(bool enabled);

  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  void updatePosition();

  QCPItemPosition *position
  {
      %SetCode
      // This is a *const, no setter
      %End
  };

protected:
  virtual void draw(QCPPainter *painter);

  // helper functions:
  QPen mainPen() const;
  QBrush mainBrush() const;
};


class  QCPItemBracket : public QCPAbstractItem
{
%TypeHeaderCode
#include "qcustomplot.h"
%End

public:
  enum BracketStyle { bsSquare  ///< A brace with angled edges
                      ,bsRound  ///< A brace with round edges
                      ,bsCurly  ///< A curly brace
                      ,bsCalligraphic ///< A curly brace with varying stroke width giving a calligraphic impression
  };

  QCPItemBracket(QCustomPlot *parentPlot);
  virtual ~QCPItemBracket();
  
  // getters:
  QPen pen() const;
  QPen selectedPen() const;
  double length() const;
  BracketStyle style() const;
  
  // setters;
  void setPen(const QPen &pen);
  void setSelectedPen(const QPen &pen);
  void setLength(double length);
  void setStyle(BracketStyle style);
  
  // non-property methods:
  virtual double selectTest(const QPointF &pos) const;
  
  QCPItemPosition *left
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemPosition *right
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  QCPItemAnchor *center
  {
      %SetCode
      // This is a *const, no setter
      %End
  };
  
protected:
  virtual void draw(QCPPainter *painter);
  virtual QPointF anchorPixelPoint(int anchorId) const;
  
  // helper functions:
  QPen mainPen() const;
};


