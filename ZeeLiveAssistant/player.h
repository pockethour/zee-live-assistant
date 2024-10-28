#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QCloseEvent>
namespace Ui {
class Player;
}

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = 0);
    ~Player();
signals:
    void stopRecord();
    void startRecord();
    void fillHoleTimesChanged(int value);
    void dilateTimesChanged(int value);
private:
    Ui::Player *ui;
protected:
	void closeEvent(QCloseEvent *event);
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

};

#endif // PLAYER_H
