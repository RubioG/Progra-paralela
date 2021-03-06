#ifndef GOLDBACHTESTER_H
#define GOLDBACHTESTER_H

#include <QCoreApplication>
#include <QFileInfo>
#include <QMap>

class GoldbachCalculator;

class GoldbachTester : public QCoreApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(GoldbachTester)

  protected:
    QMap<GoldbachCalculator*, QFileInfo> calculators;
    long long cases=0;
    long long failed=0;
    long long passed=0;

  public:
    explicit GoldbachTester(int &argc, char **argv);
    int run();
    bool testDirectory(const QString& path);
    bool testFile(const QFileInfo& fileInfo);
    static QVector<QString> loadLines(const QFileInfo& fileInfo);

  protected slots:
    void calculationDone(long long sumCount);
};

#endif // GOLDBACHTESTER_H
