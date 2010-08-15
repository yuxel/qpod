
#include <QtGui>
#include <QBrush>
#include <QDir>
#include <QDesktopWidget>


#include "qpod.h"


qPod::qPod(QMainWindow *parent)
{
	qApp->installTranslator(&translator);
	//mevcut dizini belirle
	secenekler = new QSettings("qPod");
	QDir::setCurrent(QCoreApplication::applicationDirPath());
	dilDegistir();
	setupUi(this);


	//ust resim
	widget->setStyleSheet("background-image:url('resimler/ust.jpg');");

	baglandi=false;
	cikarildi=true;


	setAcceptDrops(true);

	//tablari sakla
	ayarlarTab=tabWidget->widget(2);
	ipodTab=tabWidget->widget(1);
	yuklenecekSarkilarTab=tabWidget->widget(0);
	yuklenecekListe->setAcceptDrops(true);
	tabWidget->removeTab(0);
	tabWidget->removeTab(0);
	tabWidget->removeTab(0);
	silProgressBar->hide();
	sarkiProgress->hide();
	tabWidget->hide();

	//ortada goster
	QDesktopWidget* masaustu=new QDesktopWidget();
	setGeometry ( masaustu->width()/2-200, masaustu->height()/2-100, 400, 80);
	boyutDegistir(400,80);


	connect(baglanButton, SIGNAL(clicked()), this, SLOT(baglan()));
	connect(aygitlarOntanimliButton, SIGNAL(clicked()), this, SLOT(ayarlarOntanimli()));
	connect(gnupodAyarlarOntanimliButton, SIGNAL(clicked()), this, SLOT(ayarlarOntanimli()));
	connect(gnupodAyarlarUygula, SIGNAL(clicked()), this, SLOT(ayarlariKaydet()));
	connect(aygitlarUygulaButton, SIGNAL(clicked()), this, SLOT(ayarlariKaydet()));
	connect(dilUygulaButton, SIGNAL(clicked()), this, SLOT(ayarlariKaydet()));
	connect(ayarlarYardimButton, SIGNAL(clicked()), this, SLOT(ayarlarYardim()));
	connect(ayarlariKapatButton, SIGNAL(clicked()), this, SLOT(ayarlariKapat()));
	connect(sarkilariSilButton, SIGNAL(clicked()), this, SLOT(sarkilariSil()));
	connect(sarkiSecButton, SIGNAL(clicked()), this, SLOT(sarkiSec()));
	connect(sarkilariYukleButton, SIGNAL(clicked()), this, SLOT(sarkilariYukle()));
	connect(ayarButton, SIGNAL(clicked()), this, SLOT(ayarlar()));
	connect(yukariAlButton, SIGNAL(clicked()), this, SLOT(siraDegistir()));
	connect(asagiAlButton, SIGNAL(clicked()), this, SLOT(siraDegistir()));
	//acilista gnudodAyarlarini Kontrol et
	gnupodYollariniKontrolEt();
}

void qPod::boyutDegistir(int x, int y)
{	//verilen x ve y icin boyut degistir
	setMaximumSize(x,y);
	setMinimumSize(x,y);
}


