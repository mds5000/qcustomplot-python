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

#include "painter.h"

// ================================================================================
// =================== QCPPainter
// ================================================================================

/*! \class QCPPainter
  \brief QPainter subclass used internally
  
  This internal class is used to provide some extended functionality e.g. for tweaking position
  consistency between antialiased and non-antialiased painting and drawing common shapes (like
  scatter symbols). Further it provides workarounds for QPainter quirks.
  
  \warning This class intentionally hides non-virtual functions of QPainter, e.g. setPen, save and
  restore. So while it is possible to pass a QCPPainter instance to a function that expects a
  QPainter pointer, some of the workarounds and tweaks will be unavailable to the function (because
  it will call the base class implementations of the functions actually hidden by QCPPainter).
*/

/*!
  Creates a new QCPPainter instance and sets default values
*/
QCPPainter::QCPPainter() :
  QPainter(),
  mScaledExportMode(false),
  mPdfExportMode(false),
  mIsAntialiasing(false)
{
}

/*!
  Creates a new QCPPainter instance on the specified paint \a device and sets default values. Just
  like the analogous QPainter constructor, begins painting on \a device immediately.
*/
QCPPainter::QCPPainter(QPaintDevice *device) :
  QPainter(device),
  mScaledExportMode(false),
  mPdfExportMode(false),
  mIsAntialiasing(false)
{
}

QCPPainter::~QCPPainter()
{
}

/*!
  Sets the pixmap that will be used to draw scatters with \ref drawScatter, when the style is
  QCP::ssPixmap.
*/
void QCPPainter::setScatterPixmap(const QPixmap pm)
{
  mScatterPixmap = pm;
}

/*!
  Sets the pen of the painter and applies certain fixes to it, depending on the mode of this
  QCPPainter.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::setPen(const QPen &pen)
{
  QPainter::setPen(pen);
  if (mScaledExportMode)
    fixScaledPen();
}

/*! \overload
  
  Sets the pen (by color) of the painter and applies certain fixes to it, depending on the mode of
  this QCPPainter.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::setPen(const QColor &color)
{
  QPainter::setPen(color);
  if (mScaledExportMode)
    fixScaledPen();
}

/*! \overload
  
  Sets the pen (by style) of the painter and applies certain fixes to it, depending on the mode of
  this QCPPainter.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::setPen(Qt::PenStyle penStyle)
{
  QPainter::setPen(penStyle);
  if (mScaledExportMode)
    fixScaledPen();
}

/*! \overload
  
  Works around a Qt bug introduced with Qt 4.8 which makes drawing QLineF unpredictable when
  antialiasing is disabled.
  
  \note this function hides the non-virtual base class implementation.
*/
void QCPPainter::drawLine(const QLineF &line)
{
  if (mIsAntialiasing)
    QPainter::drawLine(line);
  else
    QPainter::drawLine(line.toLine());
}

/*! 
  Sets whether painting uses antialiasing or not. Use this method instead of using setRenderHint
  with QPainter::Antialiasing directly, as it allows QCPPainter to regain pixel exactness between
  antialiased and non-antialiased painting (Since Qt uses slightly different coordinate systems for
  AA/Non-AA painting).
*/
void QCPPainter::setAntialiasing(bool enabled)
{
  if (mPdfExportMode)
    return;
  
  setRenderHint(QPainter::Antialiasing, enabled);
  if (mIsAntialiasing != enabled)
  {
    if (mIsAntialiasing)
      translate(-0.5, -0.5);
    else
      translate(0.5, 0.5);
    mIsAntialiasing = enabled;
  }
}

/*!
  Saves the painter (see QPainter::save). Since QCPPainter adds some new internal state to
  QPainter, the save/restore functions are reimplemented to also save/restore those members.
  
  \note this function hides the non-virtual base class implementation.
  
  \see restore
*/
void QCPPainter::save()
{
  mAntialiasingStack.push(mIsAntialiasing);
  QPainter::save();
}

/*!
  Restores the painter (see QPainter::restore). Since QCPPainter adds some new internal state to
  QPainter, the save/restore functions are reimplemented to also save/restore those members.
  
  \note this function hides the non-virtual base class implementation.
  
  \see save
*/
void QCPPainter::restore()
{
  if (!mAntialiasingStack.isEmpty())
    mIsAntialiasing = mAntialiasingStack.pop();
  else
    qDebug() << Q_FUNC_INFO << "Unbalanced save/restore";
  QPainter::restore();
}

/*!
  Sets whether the painter shall adjust its fixes/workarounds optimized for vectorized pdf export.

  This means for example, that the antialiasing/non-antialiasing fix introduced with \ref
  setAntialiasing is not used, since PDF is not rastered and thus works with floating point data
  natively.
*/
void QCPPainter::setPdfExportMode(bool enabled)
{
  mPdfExportMode = enabled;
}

