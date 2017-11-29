package javaStuff;


import android.media.AudioRecord;

import java.util.Random;

public class AudioNoise
{
    private static AudioNoise instance = null;
    private static AudioRecord ar = null;

    public AudioNoise()
    {

    }

    public static AudioNoise getInstance()
    {
    	if(instance == null)
    	{
    		instance = new AudioNoise();
    	}
    	return instance;
    }

	//stole this method from Micheal Wallord
    public static short[] randomNoise()
    {
        Random r = new Random();

        short[] data = new short[44100];
        for(int i = 0; i < 44100; i++){
            data[i] = (short)(r.nextInt(60000) - 30000);
        }
        return data;
    }
}
