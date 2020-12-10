#ifndef GOLDBACHCALCULATOR_H
#define GOLDBACHCALCULATOR_H

#include <QAbstractListModel>
#include <QVector>

class GoldbachWorker;

class GoldbachCalculator : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(GoldbachCalculator)

protected:
    int fetchedRowCount = 0;
    long long totalSize=0;
    int countFinished= 0;
    long long sums=0;
    int totalPercent=0;
    QVector<GoldbachWorker *> goldbachWorkers;
    QVector<QVector<QString>> results;
    QVector<bool> cribaVector;
    void criba(long long number);

  public:
    explicit GoldbachCalculator(QObject *parent = nullptr);
    QVector<QString> getAllSums() const;
    void calculate(long long number);
    void stop();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    virtual bool canFetchMore(const QModelIndex &parent) const override;
    virtual void fetchMore(const QModelIndex &parent) override;  

signals:
    void calculationDone(long long sumCount);
    void updateProgressBar(long long percent);

  protected slots:
    void workerDone(/*int workerNumber,*/ long long sumCount);
    void updateProgressBarWorker(int percent);
};

#endif // GOLDBACHCALCULATOR_H