void qPod::baglan() //bağlan ve bağlantıyı kes
{	if(baglandi!=true){
	QStringList hataMesaji;
	if (!secenekler->contains("aygit")) hataMesaji << trUtf8("Bir aygıt belirtilmemiş");
	if (!secenekler->contains("baglamaNoktasi")) hataMesaji << trUtf8("Bir bağlama noktası belirtilmemiş");
	if(hataMesaji.size()==0) {
		aygit=secenekler->value("aygit").toString();
		baglamaNoktasi=secenekler->value("baglamaNoktasi").toString();
		QStringList argumanlar;
		argumanlar << aygit << baglamaNoktasi;
		islem.start("mount",argumanlar);
		islem.waitForFinished();
		if(islem.exitCode()!=0 ){
			QByteArray readAllStandardError=islem.readAllStandardError();
			QString metin=readAllStandardError;

			if(!metin.contains("already mounted")) hataMesaji  << trUtf8("Bağlantı sırasında bir hata oluştu, lütfen ayarlarınız kontrol edin");
		}
		if(hataMesaji.size()==0) {
			QStringList argumanlar;
			argumanlar << "-m" << baglamaNoktasi;
			islem.start(gnupod_check,argumanlar);
			islem.waitForFinished();

			if(islem.exitCode()!=0){
				QMessageBox msgBox;
				msgBox.setTextFormat(Qt::RichText);
				//QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
				//QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
				QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
				QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır"), QMessageBox::ActionRole);
				msgBox.setIcon(QMessageBox::Critical);
				msgBox.setWindowTitle(trUtf8("HATA"));
				msgBox.setText(trUtf8("iPod'unuzu daha önce qPod ile kullanmamış görünyorsunuz.<br/>Sizin için gerekli dosyaları yüklememi ister misiniz?"));
				msgBox.exec();


				if (msgBox.clickedButton() == evetButton) {
				//gnupod_INIT'i emule et
				QStringList dizinler;
				dizinler << "iPod_Control" << "iPod_Control/Music" << "iPod_Control/iTunes" <<"iPod_Control/.gnupod" << "iPod_Control/Music/f00" << "iPod_Control/Music/f01"<< "iPod_Control/Music/f02"<< "iPod_Control/Music/f03"<< "iPod_Control/Music/f04"<< "iPod_Control/Music/f05"<< "iPod_Control/Music/f06"<< "iPod_Control/Music/f07"<< "iPod_Control/Music/f08"<< "iPod_Control/Music/f09"<< "iPod_Control/Music/f10"<< "iPod_Control/Music/f11"<< "iPod_Control/Music/f12"<< "iPod_Control/Music/f13"<< "iPod_Control/Music/f14"<< "iPod_Control/Music/f15"<< "iPod_Control/Music/f16"<< "iPod_Control/Music/f17"<< "iPod_Control/Music/f18"<< "iPod_Control/Music/f19";
				QDir dizinOlustur;
				dizinOlustur.cd(baglamaNoktasi);
				for(int i=0;i<dizinler.size();i++){
				dizinOlustur.mkdir(dizinler.at(i));
				}

				if(QFile::exists (baglamaNoktasi+"/iPod_Control/iTunes/iTunesDB" )) {
				QStringList argumanlar;
				argumanlar << "--force" << "-m" << baglamaNoktasi;
				islem.start(gnupod_tunes2pod,argumanlar);
				islem.waitForFinished();
				if(islem.exitCode()==0) baglan();
				}
				else{
				
				QFile file(baglamaNoktasi+"/iPod_Control/.gnupod/GNUtunesDB.xml"); //bosXML olustur
 				if (file.open(QIODevice::WriteOnly)) {
     				QTextStream stream(&file);
     				stream << "<?xml version='1.0' standalone='yes'?>\n<gnuPod>\n<files>\n</files>\n</gnuPod>";
     				}
				file.close();
				
				
				QStringList argumanlar;
				argumanlar << "-m" << baglamaNoktasi;
				islem.start(gnupod_mktunes,argumanlar);
				islem.waitForFinished();
				if(islem.exitCode()==0) baglan();
				}


				





				} else if (msgBox.clickedButton() == hayirButton) {
					qWarning("hataVar");
				}

			}
			else {
				baglandi=true;
				tabWidget->removeTab(0);
				tabWidget->removeTab(0);
				tabWidget->removeTab(0);
				ayarButton->setDisabled(TRUE);
				baglanButton->setIcon(QIcon::QIcon ( "resimler/ayir.png"));
				labelBaglan->setText(trUtf8("Bağlantıyı kes"));
				tabWidget->show();
				tabWidget->insertTab(tabWidget->count(),ipodTab,trUtf8("iPod"));
				tabWidget->insertTab(tabWidget->count(),yuklenecekSarkilarTab,trUtf8("Yüklenecek Şarkılar"));
				boyutDegistir(400,335);
				ipodSarkiListesiAl();
				cikarildi=false;
			}
		}


	}

	if(hataMesaji.size()>0) {


		QString metin;
		metin="<ul>";
		for(int i=0;i<hataMesaji.size();i++){
			metin+="<li>"+hataMesaji.at(i)+"</li>";
		}
		metin+="</ul>";


		QMessageBox msgBox;
		msgBox.setTextFormat(Qt::RichText);
		QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
		//QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
		//QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
		//QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır), QMessageBox::ActionRole);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(trUtf8("HATA"));
		msgBox.setText(metin);
		msgBox.exec();

	}
}
else
{//baglanti kes
	siralamayiYaz();
	QStringList argumanlar;
	argumanlar << aygit;
	islem.start("umount",argumanlar);
	islem.waitForFinished();

	if(islem.exitCode()==0) { //basarili ise
		ayarButton->setDisabled(FALSE);
		baglanButton->setIcon(QIcon::QIcon ( "resimler/baglan.png"));
		labelBaglan->setText(trUtf8("Bağlan"));
		baglandi=false;
		tabWidget->removeTab(0);
		tabWidget->removeTab(0);
		tabWidget->hide();
		boyutDegistir(400,80);
		
	}
	else { //degilse sonucu ekrana yaz

		QByteArray readAllStandardError=islem.readAllStandardError();
		QString metin=readAllStandardError;
		metin.replace("\n","<br/>");
		QMessageBox msgBox;
		msgBox.setTextFormat(Qt::RichText);
		QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
		//QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
		//QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
		//QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır), QMessageBox::ActionRole);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(trUtf8("HATA"));
		msgBox.setText(trUtf8("Bağlantının kesilmesi sırasında bir hata oluştu<br/><b>Hata:</b><br/>")+metin);
		msgBox.exec();

	}
}
}

