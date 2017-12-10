package javaStuff;


import android.os.AsyncTask;
import android.speech.tts.Voice;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;


public class LifeCounterServer extends AsyncTask<Void, Void, Void> {

    private int mPort;
    private ArrayList<ClientHandler> mClients;
    private ServerSocket mSocket;
    public int mLife;

    public LifeCounterServer(int port){
        mPort = port;
        mClients = new ArrayList<>();
        mSocket = null;
        mLife = 50;
    }

    @Override
    protected Void doInBackground(Void... voids) {
        try {
            System.out.println("Creating Server Socket");
            mSocket = new ServerSocket(mPort);
            System.out.println("Waiting for Connections");

            while(true){
                Socket client = mSocket.accept();
                System.out.println("Received Connection");
                ClientHandler c = new ClientHandler(client, this);
                mClients.add(c);
                System.out.println(mClients.size());
                c.run();
            }
        } catch (IOException e) {
            System.out.println("Server Broke");
            e.printStackTrace();
        }


        return null;
    }

    protected void RemoveClient(Socket client){
        mClients.remove(client);
    }

    public void sendData(String data) throws IOException {
        for(ClientHandler c: mClients){
            c.mOutputWriter.write(data + "\n");
            c.mOutputWriter.flush();
        }
    }



    public class ClientHandler extends Thread{
        private Socket mClient;
        private LifeCounterServer mServer;
        private BufferedReader mInputReader;
        private BufferedWriter mOutputWriter;

        public ClientHandler(Socket client, LifeCounterServer server) throws IOException {
            System.out.println("Starting client handler");
            mClient = client;
            mServer = server;
            mInputReader = new BufferedReader(new InputStreamReader(mClient.getInputStream()));
            mOutputWriter = new BufferedWriter(new OutputStreamWriter(mClient.getOutputStream()));
        }

        @Override
        public void run() {
            super.run();

            System.out.println("Running Client Handler");
            while(true){
                try {
                    String data = mInputReader.readLine();
                    System.out.println("read data");
                    if (data == null){
                        System.out.println("Lost client");
                        mServer.RemoveClient(mClient);
                        break;
                    }
                    System.out.println("SERVER: new life = " + data);
                    mServer.mLife = Integer.parseInt(data);

                } catch (IOException e) {
                    System.out.println("Client Handler Broke");
                    e.printStackTrace();
                }

            }
        }
    }
}
