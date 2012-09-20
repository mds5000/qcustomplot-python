/***************************************************************************
**                                                                        **
**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
**  Copyright (C) 2011, 2012 Emanuel Eichhammer                           **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
**             Date: 09.06.12                                             **
****************************************************************************/

#ifndef QCP_AXIS_H
#define QCP_AXIS_H

#include "global.h"
#include "range.h"
#include "layer.h"

class QCPPainter;
class QCustomPlot;
class QCPAxis;

class QCP_LIB_DECL QCPGrid : public QCPLayerable
{
  Q_OBJECT
public:
  QCPGrid(QCPAxis *parentAxis);
  ~QCPGrid();
  
  // getters:
  bool subGridVisible() const { return mSubGridVisible; }
  bool antialiasedSubGrid() const { return mAntialiasedSubGrid; }
  bool antialiasedZeroLine() const { return mAntialiasedZeroLine; }
  QPen pen() const { return mPen; }
  QPen subGridPen() const { return mSubGridPen; }
  QPen zeroLinePen() const { return mZeroLinePen; }
  QBrush sectionBrushEven() const { return mSectionBrushEven; }
  QBrush sectionBrushOdd() const { return mSectionBrushOdd; }
  
  // setters:
  void setSubGridVisible(bool visible);
  void setAntialiasedSubGrid(bool enabled);
  void setAntialiasedZeroLine(bool enabled);
  void setPen(const QPen &pen);
  void setSubGridPen(const QPen &pen);
  void setZeroLinePen(const QPen &pen);
  void setSectionBrushes(const QBrush &brushEven, const QBrush &brushOdd);
  
protected:
  QCPAxis *mParentAxis;
  bool mSubGridVisible;
  bool mAntialiasedSubGrid, mAntialiasedZeroLine;
  QPen mPen, mSubGridPen, mZeroLinePen;
  QBrush mSectionBrushEven, mSectionBrushOdd;
  
  virtual void applyDefaultAntialiasingHint(QCPPainter *painter) const;
  virtual void draw(QCPPainter *painter);
  // drawing helpers:
  void drawSections(QCPPainter *painter) const;
  void drawGridLines(QCPPainter *painter) const;
  void drawSubGridLines(QCPPainter *painter) const;
  
  friend class QCPAxis;
};


class QCP_LIB_DECL QCPAxis : public QCPLayerable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(AxisType axisType READ axisType WRITE setAxisType)
  Q_PROPERTY(ScaleType scaleType READ scaleType WRITE setScaleType)
  Q_PROPERTY(double scaleLogBase READ scaleLogBase WRITE setScaleLogBase)
  Q_PROPERTY(QRect axisRect READ axisRect WRITE setAxisRect)
  Q_PROPERTY(QCPRange range READ range WRITE setRange)
  Q_PROPERTY(bool grid READ grid WRITE setGrid)
  Q_PROPERTY(bool subGrid READ subGrid WRITE setSubGrid)
  Q_PROPERTY(bool autoTicks READ autoTicks WRITE setAutoTicks)
  Q_PROPERTY(int autoTickCount READ autoTickCount WRITE setAutoTickCount)
  Q_PROPERTY(bool autoTickLabels READ autoTickLabels WRITE setAutoTickLabels)
  Q_PROPERTY(bool autoTickStep READ autoTickStep WRITE setAutoTickStep)
  Q_PROPERTY(bool autoSubTicks READ autoSubTicks WRITE setAutoSubTicks)
  Q_PROPERTY(bool ticks READ ticks WRITE setTicks)
  Q_PROPERTY(bool tickLabels READ tickLabels WRITE setTickLabels)
  Q_PROPERTY(int tickLabelPadding READ tickLabelPadding WRITE setTickLabelPadding)
  Q_PROPERTY(LabelType tickLabelType READ tickLabelType WRITE setTickLabelType)
  Q_PROPERTY(QFont tickLabelFont READ tickLabelFont WRITE setTickLabelFont)
  Q_PROPERTY(double tickLabelRotation READ tickLabelRotation WRITE setTickLabelRotation)
  Q_PROPERTY(QString dateTimeFormat READ dateTimeFormat WRITE setDateTimeFormat)
  Q_PROPERTY(QString numberFormat READ numberFormat WRITE setNumberFormat)
  Q_PROPERTY(double tickStep READ tickStep WRITE setTickStep)
  Q_PROPERTY(QVector<double> tickVector READ tickVector WRITE setTickVector)
  Q_PROPERTY(QVector<QString> tickVectorLabels READ tickVectorLabels WRITE setTickVectorLabels)
  Q_PROPERTY(int subTickCount READ subTickCount WRITE setSubTickCount)
  Q_PROPERTY(QPen basePen READ basePen WRITE setBasePen)
  Q_PROPERTY(QPen gridPen READ gridPen WRITE setGridPen)
  Q_PROPERTY(QPen subGridPen READ subGridPen WRITE setSubGridPen)
  Q_PROPERTY(QPen tickPen READ tickPen WRITE setTickPen)
  Q_PROPERTY(QPen subTickPen READ subTickPen WRITE setSubTickPen)
  Q_PROPERTY(QFont labelFont READ labelFont WRITE setLabelFont)
  Q_PROPERTY(QString label READ label WRITE setLabel)
  Q_PROPERTY(int labelPadding READ labelPadding WRITE setLabelPadding)
  /// \endcond
