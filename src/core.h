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

#ifndef QCP_CORE_H
#define QCP_CORE_H

#include "global.h"
#include "range.h"
#include "axis.h"
#include "legend.h"

class QCPPainter;
class QCPLayer;
class QCPAbstractPlottable;
class QCPAbstractItem;
class QCPGraph;

class QCP_LIB_DECL QCustomPlot : public QWidget
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(QString title READ title WRITE setTitle)
  Q_PROPERTY(QRect axisRect READ axisRect WRITE setAxisRect)
  Q_PROPERTY(int marginLeft READ marginLeft WRITE setMarginLeft)
  Q_PROPERTY(int marginRight READ marginRight WRITE setMarginRight)
  Q_PROPERTY(int marginTop READ marginTop WRITE setMarginTop)
  Q_PROPERTY(int marginBottom READ marginBottom WRITE setMarginBottom)
  Q_PROPERTY(int autoMargin READ autoMargin WRITE setAutoMargin)
  Q_PROPERTY(QColor color READ color WRITE setColor)
  Q_PROPERTY(Qt::Orientations rangeDrag READ rangeDrag WRITE setRangeDrag)
  Q_PROPERTY(Qt::Orientations rangeZoom READ rangeZoom WRITE setRangeZoom)
  /// \endcond
public:
  /*!
    Defines the mouse interactions possible with QCustomPlot
    
    \c Interactions is a flag of or-combined elements of this enum type.
    \see setInteractions, setInteraction
  */
  enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref setRangeDrag, \ref setRangeDragAxes)
                     ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref setRangeZoom, \ref setRangeZoomAxes)
                     ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref setMultiSelectModifier while clicking
                     ,iSelectTitle      = 0x008 ///< <tt>0x008</tt> The plot title is selectable
                     ,iSelectPlottables = 0x010 ///< <tt>0x010</tt> Plottables are selectable
                     ,iSelectAxes       = 0x020 ///< <tt>0x020</tt> Axes are selectable (or parts of them, see QCPAxis::setSelectable)
                     ,iSelectLegend     = 0x040 ///< <tt>0x040</tt> Legends are selectable (or their child items, see QCPLegend::setSelectable)
                     ,iSelectItems      = 0x080 ///< <tt>0x080</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref QCPAbstractItem)
                   };
  Q_ENUMS(Interaction)
  Q_DECLARE_FLAGS(Interactions, Interaction)
  /*!
    Defines how a layer should be inserted relative to a specified other layer.

    \see addLayer, moveLayer
  */
  enum LayerInsertMode { limBelow  ///< Layer is inserted below other layer
                         ,limAbove ///< Layer is inserted above other layer
                   };
  Q_ENUMS(LayerInsertMode)
  
  explicit QCustomPlot(QWidget *parent = 0);
  virtual ~QCustomPlot();
  
  // getters:
  QString title() const { return mTitle; }
  QFont titleFont() const { return mTitleFont; }
  QColor titleColor() const { return mTitleColor; }
  QRect axisRect() const { return mAxisRect; }
  QRect viewport() const { return mViewport; }
  int marginLeft() const { return mMarginLeft; }
  int marginRight() const { return mMarginRight; }
  int marginTop() const { return mMarginTop; }
  int marginBottom() const { return mMarginBottom; }
  bool autoMargin() const { return mAutoMargin; }
  QColor color() const { return mColor; }
  Qt::Orientations rangeDrag() const { return mRangeDrag; }
  Qt::Orientations rangeZoom() const { return mRangeZoom; }
  QCPAxis *rangeDragAxis(Qt::Orientation orientation);
  QCPAxis *rangeZoomAxis(Qt::Orientation orientation);
  double rangeZoomFactor(Qt::Orientation orientation);
  QCP::AntialiasedElements antialiasedElements() const { return mAntialiasedElements; }
  QCP::AntialiasedElements notAntialiasedElements() const { return mNotAntialiasedElements; }
  bool autoAddPlottableToLegend() const { return mAutoAddPlottableToLegend; }
  QPixmap axisBackground() const { return mAxisBackground; }
  bool axisBackgroundScaled() const { return mAxisBackgroundScaled; }
  Qt::AspectRatioMode axisBackgroundScaledMode() const { return mAxisBackgroundScaledMode; }
  const Interactions interactions() const { return mInteractions; }
  int selectionTolerance() const { return mSelectionTolerance; }
  QFont selectedTitleFont() const { return mSelectedTitleFont; }
  QColor selectedTitleColor() const { return mSelectedTitleColor; }
  bool titleSelected() const { return mTitleSelected; }
  bool noAntialiasingOnDrag() const { return mNoAntialiasingOnDrag; }
  QCP::PlottingHints plottingHints() const { return mPlottingHints; }
  Qt::KeyboardModifier multiSelectModifier() const { return mMultiSelectModifier; }

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
  
  QCPAxis *xAxis, *yAxis, *xAxis2, *yAxis2;
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
  QString mTitle;
  QFont mTitleFont, mSelectedTitleFont;
  QColor mTitleColor, mSelectedTitleColor;
  QRect mViewport;
  QRect mAxisRect;
  int mMarginLeft, mMarginRight, mMarginTop, mMarginBottom;
  bool mAutoMargin, mAutoAddPlottableToLegend;
  QColor mColor;
  QList<QCPAbstractPlottable*> mPlottables;
  QList<QCPGraph*> mGraphs; // extra list of items also in mPlottables that are of type QCPGraph
  QList<QCPAbstractItem*> mItems;
  QList<QCPLayer*> mLayers;
  Qt::Orientations mRangeDrag, mRangeZoom;
  QCPAxis *mRangeDragHorzAxis, *mRangeDragVertAxis, *mRangeZoomHorzAxis, *mRangeZoomVertAxis;
  double mRangeZoomFactorHorz, mRangeZoomFactorVert;
  bool mDragging;
  QCP::AntialiasedElements mAntialiasedElements, mNotAntialiasedElements;
  QPixmap mAxisBackground;
  bool mAxisBackgroundScaled;
  Qt::AspectRatioMode mAxisBackgroundScaledMode;
  Interactions mInteractions;
  int mSelectionTolerance;
  bool mTitleSelected;
  QRect mTitleBoundingBox;
  bool mNoAntialiasingOnDrag;
  // not explicitly exposed properties:
  QPixmap mPaintBuffer;
  QPoint mDragStart;
  QCPRange mDragStartHorzRange, mDragStartVertRange;
  QPixmap mScaledAxisBackground;
  bool mReplotting;
  QCP::AntialiasedElements mAADragBackup, mNotAADragBackup;
  QCPLayer *mCurrentLayer;
  QCP::PlottingHints mPlottingHints;
  Qt::KeyboardModifier mMultiSelectModifier;
  
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
  friend class QCPLegend;
  friend class QCPAxis;
  friend class QCPLayer;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QCustomPlot::Interactions)

#endif // QCP_CORE_H