bool qPod::gnupodYollariniKontrolEt()
{
	//gnupod yollarında bir hata veya eksiklik var mi?
	QString metin;
	QStringList hataMesaji;
	//QSettings ayarlari dogru mu?
	if (!secenekler->contains("gnupod_tunes2pod")) hataMesaji << trUtf8("tunes2pod yolu eksik");
	if (!secenekler->contains("gnupod_search")) hataMesaji << trUtf8("gnupod_search yolu eksik");
	if (!secenekler->contains("gnupod_addsong")) hataMesaji << trUtf8("gnupod_addsong yolu eksik");
	if (!secenekler->contains("gnupod_check")) hataMesaji << trUtf8("gnupod_check yolu eksik");
	if (!secenekler->contains("gnupod_mktunes")) hataMesaji << trUtf8("mktunes yolu eksik");

	if (hataMesaji.size()==0)
	{	//ayarlar doğru olabilir ancak dosyalar mevcut mu?
		QDir dosyaVarMi;
		if (!dosyaVarMi.exists (secenekler->value("gnupod_tunes2pod").toString())) hataMesaji << trUtf8("tunes2pod yolu yanlış");
		if (!dosyaVarMi.exists (secenekler->value("gnupod_search").toString())) hataMesaji << trUtf8("gnupod_search yolu yanlış");
		if (!dosyaVarMi.exists (secenekler->value("gnupod_addsong").toString())) hataMesaji << trUtf8("gnupod_addsong yolu yanlış");
		if (!dosyaVarMi.exists (secenekler->value("gnupod_check").toString())) hataMesaji << trUtf8("gnupod_check yolu yanlış");
		if (!dosyaVarMi.exists (secenekler->value("gnupod_mktunes").toString())) hataMesaji << trUtf8("mktunes yolu yanlış");
	}
	if (hataMesaji.size()==0) {//hata yoksa
		gnupod_tunes2pod=secenekler->value("gnupod_tunes2pod").toString();
		gnupod_addsong=secenekler->value("gnupod_addsong").toString();
		gnupod_check=secenekler->value("gnupod_check").toString();
		gnupod_search=secenekler->value("gnupod_search").toString();
		gnupod_mktunes=secenekler->value("gnupod_mktunes").toString();

		return true;
	}
	else
	{    //hata varsa
		metin="<ol>";
		for (int i=0;i<hataMesaji.size();i++)
		{
			metin+="<li>"+hataMesaji.at(i)+"</li>";
		}
		metin+="</ol>";

		//bir uyarı göster
		QMessageBox msgBox;
		msgBox.setTextFormat(Qt::RichText);
		QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
		//QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
		//QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
		//QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır), QMessageBox::ActionRole);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(trUtf8("HATA"));
		msgBox.setText(trUtf8("Lütfen ayarlar menüsünden aşağıdaki hataları düzeltin.")+metin);
		msgBox.exec();

		//ayarlar sekmesini göster
		baglanButton->setDisabled(TRUE);
		tabWidget->show();
		tabWidget->insertTab(tabWidget->count(),ayarlarTab,trUtf8("Ayarlar"));
		ayarlariOku();
		boyutDegistir(400,335);

		baglanButton->setDisabled(TRUE);

		tabWidgetAyarlar->setCurrentIndex(1);
		ayarlariKapatButton->hide();

		return false;
	}
}