public:
  /*!
    Defines at which side of the axis rect the axis will appear. This also affects how the tick
    marks are drawn, on which side the labels are placed etc.
    \see setAxisType
  */
  enum AxisType { atLeft    ///< Axis is vertical and on the left side of the axis rect of the parent QCustomPlot
                  ,atRight  ///< Axis is vertical and on the right side of the axis rect of the parent QCustomPlot
                  ,atTop    ///< Axis is horizontal and on the top side of the axis rect of the parent QCustomPlot
                  ,atBottom ///< Axis is horizontal and on the bottom side of the axis rect of the parent QCustomPlot
                };
  Q_ENUMS(AxisType)
  /*!
    When automatic tick label generation is enabled (\ref setAutoTickLabels), defines how the
    numerical value (coordinate) of the tick position is translated into a string that will be
    drawn at the tick position.
    \see setTickLabelType
  */
  enum LabelType { ltNumber    ///< Tick coordinate is regarded as normal number and will be displayed as such. (see \ref setNumberFormat)
                   ,ltDateTime ///< Tick coordinate is regarded as a date/time (seconds since 1970-01-01T00:00:00 UTC, see QDateTime::toTime_t) and will be displayed and formatted as such. (see \ref setDateTimeFormat)
                 };
  Q_ENUMS(LabelType)
  /*!
    Defines the scale of an axis.
    \see setScaleType
  */
  enum ScaleType { stLinear       ///< Normal linear scaling
                   ,stLogarithmic ///< Logarithmic scaling with correspondingly transformed plots and (major) tick marks at every base power (see \ref setScaleLogBase).
                 };
  Q_ENUMS(ScaleType)
  /*!
    Defines the selectable parts of an axis.
    \see setSelectable, setSelected
  */
  enum SelectablePart { spNone        = 0      ///< None of the selectable parts
                        ,spAxis       = 0x001  ///< The axis backbone and tick marks
                        ,spTickLabels = 0x002  ///< Tick labels (numbers) of this axis (as a whole, not individually)
                        ,spAxisLabel  = 0x004  ///< The axis label
                      };
  Q_ENUMS(SelectablePart)
  Q_DECLARE_FLAGS(SelectableParts, SelectablePart)
  
  explicit QCPAxis(QCustomPlot *parentPlot, AxisType type);
  virtual ~QCPAxis();
      
  // getters:
  AxisType axisType() const { return mAxisType; }
  QRect axisRect() const { return mAxisRect; }
  ScaleType scaleType() const { return mScaleType; }
  double scaleLogBase() const { return mScaleLogBase; }
  const QCPRange range() const { return mRange; }
  bool rangeReversed() const { return mRangeReversed; }
  bool antialiasedGrid() const { return mGrid->antialiased(); }
  bool antialiasedSubGrid() const { return mGrid->antialiasedSubGrid(); }
  bool antialiasedZeroLine() const { return mGrid->antialiasedZeroLine(); }
  bool grid() const { return mGrid->visible(); }
  bool subGrid() const { return mGrid->subGridVisible(); }
  bool autoTicks() const { return mAutoTicks; }
  int autoTickCount() const { return mAutoTickCount; }
  bool autoTickLabels() const { return mAutoTickLabels; }
  bool autoTickStep() const { return mAutoTickStep; }
  bool autoSubTicks() const { return mAutoSubTicks; }
  bool ticks() const { return mTicks; }
  bool tickLabels() const { return mTickLabels; }
  int tickLabelPadding() const { return mTickLabelPadding; }
  LabelType tickLabelType() const { return mTickLabelType; }
  QFont tickLabelFont() const { return mTickLabelFont; }
  QColor tickLabelColor() const { return mTickLabelColor; }
  double tickLabelRotation() const { return mTickLabelRotation; }
  QString dateTimeFormat() const { return mDateTimeFormat; }
  QString numberFormat() const;
  int numberPrecision() const { return mNumberPrecision; }
  double tickStep() const { return mTickStep; }
  QVector<double> tickVector() const { return mTickVector; }
  QVector<QString> tickVectorLabels() const { return mTickVectorLabels; }
  int tickLengthIn() const { return mTickLengthIn; }
  int tickLengthOut() const { return mTickLengthOut; }
  int subTickCount() const { return mSubTickCount; }
  int subTickLengthIn() const { return mSubTickLengthIn; }
  int subTickLengthOut() const { return mSubTickLengthOut; }
  QPen basePen() const { return mBasePen; }
  QPen gridPen() const { return mGrid->pen(); }
  QPen subGridPen() const { return mGrid->subGridPen(); }
  QPen zeroLinePen() const { return mGrid->zeroLinePen(); }
  QPen tickPen() const { return mTickPen; }
  QPen subTickPen() const { return mSubTickPen; }
  QFont labelFont() const { return mLabelFont; }
  QColor labelColor() const { return mLabelColor; }
  QString label() const { return mLabel; }
  int labelPadding() const { return mLabelPadding; }
  int padding() const { return mPadding; }
  SelectableParts selected() const { return mSelected; }
  SelectableParts selectable() const { return mSelectable; }
  QFont selectedTickLabelFont() const { return mSelectedTickLabelFont; }
  QFont selectedLabelFont() const { return mSelectedLabelFont; }
  QColor selectedTickLabelColor() const { return mSelectedTickLabelColor; }
  QColor selectedLabelColor() const { return mSelectedLabelColor; }
  QPen selectedBasePen() const { return mSelectedBasePen; }
  QPen selectedTickPen() const { return mSelectedTickPen; }
  QPen selectedSubTickPen() const { return mSelectedSubTickPen; }
  
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
  Qt::Orientation orientation() const { return mOrientation; }
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
  struct CachedLabel
  {
    QPointF offset;
    QPixmap pixmap;
  };
  struct TickLabelData
  {
    QString basePart, expPart;
    QRect baseBounds, expBounds, totalBounds, rotatedTotalBounds;
    QFont baseFont, expFont;
  };
  
  // simple properties with getters and setters:
  QVector<double> mTickVector;
  QVector<QString> mTickVectorLabels;
  QCPRange mRange;
  QString mDateTimeFormat;
  QString mLabel;
  QRect mAxisRect;
  QPen mBasePen, mTickPen, mSubTickPen;
  QFont mTickLabelFont, mLabelFont;
  QColor mTickLabelColor, mLabelColor;
  LabelType mTickLabelType;
  ScaleType mScaleType;
  AxisType mAxisType;
  double mTickStep;
  double mScaleLogBase, mScaleLogBaseLogInv;
  int mSubTickCount, mTickLengthIn, mTickLengthOut, mSubTickLengthIn, mSubTickLengthOut;
  int mAutoTickCount;
  int mTickLabelPadding, mLabelPadding, mPadding;
  double mTickLabelRotation;
  bool mTicks, mTickLabels, mAutoTicks, mAutoTickLabels, mAutoTickStep, mAutoSubTicks;
  bool mRangeReversed;
  SelectableParts mSelectable, mSelected;
  QFont mSelectedTickLabelFont, mSelectedLabelFont;
  QColor mSelectedTickLabelColor, mSelectedLabelColor;
  QPen mSelectedBasePen, mSelectedTickPen, mSelectedSubTickPen;
  QRect mAxisSelectionBox, mTickLabelsSelectionBox, mLabelSelectionBox;
  
  // internal or not explicitly exposed properties:
  QCPGrid *mGrid;
  QVector<double> mSubTickVector;
  QChar mExponentialChar, mPositiveSignChar;
  int mNumberPrecision;
  char mNumberFormatChar;
  bool mNumberBeautifulPowers, mNumberMultiplyCross;
  Qt::Orientation mOrientation;
  int mLowestVisibleTick, mHighestVisibleTick;
  QCache<QString, CachedLabel> mLabelCache;
  
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
  virtual void drawTickLabel(QCPPainter *painter, double x, double y, const TickLabelData &labelData) const;
  virtual TickLabelData getTickLabelData(const QFont &font, const QString &text) const;
  virtual QPointF getTickLabelDrawOffset(const TickLabelData &labelData) const;
  virtual void getMaxTickLabelSize(const QFont &font, const QString &text, QSize *tickLabelsSize) const;
  
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
  Q_DISABLE_COPY(QCPAxis)
  
  friend class QCustomPlot;
  friend class QCPGrid;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCPAxis::SelectableParts)

#endif // QCP_AXIS_H
