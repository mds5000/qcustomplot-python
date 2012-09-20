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
/*! \file */
#ifndef QCP_GLOBAL_H
#define QCP_GLOBAL_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QVector>
#include <QString>
#include <QPrinter>
#include <QDateTime>
#include <QMultiMap>
#include <QFlags>
#include <QDebug>
#include <QVector2D>
#include <QStack>
#include <QCache>
#include <qmath.h>
#include <limits>

// decl definitions for shared library compilation/usage:
#if defined(QCUSTOMPLOT_COMPILE_LIBRARY)
#  define QCP_LIB_DECL Q_DECL_EXPORT
#elif defined(QCUSTOMPLOT_USE_LIBRARY)
#  define QCP_LIB_DECL Q_DECL_IMPORT
#else
#  define QCP_LIB_DECL
#endif

/*!
  The QCP Namespace contains general enums and QFlags 
*/
namespace QCP
{
/*!
  Defines the symbol used for scatter points.
  
  On plottables/items that draw scatters, the sizes of these visualizations (with exception of \ref
  QCP::ssDot and \ref QCP::ssPixmap) can be controlled with a \a setScatterSize function. Scatters
  are in general drawn with the main pen set on the plottable/item.
  
  \see QCPGraph::setScatterStyle, QCPStatisticalBox::setOutlierStyle
*/
enum ScatterStyle { ssNone       ///< \enumimage{ssNone.png} no scatter symbols are drawn (e.g. in QCPGraph, data only represented with lines)
                    ,ssDot       ///< \enumimage{ssDot.png} a single pixel (use \ref ssDisc if you want a round shape with a certain radius)
                    ,ssCross     ///< \enumimage{ssCross.png} a cross
                    ,ssPlus      ///< \enumimage{ssPlus.png} a plus
                    ,ssCircle    ///< \enumimage{ssCircle.png} a circle which is not filled
                    ,ssDisc      ///< \enumimage{ssDisc.png} a circle which is filled with the color of the pen (not the brush!)
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

/*!
  Defines what elements of a plot can be forcibly drawn antialiased/not antialiased. If an
  element is neither forcibly drawn antialiased nor forcibly drawn not antialiased, it is up to
  the respective element how it is drawn. Typically it provides a \a setAntialiased function for
  this.
  
  \c AntialiasedElements is a flag of or-combined elements of this enum type.
  
  \see QCustomPlot::setAntialiasedElements, QCustomPlot::setNotAntialiasedElements
*/
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
Q_DECLARE_FLAGS(AntialiasedElements, AntialiasedElement)

/*!
  Defines plotting hints that control various aspects of the quality and speed of plotting.
  \see QCustomPlot::setPlottingHints
*/
enum PlottingHint { phNone            = 0x000 ///< <tt>0x000</tt> No hints are set
                    ,phFastPolylines  = 0x001 ///< <tt>0x001</tt> Graph/Curve lines are drawn with a faster method. This reduces the quality
                                              ///<                especially of the line segment joins. (Only used for solid line pens.)
                    ,phForceRepaint   = 0x002 ///< <tt>0x002</tt> causes an immediate repaint() instead of a soft update() when QCustomPlot::replot() is called. This is set by default
                                              ///<                on Windows-Systems to prevent the plot from freezing on fast consecutive replots (e.g. user drags ranges with mouse).
                    ,phCacheLabels    = 0x004 ///< <tt>0x004</tt> axis (tick) labels will be cached as pixmaps, increasing replot performance.
                  };
Q_DECLARE_FLAGS(PlottingHints, PlottingHint)
} // end of namespace QCP
Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::AntialiasedElements)
Q_DECLARE_OPERATORS_FOR_FLAGS(QCP::PlottingHints)

#endif // QCP_GLOBAL_H