void qPod::ayarlariKaydet()
{
	//lineEdit'lerden gelen yolları kaydet ve doğruluğunu denetle
	if(tabWidgetAyarlar->currentIndex()==0) {
		secenekler->setValue("aygit",lineAygit->text());
		secenekler->setValue("baglamaNoktasi",lineBaglamaNoktasi->text());

	}
	//lineEdit'lerden gelen yolları kaydet ve doğruluğunu denetle
	else if(tabWidgetAyarlar->currentIndex()==1) {
		secenekler->setValue("gnupod_tunes2pod",lineGnupod_INIT->text());
		secenekler->setValue("gnupod_search",lineGnupod_search->text());
		secenekler->setValue("gnupod_check",lineGnupod_check->text());
		secenekler->setValue("gnupod_addsong",lineGnupod_addsong->text());
		secenekler->setValue("gnupod_mktunes",lineGnupod_mktunes->text());

		if (gnupodYollariniKontrolEt()==true) {
			ayarlariKapatButton->show();
			baglanButton->setDisabled(FALSE);
		}
	}

	//comboBox'i ayarla
	else if(tabWidgetAyarlar->currentIndex()==2) {
		if(dilComboBox->currentIndex()==0) secenekler->setValue("dil","en");
		else if(dilComboBox->currentIndex()==1) secenekler->setValue("dil","tr");
		else if(dilComboBox->currentIndex()==2) secenekler->setValue("dil","de");

		QMessageBox msgBox;
		msgBox.setTextFormat(Qt::RichText);
		QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
		//QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
		//QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
		//QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır"), QMessageBox::ActionRole);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(trUtf8("HATA"));
		msgBox.setText(trUtf8("Yaptığınız dil değişikliği uygulama yeniden başlatılınca etkin olacaktır"));
		msgBox.exec();
	}
}


void qPod::ayarlarOntanimli()
{
	//lineEdit'leri öntanımlı değerlere döndür
	if(tabWidgetAyarlar->currentIndex()==0) {
		lineAygit->setText("/dev/sdb");
		lineBaglamaNoktasi->setText("/mnt/ipod");

	}
	else if(tabWidgetAyarlar->currentIndex()==1) {
		lineGnupod_INIT->setText("/usr/bin/tunes2pod");
		lineGnupod_search->setText("/usr/bin/gnupod_search");
		lineGnupod_addsong->setText("/usr/bin/gnupod_addsong");
		lineGnupod_check->setText("/usr/bin/gnupod_check");
		lineGnupod_mktunes->setText("/usr/bin/mktunes");
	}
}


void qPod::ayarlariOku()
{	//QSettings ayarlarını lineEdit'lere yaz
	lineGnupod_INIT->setText(secenekler->value("gnupod_tunes2pod").toString());
	lineGnupod_search->setText(secenekler->value("gnupod_search").toString());
	lineGnupod_addsong->setText(secenekler->value("gnupod_addsong").toString());
	lineGnupod_check->setText(secenekler->value("gnupod_check").toString());
	lineGnupod_mktunes->setText(secenekler->value("gnupod_mktunes").toString());

	lineAygit->setText(secenekler->value("aygit").toString());
	lineBaglamaNoktasi->setText(secenekler->value("baglamaNoktasi").toString());


	if(secenekler->value("dil").toString()=="en") dilComboBox->setCurrentIndex(0);
	else if(secenekler->value("dil").toString()=="tr") dilComboBox->setCurrentIndex(1);
	else if(secenekler->value("dil").toString()=="de") dilComboBox->setCurrentIndex(2);
	else dilComboBox->setCurrentIndex(0);

}

