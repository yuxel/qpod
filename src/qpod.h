#ifndef QPOD_H
#define QPOD_H
#include <QProcess>
#include <QAction>
#include <QIcon>
#include <QTranslator>
#include <QSettings>

#include "ui_anaPencere.h"
 
class qPodSecenekMenusu;

class qPod : public QMainWindow, private Ui::anaPencere
{
    Q_OBJECT
 
public:
qPod(QMainWindow *parent = 0);
QTranslator translator;
void boyutDegistir(int x, int y);
void ayarlariOku();
void dilDegistir();
void guvenliKaldir( QCloseEvent *event);
QString aygit,baglamaNoktasi;
QString gnupod_tunes2pod, gnupod_addsong, gnupod_search, gnupod_check, gnupod_mktunes; //komutlar
QProcess islem;
QSettings *secenekler;
bool baglandi,cikarildi;
QWidget *ayarlarTab, *ipodTab,*yuklenecekSarkilarTab;
int seciliRow;


public slots:
bool gnupodYollariniKontrolEt();
void baglan();
void ayarlariKapat();
void ayarlariKaydet();
void ayarlarOntanimli();
void ayarlarYardim();
void ipodSarkiListesiAl();
void silButtonEtkin();
void sarkilariSil();
void sarkilariYukle();
void ayarlar();
void sarkiSec();
void siraDegistir();
void siralamayiYaz();


protected:
 void dragEnterEvent(QDragEnterEvent *event);
     void dragMoveEvent(QDragMoveEvent *event);
     void dropEvent(QDropEvent *event);
     void closeEvent ( QCloseEvent * event );
};


 
#endif 
