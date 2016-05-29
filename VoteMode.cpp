#include<opencv2/opencv.hpp>
#include<unistd.h>
#include<sys/time.h>
#include "Util.hpp"
#include "InteractiveObject.hpp"

int main(int argc, char **argv) {
    cv::Mat frame;
    cv::Mat orig;
    cv::Mat fore;

    time_t camAdaptationStartTime = time(NULL);
    bool camAdapted = false;

    std::vector <std::vector<cv::Point>> contours;

    EnableCameraAutoAdjust(GetVideoNum(argc, argv));
    cv::VideoCapture cap(GetVideoNum(argc, argv));
    cv::Ptr<cv::BackgroundSubtractor> bgsub;

    cvNamedWindow("Contador de Votos", CV_WINDOW_NORMAL);
    cvSetWindowProperty("Contador de Votos", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
    ScreenSize ss = GetScreenSize();
    cvResizeWindow("Contador de Votos", ss.width, ss.height);

    cap >> frame;

    const int squareSize = 150;
    const int squareMargin = 10;

    cv::Point leftOrigin(squareMargin, squareMargin);
    cv::Point rightOrigin(frame.size().width - squareSize - squareMargin, squareMargin);

    InteractiveObject left(cv::Rect(leftOrigin.x, leftOrigin.y, squareSize, squareSize));
    InteractiveObject right(cv::Rect(rightOrigin.x, rightOrigin.y, squareSize, squareSize));

    cv::Mat yesc = cv::imread("SWC.png", CV_LOAD_IMAGE_COLOR);
    cv::resize(yesc, yesc, cv::Size(squareSize, squareSize));

    cv::Mat yesbw = cv::imread("vader.jpg", CV_LOAD_IMAGE_COLOR);
    cv::resize(yesbw, yesbw, cv::Size(squareSize, squareSize));

    cv::Mat noc = cv::imread("STC.png", CV_LOAD_IMAGE_COLOR);
    cv::resize(noc, noc, cv::Size(squareSize, squareSize));

    cv::Mat nobw = cv::imread("spock.png", CV_LOAD_IMAGE_COLOR);
    cv::resize(nobw, nobw, cv::Size(squareSize, squareSize));

    cv::Mat vote = cv::imread("vote.png", CV_LOAD_IMAGE_COLOR);
    cv::resize(vote,vote, cv::Size(squareSize, squareSize));

   // VideoCapture video("Game.mp4");

    while(true) {
        cap >> frame;
        cv::flip(frame, frame, 1);
        frame.copyTo(orig);

	/*Mat game;
	video >> game;
	cv::resize(game,game, cv::Size(squareSize, squareSize));
	game.copyTo(orig(cv::Rect((rightOrigin.x + leftOrigin.x)/2, (rightOrigin.y + leftOrigin.y)/2 + 2*game.size().height , game.size().width, game.size().height)));*/

        if (camAdapted) {
            cv::blur(frame, frame, cv::Size(3, 3));
            cv::blur(frame, frame, cv::Size(5, 5));
            cv::erode( frame, frame, cv::Mat());
            cv::erode( frame, frame, cv::Mat());

            cv::Mat cropped;
            cv::Rect roi(0, 0, frame.size().width, 150);
            frame(roi).copyTo(cropped);

            cv::Mat dst;
            bgsub->apply(cropped, dst, 0.0001);

            cv::threshold(dst, dst, 230, 255, CV_THRESH_BINARY);
            cv::findContours(dst , contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


            std::vector<std::vector<cv::Point> >hull( contours.size() );
            for( int i = 0; i < contours.size(); i++ )
                {  cv::convexHull( cv::Mat(contours[i]), hull[i], false ); }

            int counter = 0;

            bool leftActive = false;
            bool rightActive = false;
            for( int i = 0; i< contours.size(); i++ )
            {
               cv::Scalar color( 255, 0, 255);
               if (cv::contourArea(hull[i]) > 600) {
                    cv::drawContours( orig, hull, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
                    counter++;
               }
            }

            left.ProcessHulls(hull);
            right.ProcessHulls(hull);

		 std::string text = "VOTE";
                cv::Size txtSz = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX, 4, 4, NULL);
                //cv::putText(orig, text, cv::Point(orig.size().width / 2 - txtSz.width /2, orig.size().height - 2* txtSz.height), cv::FONT_HERSHEY_PLAIN, 7, cv::Scalar(0, 255, 255), 4);


		vote.copyTo(orig(cv::Rect((rightOrigin.x + leftOrigin.x)/2, (rightOrigin.y + leftOrigin.y)/2, vote.size().width, vote.size().height)));


            if (!(left.IsActive() && right.IsActive())) {
                char buff[11];
                sprintf(buff, "%02d", left.GetCount());
                cv::putText(orig, buff, cv::Point(40, orig.size().height - 40), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 255, 0), 4);


                sprintf(buff, "%02d", right.GetCount());
                cv::Size txtSz = cv::getTextSize(buff, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
                cv::putText(orig, buff, cv::Point(orig.size().width - txtSz.width - 40, orig.size().height - 40), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 255, 0), 4);



                left.Draw(orig);
                right.Draw(orig);

                if (right.IsCounted()) {
                    yesc.copyTo(orig(cv::Rect(rightOrigin.x, rightOrigin.y, yesc.size().width, yesc.size().height)));
                }
                else {
                    yesbw.copyTo(orig(cv::Rect(rightOrigin.x, rightOrigin.y, yesbw.size().width, yesbw.size().height)));
                }

                if (left.IsCounted()) {
                    noc.copyTo(orig(cv::Rect(leftOrigin.x, leftOrigin.y, noc.size().width, noc.size().height)));
                }
                else {
                    nobw.copyTo(orig(cv::Rect(leftOrigin.x, leftOrigin.y, nobw.size().width, nobw.size().height)));

                }

		int totalVotes = right.GetCount() + left.GetCount();
		std::string out = "FotosVote/votacao" + std::to_string(totalVotes) + ".png";

		imwrite(out, orig);

            }
            else {
                left.Deactivate();
                right.Deactivate();

               /* std::string text = "Fraude!";
                cv::Size txtSz = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
                cv::putText(orig, text, cv::Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 0, 255), 4);*/
            }
        }
        else {
            if ((time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC) {
                    camAdapted = true;
                    bgsub = cv::createBackgroundSubtractorMOG2();
                    DisableCameraAutoAdjust(GetVideoNum(argc, argv));
            }
            else {
                std::string text = "Configurando...";
                cv::Size txtSz = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
                cv::putText(orig, text, cv::Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 - 2* txtSz.height), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 0, 255), 4);

                char buff[3];
                sprintf(buff, "%d", ADAPTATION_TIME_SEC - abs(time(NULL) - camAdaptationStartTime));
                txtSz = cv::getTextSize(buff, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
                cv::putText(orig, buff, cv::Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 ), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 0, 255), 4);
            }
        }

        imshow("Contador de Votos", orig);

        int key = cv::waitKey(30);
        if ((key & 0xFF) == 27) {
                exit(0);
        }
        else if ((key & 0xFF) == ' '){
                camAdapted = false;
                EnableCameraAutoAdjust(GetVideoNum(argc, argv));
                camAdaptationStartTime = time(NULL);
        }
        else if ((key & 0xFF) == 'c') {
                imwrite("out.png", orig);
                system("python sendmail.py");
        }



    }

    return 0;
}
