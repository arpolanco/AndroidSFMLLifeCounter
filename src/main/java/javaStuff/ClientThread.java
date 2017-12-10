package javaStuff;


import android.os.AsyncTask;
import android.os.SystemClock;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;

public class ClientThread extends AsyncTask<Void, Void, Void> {

    private int mPort;
    private InetAddress mHost;
    private Socket mSocket;
    private BufferedReader mInputReader;
    private BufferedWriter mOutputWriter;

    public int mLife;

    public ClientThread(int port, InetAddress host){
        System.out.println("Creating client thread");
        mPort = port;
        mHost = host;
        mLife = 50;
    }

    @Override
    protected Void doInBackground(Void... voids) {
        try {
            System.out.println("Creating client socket");
            mSocket = new Socket(mHost, mPort);
            System.out.println("Success created client socket");
            mInputReader = new BufferedReader(new InputStreamReader(mSocket.getInputStream()));
            mOutputWriter = new BufferedWriter(new OutputStreamWriter(mSocket.getOutputStream()));

            while (true) {
                System.out.println("CLIENT: About to read data");
                String data = mInputReader.readLine();
                System.out.println("CLIENT: Read data");
                if (data == null) {
                    System.out.println("Client lost connection");
                    break;
                }

                System.out.println("CLIENT: new life = " + data);
                mLife = Integer.parseInt(data);
            }
        } catch (IOException e) {
            System.out.println("CLIENT: Broke for some reason");
            e.printStackTrace();
        }


        return null;
    }

    public void SendData(String data) throws IOException {
        System.out.println("Writing data to client socket");
        mOutputWriter.write(data + "\n");
        mOutputWriter.flush();
    }
}
