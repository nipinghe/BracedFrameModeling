#include "deformwidget.h"
#include <qcustomplot/qcustomplot.h>
#include "resp.h"
#include <QLabel>

deformWidget::deformWidget(QString xLabel, QString yLabel, QWidget *parent)
              : QWidget(parent)
{
    // initialize
    steps = 2;
    size = 2;
    xi = new QVector<double>(size,0.);
    yi = new QVector<double>(size,0.);
    xj = new Resp(size,steps);
    yj = new Resp(size,steps);

    // setup plot
    thePlot = new QCustomPlot();
    thePlot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = 0.7*rec.height();
    thePlot->setMinimumHeight(0.2*height);
    thePlot->setMaximumHeight(0.2*height);
    thePlot->xAxis->setLabel(xLabel);
    thePlot->yAxis->setLabel(yLabel);

    // axes
    thePlot->xAxis2->setVisible(true);
    thePlot->yAxis2->setVisible(true);
    thePlot->xAxis->setTicks(false);
    thePlot->yAxis->setTicks(false);
    thePlot->xAxis->setSubTicks(false);
    thePlot->yAxis->setSubTicks(false);
    thePlot->xAxis2->setTickLabels(false);
    thePlot->yAxis2->setTickLabels(false);
    thePlot->xAxis2->setTicks(false);
    thePlot->yAxis2->setTicks(false);
    thePlot->xAxis2->setSubTicks(false);
    thePlot->yAxis2->setSubTicks(false);
    thePlot->axisRect()->setAutoMargins(QCP::msNone);
    thePlot->axisRect()->setMargins(QMargins(1,1,1,1));

    // print current value...
    label = new QLabel;

    // add to layout
    QGridLayout *Lay = new QGridLayout(this);
    Lay->addWidget(thePlot,0,0);
    Lay->addWidget(label,1,0);
    Lay->setMargin(0);
    this->setLayout(Lay);
}

deformWidget::~deformWidget()
{
    delete thePlot;
    delete graph;
    delete xi;
    delete xj;
    delete yi;
    delete yj;
}

void deformWidget::setModel(QVector<double> *data_x, QVector<double> *data_y)
{
    size = data_x->size();

    // initialize
    xi->resize(size);
    yi->resize(size);
    xj->reSize(size,steps);
    yj->reSize(size,steps);

    // set
    xi = data_x;
    yi = data_y;

    // max -X
    maxX = 0.;
    minX = 0.;
    for (int j=0; j < size; j++) {
        double val = (*xi)[j];
        if (val > maxX)
            maxX = val;
        if (val < minX)
            minX = val;
    }

    // max -Y
    maxY = 0.;
    minY = 0.;
    for (int j=0; j < size; j++) {
        double val = (*yi)[j];
        if (val > maxY)
            maxY = val;
        if (val < minY)
            minY = val;
    }
}

void deformWidget::setResp(Resp *data_x, Resp *data_y)
{
    //size = data_x->size;
    steps = data_x->steps;

    // add to coordinates
    xj->reSize(size,steps);
    yj->reSize(size,steps);

    for (int t=0; t<steps; t++)
        for (int j=0; j<size; j++)
        {
            (*xj->data[j])[t] = (*xi)[j] + (*data_x->data[j])[t];
            (*yj->data[j])[t] = (*yi)[j] + (*data_y->data[j])[t];
        }

    // max -X
    for (int t=0; t<steps; t++) {
        for (int j=0; j < size; j++) {
            double val = (*xj->data[j])[t];
            if (val > maxX)
                maxX = val;
            if (val < minX)
                minX = val;
        }
    }

    // max -Y
    for (int t=0; t<steps; t++) {
        for (int j=0; j < size; j++) {
            double val = (*yj->data[j])[t];
            if (val > maxY)
                maxY = val;
            if (val < minY)
                minY = val;
        }
    }
}

void deformWidget::plotModel()
{
    thePlot->clearPlottables();
    thePlot->clearGraphs();
    //thePlot->clearItems();
    // add graph
    graph = thePlot->addGraph();

    // create pen
    QPen pen;
    pen.setWidthF(3);

    // line color
    pen.setColor(QColor(Qt::black));
    thePlot->graph(0)->setPen(pen);
    thePlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7));

    // set data
    thePlot->graph(0)->setData(*xi,*yi,true);

    // axes
    thePlot->xAxis->setRange(minX-10,maxX+10);
    thePlot->yAxis->setRange(minY-1,maxY+1);

    // update plot
    thePlot->replot();
    thePlot->update();

    // update label
    label->setText(QString("current = %1 in.").arg(maxY,0,'f',2));
}

void deformWidget::plotResponse(int t)
{
    // setup system plot
    thePlot->clearPlottables();
    thePlot->clearGraphs();

    // add graph
    graph = thePlot->addGraph();
    // create pen
    QPen pen;
    pen.setWidthF(3);
    // line color
    pen.setColor(QColor(Qt::black));
    thePlot->graph(0)->setPen(pen);
    thePlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 7));

    // set data
    QVector<double> *xt = new QVector<double>(size,0.);
    QVector<double> *yt = new QVector<double>(size,0.);
    double maxYt = 0.;
    for (int j=0; j<size; j++) {
        (*xt)[j] = (*xj->data[j])[t];
        (*yt)[j] = (*yj->data[j])[t];
        if (fabs((*yt)[j]) > fabs(maxYt))
            maxYt = (*yt)[j];
    }
    thePlot->graph(0)->setData(*xt,*yt,true);

    // axes
    thePlot->xAxis->setRange(minX-10,maxX+10);
    thePlot->yAxis->setRange(minY-1,maxY+1);

    // update plot
    thePlot->replot();
    thePlot->update();

    // update label
    label->setText(QString("current = %1 in.").arg(maxYt,0,'f',2));

    delete xt;
    delete yt;
}
