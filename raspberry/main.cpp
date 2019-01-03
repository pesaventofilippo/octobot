#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <wiringPi.h>
//#include <wiringSerial.h> // per comunicazione seriale UART
#include <wiringPiI2C.h> // per comunicazione i2c
#include <iostream>
using namespace std;
using namespace cv;

//GOTURN è buggato almeno in OpenCV 3.2, proviamo su 3.4.0 ora
string trackerTypes[6] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN" };
//creo un tracker, usiamo KCF per iniziare, che è il migliore in genere, anche MIL però non scherza, se la contendono
string trackerType = trackerTypes[2];
Ptr<Tracker> tracker;
Mat src, frame;
Size dim(300, 300);
char messaggio;
float x1, y11, bw;
float fact = 5;        //fattore di divergenza per l'invio di un'azione come il movimento a destra o sinistra
int fd = 1, k = 0, mex;
int main() {
	if (trackerType == "BOOSTING")
            tracker = TrackerBoosting::create();
        if (trackerType == "MIL")
            tracker = TrackerMIL::create();
        if (trackerType == "KCF")
            tracker = TrackerKCF::create();
        if (trackerType == "TLD")
            tracker = TrackerTLD::create();
        if (trackerType == "MEDIANFLOW")
            tracker = TrackerMedianFlow::create();
        if (trackerType == "GOTURN")
            tracker = TrackerGOTURN::create();
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		cout << "\nImpossibile aprire la webcam!\n";
		return 1;
	}
	//controllo pr la comunicazione seriale
	/*if ((fd = serialOpen("/dev/ttyUSB0", 9600)) < 0) { // per comunicazione seriale UART
		cout << "Impossibile aprire la comunicazione seriale\n";
		return 1;
	}*/
	if(wiringPiI2CSetup(0x39) == -1) { // 0x39 è indicativo, li ci va scritto l'indirizzo con cui il vostro Pi rileva l'Arduino nella comunicazione i2c
       		cout << "Impossibile aprire la comunicazione i2c\n";
        	return 1;
    	}
	cout << "---------------OCTOBOT---------------\n";
	bool ok = cap.read(frame);
	resize(frame, src, dim);
	namedWindow("PiCam", CV_WINDOW_AUTOSIZE);
	//prende il primo frame
	//scelta del tracking :
	//si sceglie la posizione dell'oggetto da tracciare in codice in alternativa
	Rect2d bbox(287, 23, 86, 320);
	//si fa scegliere manualmente con il mouse il tracking
	bbox = selectROI(src, false);
	rectangle(src, bbox, Scalar(255, 0, 0), 2, 1);
	imshow("PiCam", src);
	tracker->init(src, bbox); // inizializzazione tracker
	while (cap.read(frame)) {
		cap >> frame;
		resize(frame, src, dim);
		bool ok = tracker->update(src, bbox); // aggiornamento tracker
		if (ok) 
			rectangle(src, bbox, Scalar(255, 0, 0), 2, 1);
		else
			cout << "Tracking perso!";
		x1 = bbox.x;
		y11 = bbox.y;
		if (k > 0) {
			if (x1 < ((src.width / 2) - (bbox.width / 2) - fact)) // sinistra
				//messaggio = 's'; // per comunicazione seriale
				mex = 1; // per comunicazione i2c
			if ((x1 + bbox.width) > (src.width - (bbox.width / 2) + fact)) // destra
				//messaggio = 'd'; // per comunicazione seriale
				mex = 2; // per comunicazione i2c
			if (bbox.width < (bw - fact)) // avanti
				//messaggio = 'a'; // per comunicazione seriale
				mex = 3; // per comunicazione i2c
			if (bbox.width > (bw + fact)) // indietro
				//messaggio = 'i'; // per comunicazione seriale
				mex = 4; // per comunicazione i2c
		}
		//serialPutchar(fd, messaggio); // per comunicazione seriale
		wiringPiI2CWrite(fd, mex); // per comunicazione i2c
		bw = bbox.width;
		k++;
		mex = 0; // usiamo '0' per segnalare di rimanere fermo, nel caso l'oggetto sia centrato
		imshow("PiCam", src);
		waitKey(30);           //per dare il tempo allo schermo di aggiornarsi
	}
	return 0;
}