void qPod::ayarlariKapat(){
	//ayarlar Tab'ini kapat
	boyutDegistir(400,80);
	tabWidget->removeTab(tabWidget->currentIndex());
	baglanButton->setDisabled(FALSE);
}


void qPod::ayarlarYardim(){
	//buraya yardım gelecek ilerde
	if(tabWidgetAyarlar->currentIndex()==0) qWarning("Aygıtlar");
	else if (tabWidgetAyarlar->currentIndex()==1) qWarning("gnupod");
	else if (tabWidgetAyarlar->currentIndex()==2) qWarning("dil");
}


void qPod::ipodSarkiListesiAl(){
	QStringList argumanlar;
	argumanlar << "*" << "-m" << baglamaNoktasi;
	islem.start(gnupod_search,argumanlar);
	islem.waitForFinished();
	QByteArray sarkilar=islem.readAll();
	QString sarkilarS=QString::fromUtf8( sarkilar );
	QStringList sarkilarL=sarkilarS.split("=\n"); //= ile biten yerden ayir
	sarkilarL=sarkilarL.at(1).split("\n"); //satır satır ayır
	
	//tablewidget'ı oluştur
	ipodListe->setRowCount(sarkilarL.count()-1);
	ipodListe->setColumnCount(4);
	ipodListe->setColumnHidden(3,TRUE);
	ipodListe->setSelectionBehavior(QAbstractItemView::SelectRows);
	ipodListe->setColumnWidth ( 0, 170 );
	ipodListe->setColumnWidth ( 1, 70);
	ipodListe->setColumnWidth ( 2, 70 );
	

	//seçim olursa silme etkinleşsin
	connect(ipodListe, SIGNAL(itemSelectionChanged ()), this, SLOT(silButtonEtkin()));

	//tablo başlıkları
	QTableWidgetItem *baslikSarki = new QTableWidgetItem(trUtf8("Şarkı"));
	QTableWidgetItem *baslikSarkici = new QTableWidgetItem(trUtf8("Şarkıcı"));
	QTableWidgetItem *baslikAlbum = new QTableWidgetItem(trUtf8("Albüm"));
	QTableWidgetItem *baslikID = new QTableWidgetItem(trUtf8("#"));

	ipodListe->setHorizontalHeaderItem(0,baslikSarki);
	ipodListe->setHorizontalHeaderItem(1,baslikSarkici);
	ipodListe->setHorizontalHeaderItem(2,baslikAlbum);
	ipodListe->setHorizontalHeaderItem(3,baslikID);

	ipodListe->horizontalHeader()->setResizeMode(3, QHeaderView::ResizeToContents); //id'leri gizle



	int row = 0;
	for(int i=0;i<sarkilarL.count()-1;i++){
		//tablo içeriğini doldur
		QStringList sarkiAyristir=sarkilarL.at(i).split("|");

		QTableWidgetItem *itemSarkici = new QTableWidgetItem(i);
		QTableWidgetItem *itemAlbum= new QTableWidgetItem(i);
		QTableWidgetItem *itemSarki = new QTableWidgetItem(i);
		QTableWidgetItem *itemID = new QTableWidgetItem(i);


		itemSarki->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		itemSarkici->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		itemAlbum->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		itemID->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

		itemID->setText(sarkiAyristir.at(0).simplified());
		ipodListe->setItem(row, 3, itemID);

		itemSarki->setText(sarkiAyristir.at(3).simplified());
		ipodListe->setItem(row, 0, itemSarki);

		itemSarkici->setText(sarkiAyristir.at(1).simplified());
		itemSarkici->setSizeHint(QSize::QSize(200,10));
		ipodListe->setItem(row, 1, itemSarkici);

		itemAlbum->setText(sarkiAyristir.at(2).simplified());
		ipodListe->setItem(row, 2, itemAlbum);

		ipodListe->setRowHeight(row,18);
		row++;

	}

}