/*!
  Sets whether the painter shall adjust its fixes/workarounds optimized for scaled export to
  rastered image formats.

  For example this provides a workaround for a QPainter bug that prevents scaling of pen widths for
  pens with width 0, although the QPainter::NonCosmeticDefaultPen render hint is set.
*/
void QCPPainter::setScaledExportMode(bool enabled)
{
  mScaledExportMode = enabled;
}

/*!
  Provides a workaround for a QPainter bug that prevents scaling of pen widths for pens with width
  0, although the QPainter::NonCosmeticDefaultPen render hint is set.
  
  Changes the pen width from 0 to 1, if appropriate.
  
  Does nothing if the QCPPainter is not in scaled export mode (\ref setScaledExportMode).
*/
void QCPPainter::fixScaledPen()
{
  if (mScaledExportMode && pen().isCosmetic() && qFuzzyIsNull(pen().widthF()))
  {
    QPen p = pen();
    p.setWidth(1);
    QPainter::setPen(p);
  }
}

/*! 
  Draws a single scatter point with the specified \a style and \a size in pixels at the pixel position \a x and \a y.
  
  If the \a style is ssPixmap, make sure to pass the respective pixmap with \ref setScatterPixmap before calling
  this function.
*/
void QCPPainter::drawScatter(double x, double y, double size, QCP::ScatterStyle style)
{
  double w = size/2.0;
  switch (style)
  {
    case QCP::ssNone: break;
    case QCP::ssDot:
    {
      drawPoint(QPointF(x, y));
      break;
    }
    case QCP::ssCross:
    {
      drawLine(QLineF(x-w, y-w, x+w, y+w));
      drawLine(QLineF(x-w, y+w, x+w, y-w));
      break;
    }
    case QCP::ssPlus:
    {
      drawLine(QLineF(x-w, y, x+w, y));
      drawLine(QLineF(x, y+w, x, y-w));
      break;
    }
    case QCP::ssCircle:
    {
      setBrush(Qt::NoBrush);
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssDisc:
    {
      setBrush(QBrush(pen().color()));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssSquare:
    {
      setBrush(Qt::NoBrush);
      drawRect(QRectF(x-w, y-w, size, size));
      break;
    }
    case QCP::ssDiamond:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y, x, y-w));
      drawLine(QLineF(x, y-w, x+w, y));
      drawLine(QLineF(x+w, y, x, y+w));
      drawLine(QLineF(x, y+w, x-w, y));
      break;
    }
    case QCP::ssStar:
    {
      drawLine(QLineF(x-w, y, x+w, y));
      drawLine(QLineF(x, y+w, x, y-w));
      drawLine(QLineF(x-w*0.707, y-w*0.707, x+w*0.707, y+w*0.707));
      drawLine(QLineF(x-w*0.707, y+w*0.707, x+w*0.707, y-w*0.707));
      break;
    }
    case QCP::ssTriangle:
    {
      drawLine(QLineF(x-w, y+0.755*w, x+w, y+0.755*w));
      drawLine(QLineF(x+w, y+0.755*w, x, y-0.977*w));
      drawLine(QLineF(x, y-0.977*w, x-w, y+0.755*w));
      break;
    }
    case QCP::ssTriangleInverted:
    {
      drawLine(QLineF(x-w, y-0.755*w, x+w, y-0.755*w));
      drawLine(QLineF(x+w, y-0.755*w, x, y+0.977*w));
      drawLine(QLineF(x, y+0.977*w, x-w, y-0.755*w));
      break;
    }
    case QCP::ssCrossSquare:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y-w, x+w*0.95, y+w*0.95));
      drawLine(QLineF(x-w, y+w*0.95, x+w*0.95, y-w));
      drawRect(QRectF(x-w,y-w,size,size));
      break;
    }
    case QCP::ssPlusSquare:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y, x+w*0.95, y));
      drawLine(QLineF(x, y+w, x, y-w));
      drawRect(QRectF(x-w, y-w, size, size));
      break;
    }
    case QCP::ssCrossCircle:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w*0.707, y-w*0.707, x+w*0.67, y+w*0.67));
      drawLine(QLineF(x-w*0.707, y+w*0.67, x+w*0.67, y-w*0.707));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssPlusCircle:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x-w, y, x+w, y));
      drawLine(QLineF(x, y+w, x, y-w));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssPeace:
    {
      setBrush(Qt::NoBrush);
      drawLine(QLineF(x, y-w, x, y+w));
      drawLine(QLineF(x, y, x-w*0.707, y+w*0.707));
      drawLine(QLineF(x, y, x+w*0.707, y+w*0.707));
      drawEllipse(QPointF(x,y), w, w);
      break;
    }
    case QCP::ssPixmap:
    {
      drawPixmap(x-mScatterPixmap.width()*0.5, y-mScatterPixmap.height()*0.5, mScatterPixmap);
      // if something in here is changed, adapt QCP::ssPixmap special case in drawLegendIcon(), too
      break;
    }
  }
}
