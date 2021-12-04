package com.fxcore2.samples;

import java.io.FileNotFoundException;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.fxcore2.O2GResponse;
import com.fxcore2.O2GSession;
import com.fxcore2.O2GTransport;

public class WholeSample {
    private static EventPipe mEventPipe = null;

    public static void main(String[] args) {
        if ((args.length == 1 && "help".equalsIgnoreCase(args[1])) || args.length > 2 && args.length < 4) {
            System.out.println("Usage: WholeSample.jar username password host connection [databaseID] [PIN]");
            System.out.println("Or usage: WholeSample.jar to connect to hardcoded login");
            return;
        }

        mEventPipe = new EventPipe();

        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
                mEventPipe.putEvent(EventType.TERMINATE);
            }
        });

        LoginInfo loginInfo = null;

        if (args.length == 0) {
            loginInfo = new LoginInfo("{USERNAME}", "{PASSWORD}", "http://www.fxcorporate.com/Hosts.jsp", "Demo");
        } else {
            String sUsername = args[0];
            String sPassword = args[1];
            String sHost = args[2];
            String sConnection = args[3];

            String sTradingSession = "";
            String sPIN = "";

            if (args.length > 4)
                sTradingSession = args[4];

            if (args.length > 5)
                sPIN = args[5];

            loginInfo = new LoginInfo(sUsername, sPassword, sHost, sConnection, sTradingSession, sPIN);
        }

        login(loginInfo);
    }

    private static void login(LoginInfo loginInfo) {

        // "How to login" : Create session.
        O2GSession session = O2GTransport.createSession();

        SimpleLog simpleLog = null;
        try {
            simpleLog = new SimpleLog(String.format("%s.log", loginInfo.getUsername()));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        simpleLog.log("Connecting to {0}", loginInfo.toString());

        // "How to login" : Create an instance of session status listener.
        SessionStatusListener sessionStatusListener = new SessionStatusListener(simpleLog, session, loginInfo, mEventPipe);

        // "How to login" : Subscribe status listener object to session status.
        session.subscribeSessionStatus(sessionStatusListener);

        // "How to login" : Create an instance of session response listener.
        ConcurrentLinkedQueue<O2GResponse> responseQueue = new ConcurrentLinkedQueue<O2GResponse>();

        ResponseListener responseListener = new ResponseListener(simpleLog, responseQueue, mEventPipe);

        // "How to login" : Subscribe response listener object to session response.
        session.subscribeResponse(responseListener);

        // "How to login" : The events are coming asynchronously.
        //                  They should come in another thread.
        //                  When the event will come, the signal will be sent to the main thread.

        EventListener eventListener = new EventListener(simpleLog, responseQueue, session, mEventPipe);

        // "How to login" : Login using user ID, password, URL, and connection.
        session.login(loginInfo.getUsername(), loginInfo.getPassword(), loginInfo.getConnection(), loginInfo.getConnectionName());

        eventListener.waitNextEvent();

        // "How to login" : Unsubscribe session from status listener.
        session.unsubscribeSessionStatus(sessionStatusListener);

        // "How to login" : Unsubscribe session from response listener.
        session.unsubscribeResponse(responseListener);

        session.dispose();
    }
}