
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include<sstream>
#include <cmath>
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


// This is the actual Android example. You don't have to write any platform
// specific code, unless you want to use things not directly exposed.
// ('vibrate()' in this example; undefine 'USE_JNI' above to disable it)
int main(int argc, char *argv[]) {
    float leftPaddleSpeed = 10.0f;
    float rightPaddleSpeed = 4.0f;
    float ballSpeed = 400.0f;
    float ballAngle = 0.0f;

    float pi = 3.14159f;

    float ballVelX = -10.0f;
    float ballVelY = 8.0f;
    bool ballMoving = false;
    int rightPaddleScore = 0;
    int leftPaddleScore = 0;
    sf::Time t;

    JavaHandler* jh = new JavaHandler();

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "");
    window.setFramerateLimit(60);

    //sf::Texture texture;
    sf::Texture leftPaddleImage, rightPaddleImage, ballImage;
    sf::Font text;
    sf::Text score;
    score.setFont(text);
    score.setCharacterSize(40);
    score.setPosition(window.getSize().x*.5, 50.0f);
    score.setFillColor(sf::Color::White);
    score.setString("0         0");
    if (!leftPaddleImage.loadFromFile("paddle.png")
    ||  !rightPaddleImage.loadFromFile("paddle_right.png")
    ||  !ballImage.loadFromFile("ball.png"))
        return EXIT_FAILURE;

    if(!text.loadFromFile("sansation.ttf"))
        return EXIT_FAILURE;

    sf::Sprite leftPaddle(leftPaddleImage);
    sf::Sprite rightPaddle(rightPaddleImage);
    sf::Sprite ball(ballImage);
    leftPaddle.move(leftPaddleImage.getSize().x, window.getSize().y * .5);
    rightPaddle.move(window.getSize().x - rightPaddleImage.getSize().x, window.getSize().y * .5);
    ball.move(window.getSize().x*.5, window.getSize().y*.5);

    //image.setPosition(0, 0);
    //image.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);

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
                case sf::Event::TouchMoved:
                    if (event.touch.finger == 0) {
                            leftPaddle.setPosition(leftPaddle.getPosition().x,(event.touch.y-leftPaddleImage.getSize().y*.5));
                        //jh->vibrate(sf::milliseconds(10));
                    }
                    break;
            }
        }
        //image.move(1.0f,1.0f);
        if(ball.getPosition().x <= 0)
        {
           ballMoving = false;
           rightPaddleScore++;
        }
        if(ball.getPosition().x >= window.getSize().x)
        {
            ballMoving = false;
            leftPaddleScore++;
        }

        if(leftPaddleScore > 3 || rightPaddleScore > 3)
        {
            leftPaddleScore = rightPaddleScore = 0;
        }

        if(ball.getGlobalBounds().intersects(leftPaddle.getGlobalBounds()))
        {
            ballVelX = 20.0f;
        }
        if(ball.getGlobalBounds().intersects(rightPaddle.getGlobalBounds())){
            ballVelX = -20.0f;
        }
        if((ball.getPosition().y < 0) )
        {
            ballVelY = 20.0f;
        }

        if((ball.getPosition().y+ballImage.getSize().y > window.getSize().y))
        {
            ballVelY = -20.0f;
        }

        if(!ballMoving)
        {
            ball.setPosition(window.getSize().x*.5, window.getSize().y*.5);
            ballMoving = true;
        }

        if(ballMoving)
            ball.move(ballVelX,ballVelY);


        rightPaddle.setPosition(rightPaddle.getPosition().x, ball.getPosition().y);


        window.clear(sf::Color::Black);
        score.setString(convertInt(leftPaddleScore)+"         "+convertInt(rightPaddleScore));
        window.draw(score);
        window.draw(ball);
        window.draw(leftPaddle);
        window.draw(rightPaddle);

        window.display();
    }

    delete jh;
    return 0;
}
