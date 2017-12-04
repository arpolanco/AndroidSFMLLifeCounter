package javaStuff;

import android.app.Activity;
import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;

import java.net.InetAddress;

public class NSD {

    private static NsdManager mNsdManager;
    private static NsdServiceInfo mServiceInfo;
    private static NsdManager.RegistrationListener mRegistrationListener;
    private static NsdManager.DiscoveryListener mDiscoveryListener;
    private static NsdManager.ResolveListener mResolveListener;

    private static InetAddress discoveredHost;
    private static int discoveredPort;

    public static void initNetworkDiscoveryService(Activity activity){
        mNsdManager = (NsdManager) activity.getSystemService(Context.NSD_SERVICE);

        initRegistrationListener();
        initDiscoveryListener();
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

                if (!nsdServiceInfo.getServiceType().equals("_http._tcp")){
                    System.out.println("Incorrect Service Type");
                }
                else if (nsdServiceInfo.getServiceName().equals(mServiceInfo.getServiceName())){
                    System.out.println("Correct Service Type");
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
                discoveredHost = nsdServiceInfo.getHost();
                discoveredPort = nsdServiceInfo.getPort();
            }
        };
    }

    public static void registerService(int port){
        mServiceInfo = new NsdServiceInfo();
        mServiceInfo.setServiceName("LifeCounter");
        mServiceInfo.setServiceType("_http._tcp");
        mServiceInfo.setPort(port);
        mNsdManager.registerService(mServiceInfo, NsdManager.PROTOCOL_DNS_SD, mRegistrationListener);
    }

    public static void discoverService(){
        mNsdManager.discoverServices(mServiceInfo.getServiceType(), NsdManager.PROTOCOL_DNS_SD, mDiscoveryListener);
    }
}
