#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    this->setWindowTitle("opengl player");
    connect(this,&Player::stopRecord,ui->openGLWidget,&XVideoWidget::stopRecord);
    connect(this,&Player::startRecord,ui->openGLWidget,&XVideoWidget::startRecord);
}

Player::~Player()
{
    delete ui;
}

void Player::closeEvent(QCloseEvent *event)
{
	qApp->quit();
}
void Player::on_pushButton_2_clicked()
{
    emit stopRecord();
}

void Player::on_pushButton_clicked()
{
    emit startRecord();
}