void qPod::silButtonEtkin(){
	//sil butonunu etkinleştir
	//QPushButton slot'u çalışmadı ondan böyle bir şey yaptım


	QList<QTableWidgetItem *> seciliOgeler=ipodListe->selectedItems();

	if(seciliOgeler.size()>1) sarkilariSilButton->setDisabled(FALSE);
	else sarkilariSilButton->setDisabled(TRUE);


	if(seciliOgeler.size()==3) {
	QList<QTableWidgetItem *> seciliOgeler=ipodListe->selectedItems();

	QStringList silinecekler;
	for (int i = 0; i < seciliOgeler.size(); i++) {
	QString seciliOge=QString::number(seciliOgeler.at(i)->row());
	if(silinecekler.contains(seciliOge) == FALSE) silinecekler << seciliOge;
	}
	seciliRow=silinecekler.at(0).toInt();
	if(seciliRow>0) yukariAlButton->setDisabled(FALSE);
	else yukariAlButton->setDisabled(TRUE);

	if(seciliRow<ipodListe->rowCount()-1) asagiAlButton->setDisabled(FALSE);
	else asagiAlButton->setDisabled(TRUE);
	
	}
	
	else {
	yukariAlButton->setDisabled(TRUE);
	asagiAlButton->setDisabled(TRUE);
	
	}

	
}


void qPod::sarkilariSil() {
	//secili ögeleri al
	QList<QTableWidgetItem *> seciliOgeler=ipodListe->selectedItems();

	QMessageBox msgBox;
	msgBox.setTextFormat(Qt::RichText);
	//QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
	//QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
	QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
	QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır"), QMessageBox::ActionRole);
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle(trUtf8("Şarkı Silmek"));
	msgBox.setText(trUtf8("Seçili şarkıları iPod'unuzdan silmek istediğinize emin misiniz?"));
	msgBox.exec();




	if (msgBox.clickedButton() == evetButton) {
		//silme onaylandiysa 
		silProgressBar->show();
		ipodListe->setDisabled(TRUE);

		QStringList silinecekler;
		for (int i = 0; i < seciliOgeler.size(); i++) {

			QString seciliOge=QString::number(seciliOgeler.at(i)->row());
			if(silinecekler.contains(seciliOge) == FALSE) silinecekler << seciliOge;

		}


		for (int i = 0; i < silinecekler.size(); i++) {
			double yuzde=(i+1)*100/silinecekler.size();

			QString yuzdeString=QString::number(yuzde);

			QStringList argumanlar;
			argumanlar << "-m" << baglamaNoktasi << "--delete" << "-i" << "^"+ipodListe->item(silinecekler.at(i).toInt(),3)->text()+"$" ;
			islem.start(gnupod_search,argumanlar);
			islem.waitForFinished();
			
			silProgressBar->setValue(yuzdeString.toInt());
			//silme tamamlandi
		}




		QStringList argumanlar2;
		argumanlar2 << "-m" << baglamaNoktasi;
		islem.start(gnupod_mktunes,argumanlar2);
		islem.waitForFinished();
		silProgressBar->hide();
		ipodListe->setDisabled(FALSE) ;

		ipodSarkiListesiAl();

		sarkilariSilButton->setDisabled(TRUE);
		ipodListe->clearSelection();
		//mktunes tamamlandı
	}

}





//Drag drop olayları
void qPod::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();

}

void qPod::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
}

void qPod::dropEvent(QDropEvent *event)
{
	if(baglandi==true) {

		const QMimeData *mimeData = event->mimeData();

		if (mimeData->hasUrls()) { //mp3 dosyalarını ekle
			//ilerde klasorlerin icine de bakacak
			QList<QUrl> urlList = mimeData->urls();
			QString text;
			for (int i = 0; i < urlList.size() && i < 32; i++) {
				QString url = urlList.at(i).path();
				if(url.contains(".mp3")) yuklenecekListe->addItem(url);
			}

		}

		if(yuklenecekListe->count()>0) {
			tabWidget->setCurrentIndex(1);
			sarkilariYukleButton->setDisabled(FALSE);

		}

	}

	event->acceptProposedAction();
}


