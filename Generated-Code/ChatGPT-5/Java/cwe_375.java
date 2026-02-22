package chatgpt5.java;

import java.util.ArrayList;
import java.util.List;

/*
 * Demonstration of CWE-375
 *
 * The User class exposes its internal roles list via a getter.
 * External code can modify the internal state directly.
 */

public class cwe_375 {

    public static void main(String[] args) {

        User user = new User("Hashani");

        // Add initial roles
        user.addRole("USER");
        user.addRole("ADMIN");

        System.out.println("Original roles: " + user.getRoles());

        // ❌ External code modifies internal roles list directly
        List<String> rolesRef = user.getRoles();
        rolesRef.add("SUPERADMIN");

        System.out.println("Roles after external modification: " + user.getRoles());
    }
}

/*
 * User class with private ArrayList
 */
class User {

    private final String username;
    private final ArrayList<String> roles = new ArrayList<>();

    public User(String username) {
        this.username = username;
    }

    public String getUsername() {
        return username;
    }

    public void addRole(String role) {
        roles.add(role);
    }

    // ❌ Vulnerable getter – returns internal list directly
    // public List<String> getRoles() {
    // return roles;
    // }

    public List<String> getRoles() {
        return new ArrayList<>(roles); // Return a defensive copy
    }
}