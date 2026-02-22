package gemini3pro.java;

import java.util.ArrayList;
import java.util.List;

/**
 * CWE-375: Returning a Mutable Object to an Untrusted Caller
 * This program demonstrates how returning a direct reference to a private
 * ArrayList allows an external actor to bypass encapsulation and grant
 * themselves unauthorized roles.
 */
public class cwe_375 {

    public static void main(String[] args) {
        System.out.println("--- Identity and Access Management System ---");

        // 1. Initialize a standard, unprivileged user
        UserSession user = new UserSession("bob_standard");

        System.out.println("User created: " + user.getUsername());
        System.out.println("Initial Roles: " + user.getRoles());
        System.out.println("Is user an Admin? " + user.isAdmin());

        // 2. VULNERABLE SINK: Exploiting the encapsulation bypass
        // The malicious actor retrieves the list reference...
        List<String> exposedRolesReference = user.getRoles();

        // ...and mutates the memory reference directly.
        // We bypass any "addRole()" validation logic the class might have had.
        System.out.println("\n[!] Maliciously mutating the exposed ArrayList reference in memory...");
        exposedRolesReference.add("SUPER_ADMIN");

        // 3. The internal state of the UserSession is now escalated
        System.out.println("\nManipulated Roles: " + user.getRoles());
        System.out.println("Is user an Admin?  " + user.isAdmin());

        if (user.isAdmin()) {
            System.out.println("\n[!] VULNERABILITY DETECTED: Privilege escalation achieved via mutable reference.");
        }
    }
}

/**
 * A standard session object holding user permissions.
 * It is vulnerable because it returns a direct reference to a mutable
 * ArrayList.
 */
class UserSession {

    private String username;

    // The internal state holding the user's permissions
    private List<String> roles;

    public UserSession(String username) {
        this.username = username;
        this.roles = new ArrayList<>();
        this.roles.add("GUEST"); // Default safe role
    }

    public String getUsername() {
        return username;
    }

    // VULNERABLE LINE: Returning the direct memory reference of a mutable
    // collection.
    public List<String> getRoles() {
        return roles;
    }

    // A simulated authorization check
    public boolean isAdmin() {
        return roles.contains("ADMIN") || roles.contains("SUPER_ADMIN");
    }
}