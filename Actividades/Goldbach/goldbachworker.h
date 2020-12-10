#ifndef GOLDBACHWORKER_H
#define GOLDBACHWORKER_H

#include <QTime>
#include <QThread>
#include <QVector>

class GoldbachWorker : public QThread
{
    Q_OBJECT

  protected:
    long long number = 0;
    long long cota=0;
    long long indexWorker=0;
    long long inicio=0;
    long long sumas=0;
    long long onePercent=0;
    #ifndef TESTING
    QTime time;
    #endif
    QVector<QString>& results;
    QVector<bool>& cribaVector;

  public:
    explicit GoldbachWorker(long long number, long long cota, long long inicio, long long indexWorker, QVector<QString>& results, QVector<bool>& cribaVector,  QObject *parent = nullptr);

  protected:
    void run() override;

  signals:
    void calculationDone(long long sumCount);
    void percentangeCalculation(int percent);

  public slots:

  protected:
    /**
    * @brief Calcula las sumas de Goldbach para el numero dado y las agrega a una pizarra
    * @param number El numero dado por el usuario
    * @return La cantidad de sumas encontradas
    */
    long long calculate(long long number);
    /**
    * Calcula todas las sumas de dos primos que equivalen al numero dado, y las presenta
    * en el area resultado. Incrementa la barra de progreso mientras realiza el calculo
    * de la conjetura fuerte de Goldbach, por ejemplo:
    *
    *   4 == 2 + 2
    *   6 == 3 + 3
    *  10 == 3 + 7 == 5 + 5
    *
    * @param numero Un numero entero par mayor o igual a 4
    * @return la cantidad de sumas de dos primos encontradas
    */
    long long calculateEvenGoldbach(long long number);
    /**
    * Calcula todas las sumas de tres primos que equivalen al numero dado, y las presenta
    * en el area resultado. Incrementa la barra de progreso mientras realiza el calculo
    * de la conjetura debil de Goldbach, por ejemplo:
    *
    *   7 == 2 + 2 + 3
    *   9 == 2 + 2 + 5 == 3 + 3 + 3
    *
    * @param numero Un numero entero impar mayor o igual a 7
    * @return la cantidad de sumas de tres primos encontradas
    */
    long long calculateOddGoldbach(long long number);
    /**
    * Retorna true si numero es primo, false si numero no es primo o menor que 2
    * Por definicion 1 no es primo ni compuesto, este metodo retorna false
    */
    bool isPrime(long long numero);
};

#endif // GOLDBACHWORKER_H
