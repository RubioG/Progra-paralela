 #include <QtMath>

#include <QDebug>
#include "goldbachworker.h"

GoldbachWorker::GoldbachWorker(long long number, long long cota, long long inicio, long long indexWorker, QVector<QString>& results, QVector<bool> &cribaVector, QObject *parent)
    : QThread(parent)
    , number{number}
    , results{results}
    , cota{cota}
    , inicio{inicio}
    , indexWorker{indexWorker}
    , cribaVector{cribaVector}
{
}

void GoldbachWorker::run()
{
    #ifndef TESTING
    time.start();
    #endif
    long long sumCount = this->calculate(number);
    emit this->calculationDone( sumCount );
}

long long GoldbachWorker::calculate(long long number)
{
    if ( number < 4 || number == 5 ) return 0;
    this->onePercent = cota/100;
    return number % 2 == 0 ? calculateEvenGoldbach(number) : calculateOddGoldbach(number);
}

long long GoldbachWorker::calculateEvenGoldbach(long long number)
{
    long long cantResults = 0;
    for ( long long a = inicio; a <= cota; ++a, ++this->sumas )
    {
        if(sumas>=onePercent){
            sumas=0;
            emit this->percentangeCalculation((a + 1)* 101 / cota);
        }
        if ( ! isPrime(a) ) continue;
        long long b = number - a;
        if ( b >= a && isPrime(b) ){
            this->results.append( tr("%2 + %3").arg(a).arg(b) );
            ++cantResults;
        }
        // If user cancelled, stop calculations
        if ( this->isInterruptionRequested() ){
            return cantResults;
        }
    }
    #ifndef TESTING
    double seconds = time.elapsed() / 1000.0;
    QString message = QString("Hilo: %1 termino en %2 secods").arg(this->indexWorker).arg(seconds);
    qDebug(qPrintable(message));
    #endif
    return cantResults;
}

long long GoldbachWorker::calculateOddGoldbach(long long number)
{
    long long cantResults = 0;
    for ( long long a = inicio; a <= cota; ++a, ++this->sumas )
    {
        if(sumas>=onePercent){
            sumas=0;
            emit this->percentangeCalculation((a + 1)* 101 / cota);
        }
        if ( ! isPrime(a) ) continue;
        for ( long long b = a; b < number; ++b )
        {
            if ( ! isPrime(b) ) continue;
            long long c = number - a - b;
            if ( c >= b && isPrime(c) ){
                this->results.append( tr("%2 + %3 + %4").arg(a).arg(b).arg(c));
                ++cantResults;
            }
            // If user cancelled, stop calculations
            if ( this->isInterruptionRequested() ){
                return cantResults;
            }
        }
    }
    #ifndef TESTING
    double seconds = time.elapsed() / 1000.0;
    QString message = QString("Hilo: %1 termino en %2 secods").arg(this->indexWorker).arg(seconds);
    qDebug(qPrintable(message));
    #endif
    return cantResults;
}

bool GoldbachWorker::isPrime(long long number){
    return cribaVector[number];
}
