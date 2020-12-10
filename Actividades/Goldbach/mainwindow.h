#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>

class GoldbachCalculator;
class QProgressBar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

  protected:
    Ui::MainWindow *ui;
    QProgressBar* progressBar = nullptr;
    QTime time;
    GoldbachCalculator* goldbachCalculator = nullptr;

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    /**
     * @brief Start the calculations of sums for the given @a number
     * @param number The number given by user
     */
    void startCalculation(long long number);
    void updateProgressBar(int percent);

  private slots:
    void on_lineEditNumber_textEdited(const QString &arg1);
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();

  protected slots:
    void calculationDone(long long sumCount);
};

#endif // MAINWINDOW_H
