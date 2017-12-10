package javaStuff;

import android.app.Application;
import android.app.NativeActivity;
import android.content.Context;
import android.os.Bundle;


public class MyNativeActivity extends Application {
    public static Context myContext;

    @Override
    public void onCreate() {
        super.onCreate();

        myContext = this.getApplicationContext();
    }
}
