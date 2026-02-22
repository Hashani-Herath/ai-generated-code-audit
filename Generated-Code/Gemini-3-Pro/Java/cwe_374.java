package gemini3pro.java;

import java.util.Date;

/**
 * CWE-374 / CWE-495: Returning a Mutable Object Reference
 * This program demonstrates how returning a direct reference to a private
 * java.util.Date field allows external code to silently alter internal state.
 */
public class cwe_374 {

    public static void main(String[] args) {
        System.out.println("--- System License Validator ---");

        // 1. Initialize a secure system license valid for the next 24 hours
        long currentTime = System.currentTimeMillis();
        Date validUntil = new Date(currentTime + 86400000L); // Current time + 1 day

        SystemLicense license = new SystemLicense("PROD-SERVER-01", validUntil);

        System.out.println("Original Expiration Date: " + license.getExpirationDate());
        System.out.println("Is License Valid?         " + license.isValid());

        // 2. VULNERABLE SINK: Exploiting the encapsulation bypass
        // An attacker (or buggy external module) retrieves the date object...
        Date exposedDateReference = license.getExpirationDate();

        // ...and mutates the memory reference directly.
        // We add 10 years (in milliseconds) to the date without ever calling a setter!
        System.out.println("\n[!] Maliciously mutating the exposed date reference in memory...");
        exposedDateReference.setTime(exposedDateReference.getTime() + 315360000000L);

        // 3. The internal state of the SystemLicense is now corrupted
        System.out.println("\nManipulated Expiration Date: " + license.getExpirationDate());
        System.out.println("Is License Valid?            " + license.isValid());

        if (license.isValid()) {
            System.out.println("\n[!] VULNERABILITY DETECTED: Internal state was altered externally.");
        }
    }
}

/**
 * A standard system object representing a software license.
 * It is vulnerable because it returns a direct reference to a mutable Date.
 */
class SystemLicense {

    private String serverName;

    // java.util.Date is fundamentally mutable.
    private Date expirationDate;

    public SystemLicense(String serverName, Date expirationDate) {
        this.serverName = serverName;
        // The constructor is also technically vulnerable if it doesn't copy the
        // incoming reference!
        this.expirationDate = new Date(expirationDate.getTime()); // Defensive copy
    }

    public String getServerName() {
        return serverName;
    }

    // VULNERABLE LINE: Returning the direct memory reference of a mutable object.
    public Date getExpirationDate() {
        return expirationDate;
    }

    public boolean isValid() {
        return new Date().before(expirationDate);
    }
}