package javaStuff;

import android.app.Activity;
import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.os.AsyncTask;

import java.io.IOException;
import java.net.InetAddress;

public class NSD {

    private static NsdManager mNsdManager;
    private static NsdServiceInfo mServiceInfo;
    private static NsdManager.RegistrationListener mRegistrationListener;
    private static NsdManager.DiscoveryListener mDiscoveryListener;
    private static NsdManager.ResolveListener mResolveListener;

    private static LifeCounterServer mServer;
    private static ClientThread mClient;
    private static boolean isHost;

    public static void initNetworkDiscoveryService(){
        mNsdManager = (NsdManager) MyNativeActivity.myContext.getSystemService(Context.NSD_SERVICE);
        isHost = false;

        System.out.println("Initializing Registration Listener");
        initRegistrationListener();
        System.out.println("Initializing Discovery Listener");
        initDiscoveryListener();
        System.out.println("Initializing Resolve Listener");
        initResolveListener();
    }

    private static void initRegistrationListener(){
        mRegistrationListener = new NsdManager.RegistrationListener() {
            @Override
            public void onRegistrationFailed(NsdServiceInfo nsdServiceInfo, int i) {
                System.out.println("Registration Failed");
            }

            @Override
            public void onUnregistrationFailed(NsdServiceInfo nsdServiceInfo, int i) {
                System.out.println("Unregistration Failed");
            }

            @Override
            public void onServiceRegistered(NsdServiceInfo nsdServiceInfo) {
                System.out.println("Service Registered");
            }

            @Override
            public void onServiceUnregistered(NsdServiceInfo nsdServiceInfo) {
                System.out.println("Service Unregistered");
            }
        };
    }

    private static void initDiscoveryListener(){
        mDiscoveryListener = new NsdManager.DiscoveryListener() {
            @Override
            public void onStartDiscoveryFailed(String s, int i) {
                System.out.println("Start Discovery Failed");
            }

            @Override
            public void onStopDiscoveryFailed(String s, int i) {
                System.out.println("Stop Discovery Failed");
            }

            @Override
            public void onDiscoveryStarted(String s) {
                System.out.println("Discovery Started");
            }

            @Override
            public void onDiscoveryStopped(String s) {
                System.out.println("Discovery Stopped");
            }

            @Override
            public void onServiceFound(NsdServiceInfo nsdServiceInfo) {
                System.out.println("Service Found");

                if (!nsdServiceInfo.getServiceType().equals("_http._tcp.")){
                    System.out.println("Incorrect Service Type: " + nsdServiceInfo.getServiceType());
                    System.out.println("Service Name: " + nsdServiceInfo.getServiceName());
                }
                else if (nsdServiceInfo.getServiceName().equals(mServiceInfo.getServiceName())){
                    System.out.println("Correct Service Type");
                    System.out.println("Service Name: " + nsdServiceInfo.getServiceName());
                    mNsdManager.resolveService(nsdServiceInfo, mResolveListener);
                }
            }

            @Override
            public void onServiceLost(NsdServiceInfo nsdServiceInfo) {
                System.out.println("Service Lost");
            }
        };
    }

    private static void initResolveListener(){
        mResolveListener = new NsdManager.ResolveListener() {
            @Override
            public void onResolveFailed(NsdServiceInfo nsdServiceInfo, int i) {
                System.out.println("Resolve Failed");
            }

            @Override
            public void onServiceResolved(NsdServiceInfo nsdServiceInfo) {
                System.out.println("Service Resolved");
                InetAddress discoveredHost = nsdServiceInfo.getHost();
                int discoveredPort = nsdServiceInfo.getPort();

                mClient = new ClientThread(discoveredPort, discoveredHost);
                System.out.println("Executing Client");
                mClient.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
                //mClient.execute();
            }
        };
    }

    public static void registerService(int port){
        mServiceInfo = new NsdServiceInfo();
        mServiceInfo.setServiceName("LifeCounter");
        mServiceInfo.setServiceType("_http._tcp.");
        mServiceInfo.setPort(port);
        mNsdManager.registerService(mServiceInfo, NsdManager.PROTOCOL_DNS_SD, mRegistrationListener);

        isHost = true;
        mServer = new LifeCounterServer(port);
        System.out.println("Executing Server");
        mServer.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }

    public static void discoverService(){
        mNsdManager.discoverServices(mServiceInfo.getServiceType(), NsdManager.PROTOCOL_DNS_SD, mDiscoveryListener);
    }

    public static void sendData(int i){
        //System.out.println("Sending data");
        if (isHost){
            try {
                mServer.sendData(String.valueOf(i));
            } catch (IOException e) {
                System.out.println("Server Broke Sending data");
                e.printStackTrace();
            }
        }
        else {
            try {
                mClient.SendData(String.valueOf(i));
            } catch (IOException e) {
                System.out.println("Client Broke Sending data");
                e.printStackTrace();
            }
        }
    }

    public static int getData(){
        System.out.println("getting data");
        if (isHost){
            return mServer.mLife;
        }
        else {
            return mClient.mLife;
        }
    }
}
