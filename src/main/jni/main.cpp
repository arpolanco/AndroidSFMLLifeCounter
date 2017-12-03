
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<sstream>
#include <cmath>
#include <SFML/Network.hpp>
#include "Player.h"

// Do we want to showcase direct JNI/NDK interaction?
// Undefine this to get real cross-platform code.
#define USE_JNI

#if defined(USE_JNI)
// These headers are only needed for direct NDK/JDK interaction
#include <jni.h>
#include <android/native_activity.h>

// Since we want to get the native activity from SFML, we'll have to use an
// extra header here:
#include <SFML/System/NativeActivity.hpp>

#include <android/log.h>

class JavaHandler
{
public:
    JavaHandler()
    {
        activity = sf::getNativeActivity();
        vm = activity->vm;
        env = activity->env;
        JavaVMAttachArgs attachargs;
        attachargs.version = JNI_VERSION_1_6;
        attachargs.name = "NativeThread";
        attachargs.group = NULL;
        jint res = vm->AttachCurrentThread(&env, &attachargs);
    }
    ~JavaHandler()
    {
        vm->DetachCurrentThread();
    }
    void vibrate(sf::Time duration)
    {
        // Retrieve class information
        jclass natact = env->FindClass("android/app/NativeActivity");
        jclass context = env->FindClass("android/content/Context");

        // Get the value of a constant
        jfieldID fid = env->GetStaticFieldID(context, "VIBRATOR_SERVICE", "Ljava/lang/String;");
        jobject svcstr = env->GetStaticObjectField(context, fid);

        // Get the method 'getSystemService' and call it
        jmethodID getss = env->GetMethodID(natact, "getSystemService",
                                           "(Ljava/lang/String;)Ljava/lang/Object;");
        jobject vib_obj = env->CallObjectMethod(activity->clazz, getss, svcstr);

        // Get the object's class and retrieve the member name
        jclass vib_cls = env->GetObjectClass(vib_obj);
        jmethodID vibrate = env->GetMethodID(vib_cls, "vibrate", "(J)V");

        // Determine the timeframe
        jlong length = duration.asMilliseconds();

        // Bzzz!
        env->CallVoidMethod(vib_obj, vibrate, length);

        // Free references
        env->DeleteLocalRef(vib_obj);
        env->DeleteLocalRef(vib_cls);
        env->DeleteLocalRef(svcstr);
        env->DeleteLocalRef(context);
        env->DeleteLocalRef(natact);
    }
private:
    ANativeActivity* activity;
    JavaVM* vm;
    JNIEnv* env;
};




#endif

std::string convertInt(int number)
{
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}





// This is the actual Android example. You don't have to write any platform
// specific code, unless you want to use things not directly exposed.
// ('vibrate()' in this example; undefine 'USE_JNI' above to disable it)
int main(int argc, char *argv[]) {

    sf::Time t;
    srand(time(NULL));

    JavaHandler* jh = new JavaHandler();

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "");
    window.setFramerateLimit(60);

    //sf::Texture texture;
    sf::Texture leftPaddleImage, rightPaddleImage;
    sf::Font text;
    sf::Text score;
    score.setFont(text);

    score.setCharacterSize(40);
    score.setPosition(window.getSize().x*.5, 50.0f);
    score.setFillColor(sf::Color::White);
    score.setString("0         0");
    if (!leftPaddleImage.loadFromFile("paddle.png")
    ||  !rightPaddleImage.loadFromFile("paddle_right.png"))
        return EXIT_FAILURE;

    if(!text.loadFromFile("sansation.ttf"))
        return EXIT_FAILURE;


    //creating all the sprites
    //player Sprites
    sf::Sprite playerSprite(leftPaddleImage);
    sf::Sprite otherPlayerSprite(rightPaddleImage);
    //Sprites to add or subtract score from the player's side
    sf::Sprite addScore();
    sf::Sprite subtractScore();
    sf::Sprite rollDice();
    //sprites to join or host game
    sf::Sprite hostSprite();
    sf::Sprite joinSprite();


    Player player(playerSprite, 50, false);
    Player otherPlayer(otherPlayerSprite, 50, false);



    sf::Music music;
    //if (!music.openFromFile("canary.wav"))
    //    return EXIT_FAILURE;

    //music.play();

    sf::View view = window.getDefaultView();

    sf::Event event;
    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::Resized:
                    view.setSize(event.size.width, event.size.height);
                    view.setCenter(event.size.width / 2, event.size.height / 2);
                    window.setView(view);
                    break;
                case sf::Event::TouchBegan:
                    if (event.touch.finger == 0) {
                        //insert code to do hit detection on all the different sprites
                    }
                    break;
            }
        }
        //image.move(1.0f,1.0f);

        window.clear(sf::Color::Black);
        //score.setString(convertInt(leftPaddleScore)+"         "+convertInt(rightPaddleScore));
        window.draw(score);
        player.draw(window);
        otherPlayer.draw(window);

        window.display();
    }

    delete jh;
    return 0;
}
