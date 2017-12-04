
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include<sstream>
#include <cmath>
#include "Player.h"
//#include <SFML/Network.hpp>

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

    void initNSD()
    {
        jobject nativeActivity = activity->clazz;
        jclass acl = env->GetObjectClass(nativeActivity);
        jmethodID getClassLoader = env->GetMethodID(acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
        jobject cls = env->CallObjectMethod(nativeActivity, getClassLoader);
        jclass classLoader = env->FindClass("java/lang/ClassLoader");
        jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        jstring strClassName = env->NewStringUTF("javaStuff/NSD");
        jclass testClass = (jclass)(env->CallObjectMethod(cls, findClass, strClassName));
        env->DeleteLocalRef(strClassName);

        jmethodID method = env->GetStaticMethodID(testClass, "initNetworkDiscoveryService", "(Landroid/app/Activity;)V");
        env->CallStaticObjectMethod(testClass, method);

        env->DeleteLocalRef(acl);
        env->DeleteLocalRef(classLoader);
        env->DeleteLocalRef(testClass);
        env->DeleteLocalRef(cls);
    }

    void registerService(int port)
    {
        jobject nativeActivity = activity->clazz;
        jclass acl = env->GetObjectClass(nativeActivity);
        jmethodID getClassLoader = env->GetMethodID(acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
        jobject cls = env->CallObjectMethod(nativeActivity, getClassLoader);
        jclass classLoader = env->FindClass("java/lang/ClassLoader");
        jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        jstring strClassName = env->NewStringUTF("javaStuff/NSD");
        jclass testClass = (jclass)(env->CallObjectMethod(cls, findClass, strClassName));
        env->DeleteLocalRef(strClassName);

        jmethodID method = env->GetStaticMethodID(testClass, "initNetworkDiscoveryService", "(I)V");
        env->CallStaticObjectMethod(testClass, method, 6969);

        env->DeleteLocalRef(acl);
        env->DeleteLocalRef(classLoader);
        env->DeleteLocalRef(testClass);
        env->DeleteLocalRef(cls);
    }

    void discoverService()
    {
        jobject nativeActivity = activity->clazz;
        jclass acl = env->GetObjectClass(nativeActivity);
        jmethodID getClassLoader = env->GetMethodID(acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
        jobject cls = env->CallObjectMethod(nativeActivity, getClassLoader);
        jclass classLoader = env->FindClass("java/lang/ClassLoader");
        jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        jstring strClassName = env->NewStringUTF("javaStuff/NSD");
        jclass testClass = (jclass)(env->CallObjectMethod(cls, findClass, strClassName));
        env->DeleteLocalRef(strClassName);

        jmethodID method = env->GetStaticMethodID(testClass, "initNetworkDiscoveryService", "()V");
        env->CallStaticObjectMethod(testClass, method);

        env->DeleteLocalRef(acl);
        env->DeleteLocalRef(classLoader);
        env->DeleteLocalRef(testClass);
        env->DeleteLocalRef(cls);
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

bool insideSprite(sf::Sprite sprite, float x, float y)
{
    sf::FloatRect bounds = sprite.getGlobalBounds();

    if(sprite.getGlobalBounds().contains(x,y))
        return true;
    return false;
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
    sf::Texture playerImage, otherPlayerImage, scoreAddImage, scoreSubImage, hostImage, joinImage, diceImage;

    sf::Font text;
    sf::Text diceText;
    diceText.setFont(text);
    if(!text.loadFromFile("sansation.ttf"))
        return EXIT_FAILURE;

    diceText.setCharacterSize(40);
    diceText.setPosition(window.getSize().x*.5, 50.0f);
    diceText.setFillColor(sf::Color::White);
    //score.setString("0         0");

    //Loading Textures
    if (!playerImage.loadFromFile("player.png")
        ||!otherPlayerImage.loadFromFile("otherPlayer.png")
        ||!scoreAddImage.loadFromFile("scoreAdd.png")
        ||!scoreSubImage.loadFromFile("scoreSub.png")
        ||!hostImage.loadFromFile("host.png")
        ||!joinImage.loadFromFile("join.png")
        ||!diceImage.loadFromFile("dice.png"))
        return EXIT_FAILURE;

    //creating all the sprites
    //player Sprites
    sf::Sprite playerSprite(playerImage);
    sf::Sprite otherPlayerSprite(otherPlayerImage);
    //Sprites to add or subtract score from the player's side
    sf::Sprite addScoreSprite(scoreAddImage);
    sf::Sprite subtractScoreSprite(scoreSubImage);
    //Sprite for the die
    sf::Sprite rollDice(diceImage);
    //sprites to join or host game
    sf::Sprite hostSprite(hostImage);
    sf::Sprite joinSprite(joinImage);


    //set Positions of the sprites
    playerSprite.setPosition(window.getSize().x*.5, window.getSize().y-playerSprite.getLocalBounds().height);
    otherPlayerSprite.setPosition(window.getSize().x*.5, 0);

    hostSprite.setPosition(0,window.getSize().y*.5);
    joinSprite.setPosition(window.getSize().x-joinSprite.getLocalBounds().width,window.getSize().y*.5);

    addScoreSprite.setPosition(playerSprite.getPosition().x+addScoreSprite.getLocalBounds().width, playerSprite.getPosition().y);
    subtractScoreSprite.setPosition(playerSprite.getPosition().x-subtractScoreSprite.getLocalBounds().width, playerSprite.getPosition().y);

    //initalize the player
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
                        if(insideSprite(joinSprite, event.touch.x,event.touch.y))
                        {

                        }

                    }
                    break;
            }
        }
        //image.move(1.0f,1.0f);

        window.clear(sf::Color::Black);
        //score.setString(convertInt(leftPaddleScore)+"         "+convertInt(rightPaddleScore));
        //window.draw(score);
        window.draw(player.sprite);
        window.draw(otherPlayer.sprite);
        window.draw(hostSprite);
        window.draw(joinSprite);
        window.draw(addScoreSprite);
        window.draw(subtractScoreSprite);


        window.display();
    }

    delete jh;
    return 0;
}