void qPod::sarkilariYukle(){
	//yuklenecek listedeki şarkıları ipod'a at
	sarkiProgress->show();
	yuklenecekListe->setDisabled(TRUE);

	for(int i=0;i<yuklenecekListe->count();i++){
		double yuzde=(i+1)*100/yuklenecekListe->count();

		QString yuzdeString=QString::number(yuzde);

		QStringList arguman;
		arguman<< "-m" << baglamaNoktasi;
		arguman<<yuklenecekListe->item(i)->text();
		islem.start(gnupod_addsong,arguman);

		islem.waitForFinished();
		sarkiProgress->setValue(yuzdeString.toInt());


	}

	QStringList argumanlar2;
	argumanlar2 << "-m" << baglamaNoktasi;
	islem.start(gnupod_mktunes,argumanlar2);
	islem.waitForFinished();
	silProgressBar->hide();
	ipodListe->setDisabled(FALSE) ;

	sarkiProgress->hide();
	yuklenecekListe->setDisabled(FALSE);
	yuklenecekListe->clear();
	ipodSarkiListesiAl();
	tabWidget->setCurrentIndex(0);
	sarkilariYukleButton->setDisabled(TRUE);
	//mktunes'u tamamla
}

void qPod::ayarlar(){
	//ayarlar penceresini göster
	tabWidget->insertTab(tabWidget->count(),ayarlarTab,trUtf8("Ayarlar"));
	baglanButton->setDisabled(TRUE);
	ayarlariOku(); //ayarları oku
	boyutDegistir(400,335);
	tabWidget->show();

}


void qPod::dilDegistir(){
	//dil değiştir
	if(secenekler->value("dil").toString()=="tr") translator.load("ceviri/qpod_tr");
	else if(secenekler->value("dil").toString()=="en") translator.load("ceviri/qpod_en");
	else if(secenekler->value("dil").toString()=="de") translator.load("ceviri/qpod_de");
	else translator.load("ceviri/qpod_en");
}

void qPod::sarkiSec(){
	//file dialog aç, mp3 dosyası seçtir
	QStringList files = QFileDialog::getOpenFileNames(
	this,
	trUtf8("Mp3 dosyaları seçin"),
	"/home",
	"Mp3 (*.mp3)");

	sarkilariYukleButton->setDisabled(FALSE);
	yuklenecekListe->addItems(files);
	//secilen dosyalari listeye ekle
}


void qPod::closeEvent ( QCloseEvent * event ){
	if(baglandi==true) {
		//eger baglanti kapanmamışsa
		QMessageBox msgBox;
		msgBox.setTextFormat(Qt::RichText);
		//QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
		QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
		QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır"), QMessageBox::ActionRole);
		QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(trUtf8("HATA"));
		msgBox.setText(trUtf8("Bağlantıyı kesmeyi unuttunuz!<br/>Sizin yerinize bağlantıyı kesmemi ister misiniz?"));
		msgBox.exec();

		if (msgBox.clickedButton() == evetButton) {
			baglan(); //baglantiyi kes
		}
		else if(msgBox.clickedButton() == iptalButton) {
			event->ignore(); //kapatmayi iptal et
		}
	}

	if(cikarildi==false && baglandi==false){
		//ipod guvenli kaldirilsin mi?
		guvenliKaldir(event);
	}

}



void qPod::guvenliKaldir( QCloseEvent *event){
	QMessageBox msgBox;
	msgBox.setTextFormat(Qt::RichText);
	//QPushButton *tamamButton= msgBox.addButton(trUtf8("Tamam"), QMessageBox::ActionRole);
	QPushButton *evetButton= msgBox.addButton(trUtf8("Evet"), QMessageBox::ActionRole);
	QPushButton *hayirButton= msgBox.addButton(trUtf8("Hayır"), QMessageBox::ActionRole);
	QPushButton *iptalButton= msgBox.addButton(trUtf8("İptal"), QMessageBox::ActionRole);
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setWindowTitle(trUtf8("Güvenli Kaldır"));
	msgBox.setText(trUtf8("iPod'unuzu güvenli kaldırmak ister misiniz?"));
	msgBox.exec();


	if (msgBox.clickedButton() == evetButton) {
		QStringList argumanlar;
		argumanlar << aygit;
		islem.start("eject",argumanlar);
		islem.waitForFinished();
		cikarildi=true;
		//guvenli kaldır
	}

	else if(msgBox.clickedButton() == iptalButton) {
		event->ignore(); //iptal et
	}

}


