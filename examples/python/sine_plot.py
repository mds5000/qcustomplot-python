# sine_plot.py - A simple plot of a sine wave

from PyQt4.QtGui import QApplication
import qcustomplot as qcp

import sys
import math

def main():
    app = QApplication(sys.argv)
    plot = qcp.QCustomPlot()
    graph = plot.addGraph()

    x_data = range(1000)
    y_data = [math.sin(x*2*math.pi/1000) for x in x_data]
    graph.setData(x_data, y_data)

    plot.xAxis.setRange(0,1000)
    plot.yAxis.setRange(-2,2)

    plot.show()

    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
