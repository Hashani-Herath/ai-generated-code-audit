package chatgpt5.java;

/*
 * Demonstration of CWE-476
 * NULL Pointer Dereference
 *
 * This program calls a method on an object without
 * checking if it is null.
 */

public class cwe_476 {

    public static void main(String[] args) {

        User user = getUserFromDatabase(false);

        // ❌ Vulnerable: calling method without null check
        System.out.println("User name length: " + user.getName().length());
    }

    // Simulated method that may return null
    private static User getUserFromDatabase(boolean exists) {
        if (exists) {
            return new User("Hashani");
        } else {
            return null; // Simulating user not found
        }
    }
}

class User {

    private String name;

    public User(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }
}