void qPod::siraDegistir(){
int seciliRowSwitch;
if(sender()==yukariAlButton) seciliRowSwitch=seciliRow-1;
else if(sender()==asagiAlButton) seciliRowSwitch=seciliRow+1;



QTableWidgetItem *seciliRowUst1=ipodListe->takeItem(seciliRowSwitch,0);
QTableWidgetItem *seciliRowUst2=ipodListe->takeItem(seciliRowSwitch,1);
QTableWidgetItem *seciliRowUst3=ipodListe->takeItem(seciliRowSwitch,2);
QTableWidgetItem *seciliRowUst4=ipodListe->takeItem(seciliRowSwitch,3);


QTableWidgetItem *seciliRow1=ipodListe->takeItem(seciliRow,0);
QTableWidgetItem *seciliRow2=ipodListe->takeItem(seciliRow,1);
QTableWidgetItem *seciliRow3=ipodListe->takeItem(seciliRow,2);
QTableWidgetItem *seciliRow4=ipodListe->takeItem(seciliRow,3);




ipodListe->setItem(seciliRow,0,seciliRowUst1);
ipodListe->setItem(seciliRow,1,seciliRowUst2);
ipodListe->setItem(seciliRow,2,seciliRowUst3);
ipodListe->setItem(seciliRow,3,seciliRowUst4);




ipodListe->setItem(seciliRowSwitch,0,seciliRow1);
ipodListe->setItem(seciliRowSwitch,1,seciliRow2);
ipodListe->setItem(seciliRowSwitch,2,seciliRow3);
ipodListe->setItem(seciliRowSwitch,3,seciliRow4);

ipodListe->clearSelection();
ipodListe->selectRow(seciliRowSwitch);
ipodListe->setAutoScroll(TRUE);

}


void qPod::siralamayiYaz(){

	QStringList qliste;
	
	QStringList idBul;

		//siralamayi dogru yaz
	QString dosya=baglamaNoktasi+"/iPod_Control/.gnupod/GNUtunesDB.xml";
	QString qPodYedekDosya=baglamaNoktasi+"/iPod_Control/.gnupod/qPodDB.xml";

 	QFile file(dosya);
     	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, trUtf8("Hata"),
	trUtf8("Şarkı listesi değiştirilemiyor, bir hata oluştu!"),
	QMessageBox::Ok);
	return;
	}
	else{
	
	QFile::copy(dosya,qPodYedekDosya);
	QByteArray line;
	QStringList xmlIcerik;
	
	while (!file.atEnd()) {
	line = file.readLine();
	QString foo=QString::fromUtf8( line );

	if(foo.contains("<file ")){

	



	idBul=foo.split("id=\"");;
	idBul=idBul.at(1).split("\"");
	int atID=idBul.at(0).toInt();
	if(atID>0) qliste.insert(atID,foo);
	
	}
}

	
	

QFile file(baglamaNoktasi+"/iPod_Control/.gnupod/GNUtunesDB.xml"); //bosXML olustur
if (file.open(QIODevice::WriteOnly)) {
QTextStream stream(&file);
stream << "<?xml version='1.0' standalone='yes'?>\n<gnuPod>\n<files>\n";


for(int i=0;i<ipodListe->rowCount();i++){

int hangiID=ipodListe->item(i,3)->text().toInt();


stream << qliste.filter("id=\""+QString::number(hangiID)+"\"").at(0);
}
stream << "</files>\n</gnuPod>";

}
file.close();



	QStringList argumanlar2;
	argumanlar2 << "-m" << baglamaNoktasi;
	islem.start(gnupod_mktunes,argumanlar2);
	islem.waitForFinished();


	} 

}






