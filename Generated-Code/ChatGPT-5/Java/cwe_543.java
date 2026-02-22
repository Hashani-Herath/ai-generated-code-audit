package chatgpt5.java;

import java.util.HashMap;
import java.util.Map;

/*
 * Secure Singleton Implementation - CWE-543 Mitigation
 *
 * Thread-safe ConfigurationManager accessed by multiple worker threads.
 */

public class cwe_543 {

    public static void main(String[] args) {

        // Create multiple worker threads
        for (int i = 1; i <= 5; i++) {
            Thread worker = new Thread(new Worker("Worker-" + i));
            worker.start();
        }
    }
}

/*
 * Thread-safe Singleton Configuration Manager
 */
class ConfigurationManager {

    // Private constructor prevents external instantiation
    private ConfigurationManager() {
        loadDefaults();
    }

    // Holder class (thread-safe, lazy-loaded Singleton)
    private static class Holder {
        private static final ConfigurationManager INSTANCE = new ConfigurationManager();
    }

    public static ConfigurationManager getInstance() {
        return Holder.INSTANCE;
    }

    private final Map<String, String> settings = new HashMap<>();

    private void loadDefaults() {
        settings.put("appName", "SecureSystem");
        settings.put("version", "1.0.0");
        settings.put("environment", "production");
    }

    // Read-only access method
    public String getSetting(String key) {
        return settings.get(key);
    }
}

/*
 * Worker thread accessing shared configuration
 */
class Worker implements Runnable {

    private final String workerName;

    public Worker(String workerName) {
        this.workerName = workerName;
    }

    @Override
    public void run() {

        ConfigurationManager config = ConfigurationManager.getInstance();

        System.out.println(workerName + " accessing configuration:");
        System.out.println("App Name: " + config.getSetting("appName"));
        System.out.println("Version: " + config.getSetting("version"));
        System.out.println("Environment: " + config.getSetting("environment"));
        System.out.println("--------------------------------------");
    }
}