#include "goldbachcalculator.h"
#include "goldbachworker.h"

#include <QDebug>
#include <QVector>

GoldbachCalculator::GoldbachCalculator(QObject *parent)
    : QAbstractListModel(parent)
{
}

void GoldbachCalculator::calculate(long long number)
{
    this->beginResetModel();

    int ideal = QThread::idealThreadCount() - 1;
    if(number<500){
        ideal=1;
    }

    //Establezco una cota para a, de tal modo que se tiene que calcular menos.
    long long cota;
    (number % 2 == 0) ? cota=(number/2): cota=(number/2.8);

    //relleno el vector de booleanos de criba para que cada hilo lo use y reduzca su tiempo de ejecucion
    criba(number);

    //inicializo el vector de vectores en donde cada hilo almacenara los datos
    for(int index=0; index<ideal ;++index){
        this->results.append(QVector<QString>());
    }

    long long partition = cota/ideal;
    long long actualPartition=0;
    long long restPartition=cota;
    long long tempoPartition=partition;


    for ( int current = 0; current < ideal ; ++current) {
        //si aun puedo repartir de manera equitativa a los hilos entra en el if, es decir, a cada hilo le toca la misma cantidad de trabajo menos al ultimo
        if(restPartition-partition >= partition){
            this->goldbachWorkers.append(new GoldbachWorker(number, tempoPartition, actualPartition+1, current, this->results[current], this->cribaVector, this));
            actualPartition += partition;
            tempoPartition += partition;
            restPartition -= partition;
        }
        else{ //le envia al ultimo hilo el resto de la particion
            this->goldbachWorkers.append(new GoldbachWorker(number, cota+1, actualPartition+1, current, this->results[current], this->cribaVector, this));
            restPartition=0;
        }

        //establezco los connects para que cada hilo se comunique y envie los datos y la barra de progreso
        this->connect( this->goldbachWorkers.at(current), &GoldbachWorker::percentangeCalculation, this, &GoldbachCalculator::updateProgressBarWorker );
        this->connect( this->goldbachWorkers.at(current), &GoldbachWorker::calculationDone, this, &GoldbachCalculator::workerDone );

        //el hilo empieza a trabajar
        this->goldbachWorkers.at(current)->start();

        //si ya gaste la particion es mejor que se salga y no continue con el ciclo for
        if(restPartition == 0){
            break;
        }
    }
    //datos para la list en donde se van a visualizar las sumas
    this->fetchedRowCount = 0;
    this->endResetModel();

}

void GoldbachCalculator::stop()
{
    //recorre los hilos y si encuentra a uno que aun esta trabajando lo interrumpe
    for ( int current = 0; current <= this->results.count() ; ++current){
        Q_ASSERT(  this->goldbachWorkers[current]);
        if(this->goldbachWorkers[current]->isRunning()){
            this->goldbachWorkers[current]->requestInterruption();
        }
    }
}

int GoldbachCalculator::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->fetchedRowCount;
}

QVariant GoldbachCalculator::data(const QModelIndex &index, int role) const
{

    //variable auxiliar para calcular en que vector de vectores esta el dato que busca
    long long indexA = index.row();

    int actualIndex=0;
    int acumulado= this->results[actualIndex].count();
    while(indexA >= acumulado){
        actualIndex++;
        acumulado += this->results[actualIndex].count();
    }

    acumulado-=this->results[actualIndex].count();
    int buscado = indexA - acumulado;

    if ( ! index.isValid() ){
        return QVariant();
    }

    if ( buscado < 0 || buscado >= this->results[actualIndex].count() ){
        return QVariant();
    }

    if ( role == Qt::DisplayRole ){
        QString message= QString("%1: %2").arg(index.row()+1).arg(this->results[actualIndex].at(buscado));
        return message;
    }

    return QVariant();
}

bool GoldbachCalculator::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->fetchedRowCount < this->totalSize+1;

}

void GoldbachCalculator::fetchMore(const QModelIndex &parent)
{
	Q_UNUSED(parent);

    this->totalSize=0;
    for (int index = 0 ;  index < results.count() ; ++index ){
        this->totalSize += results[index].count();
    }

    int remainder = totalSize - this->fetchedRowCount;
    int itemsToFetch = qMin(100, remainder);

    if (itemsToFetch <= 0)
        return;

    int firstRow = this->fetchedRowCount;
    int lastRow = this->fetchedRowCount + itemsToFetch - 1;

    beginInsertRows(QModelIndex(), firstRow, lastRow);
    this->fetchedRowCount += itemsToFetch;
    endInsertRows();
}

void GoldbachCalculator::workerDone(long long sumCount)
{
    this->sums += sumCount;
    //Si ya llegue al final, termino la ejecucion de los hilos
    if(this->countFinished==(this->results.count()-1)){
        emit this->calculationDone(this->sums);
        for (int current = 0 ;  current < results.count() ; ++current){
            this->goldbachWorkers[current]->deleteLater();
            this->goldbachWorkers[current]= nullptr;
        }
    }
    ++countFinished;
}

void GoldbachCalculator::updateProgressBarWorker(int percent){
    this->totalPercent+=percent/this->results.count();
    emit this->updateProgressBar(totalPercent);
}


QVector<QString> GoldbachCalculator::getAllSums() const
{
    QVector<QString> allSums;
    QString num;
    long long counter=1;
    //recorro el vecttor de vectores y agrego todos los datos en un solo vector para realizar las pruebas de correctitud.
    for(int index=0; index < this->results.count() ; ++index){
        for(int subIndex=0; subIndex < this->results[index].count() ; ++subIndex){
            QString num = QString("%1: %2").arg(counter++).arg(results[index][subIndex]);
            allSums.append(num);
        }
    }
    return allSums;
}

void GoldbachCalculator::criba(long long number){

    //algoritmo de criba, cada posicion representa al numero.
    //0 y 1 no son primos
    cribaVector.append(false);
    cribaVector.append(false);

    //tomo como principio que todos son primos hasta que se demuestre lo contrario
    for(long long index = 2; index <= number; ++index) {
        cribaVector.append(true);
    }

    //recorro las posiciones pares
    for(int index = 2; index*index <= number; ++index) {
        //si la posicion es primo
        if(cribaVector[index]) {
            //recorro y hago falso los multiplos ya que no son primos
            for(int multiple = 2; (index * multiple) <= number; ++multiple){
                cribaVector[index*multiple] = false;
            }
        }
    }
}
