#include <iostream>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "goldbachcalculator.h"
#include "goldbachtester.h"

GoldbachTester::GoldbachTester(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
}

int GoldbachTester::run()
{
    if(this->arguments().count()>1){
        for ( int index = 1; index < this->arguments().count(); ++index ){
            this->testDirectory( this->arguments()[index] );
        }
    }
    else{
        QDir dir = QDir::current();
        dir.cd("..");
        dir.cd("Goldbach");

        QString message = QString("The current path is: %1").arg(dir.absolutePath());
        std::cout << message.toStdString() << std::endl;

        dir.setFilter(QDir::Dirs);
        QFileInfoList fileList = dir.entryInfoList();

        std::cout <<"Usage: \n"<< std::endl;
        QString dirs;
        for ( int index = 0; index < fileList.count(); ++index ){
            dirs=QString("%1").arg(fileList[index].baseName());
            if(dirs.length()>0){
                std::cout << "|  ";
                std::cout << fileList[index].baseName().toStdString();
                std::cout << "  |";
            }
        }

        std::cout <<"\nInsert the name of the folder "<< std::endl;
        QTextStream qtin(stdin);
        QString direx = qtin.readLine();
        QString path = QString("%1/%2").arg(dir.absolutePath()).arg(direx);
        this->testDirectory(path);

    }
    return this->exec();
}

bool GoldbachTester::testDirectory(const QString &path)
{

    QDir dir(path);
    if ( ! dir.exists() ){
        return std::cerr << "error: could not open directory: " << qPrintable(path) << std::endl, false;
    }

    dir.setFilter(QDir::Files);
    if(dir.count()>0){
        QFileInfoList fileList = dir.entryInfoList();
        for ( int index = 0; index < fileList.count(); ++index )
            this->testFile( fileList[index] );

        std::cout << path.toStdString() << std::endl;
    }
    else{
        return std::cerr << "error: test cases not found: " << qPrintable(path) << std::endl, false;
    }
    return true;
}

bool GoldbachTester::testFile(const QFileInfo &fileInfo)
{
    bool ok = true;
    long long number = fileInfo.baseName().toLongLong(&ok);
    if ( ! ok )
        return std::cerr << "error: invalid number: " << qPrintable(fileInfo.fileName()) << std::endl, false;

    GoldbachCalculator* goldbachCalculator = new GoldbachCalculator(this);
    this->calculators.insert(goldbachCalculator, fileInfo);
    this->connect( goldbachCalculator, &GoldbachCalculator::calculationDone, this, &GoldbachTester::calculationDone );
    goldbachCalculator->calculate(number);

    return true;
}

void GoldbachTester::calculationDone(long long sumCount)
{
    Q_UNUSED(sumCount);
    ++this->cases;

    GoldbachCalculator* goldbachCalculator = dynamic_cast<GoldbachCalculator*>( sender() );
    Q_ASSERT(goldbachCalculator);
    const QFileInfo& fileInfo = this->calculators.value( goldbachCalculator );
    const QVector<QString>& expectedSums = this->loadLines(fileInfo);
    const QVector<QString>& calculatorSums = goldbachCalculator->getAllSums();
    bool isFailed=false;
    for(int index = 0 ; index < calculatorSums.length() ; ++index){
        if(expectedSums.length()>index){
            if(calculatorSums[index] != expectedSums[index]){
                std::cout << "============================================================" << std::endl;
                QString message = QString("Test case %1 failed at line %2").arg(fileInfo.baseName()).arg(index+1);
                std::cout << message.toStdString() << std::endl;
                std::cout << "    expected: " << calculatorSums[index].toStdString()   << std::endl;
                std::cout << "    produced: " << expectedSums[index].toStdString()  << std::endl;
                isFailed=true;
                break;
            }
        }
        else{
            if(!isFailed){
                std::cout << "============================================================" << std::endl;
                QString message = QString("Test case %1 failed, more sums were calculated").arg(fileInfo.baseName());
                std::cout << message.toStdString() << std::endl;
                isFailed=true;
            }
            std::cout <<"    produced: "<< calculatorSums[index].toStdString()   << std::endl;
        }
    }
    if(!isFailed){
        ++this->passed;
    }
    else{
        std::cout << "============================================================" << std::endl;
        ++this->failed;
    }


    goldbachCalculator->deleteLater();
    this->calculators.remove( goldbachCalculator );
    if ( this->calculators.count() <= 0 ){
        std::cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        long long percentPassed=(this->passed*100)/this->cases;
        QString message=QString("%1 test cases: %2 passed (%3%), %4 failed (%5%).").arg(this->cases).arg(this->passed).arg(percentPassed).arg(this->failed).arg(100-percentPassed);
        std::cout << message.toStdString() << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        this->quit();
    }
}

QVector<QString> GoldbachTester::loadLines(const QFileInfo &fileInfo)
{
    QVector<QString> result;

    QFile file( fileInfo.absoluteFilePath() );
    if ( ! file.open(QFile::ReadOnly) )
        return std::cerr << "error: could not open: " << qPrintable(fileInfo.fileName()) << std::endl, result;

    QTextStream textStream( &file );
    while ( ! textStream.atEnd() ){
        QString message = QString(textStream.readLine());
        if(message.length()>0){
            result.append( message);
        }
    }

    return result;
}
